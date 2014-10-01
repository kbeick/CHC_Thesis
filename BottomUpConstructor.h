#include "./kdtree-0.5.6/kdtree.c"

struct Pair {
    float distance;
    BVH_Node* A;
    BVH_Node* B;
};

class ComparePair {
public:
    bool operator()(struct Pair& p1, struct Pair& p2) // Returns true if p1.distance > p2.distance
    {
        return (p1.distance > p2.distance);
    }
};

BVH_Node* SetUpNodeFor_BottomUp(Triangle* triangles, int numTriangles)
{
    BVH_Node *node = new BVH_Node();
    node->triangle_count = numTriangles;
    node->triangles = triangles;

    //find the total bbox for the array
    node->bbox = triangles[0].bbox;
    for(int i=1; i<numTriangles; i++)
    {   
        node->bbox.expandToInclude(triangles[i].bbox);
    }

    return node;
}


/*
    nodes need an id (inner_node_counter ?)
    flatArrayCount ??
    node parent pointer
*/
void BuildBVH_bottomup(Triangle* triangles, BVH_Node **root, int count)
{
    struct kdtree *kd;
    struct kdres *set;
    float dist;
    int kd_size = 0;
    int new_id = 0;
    BVH_Node* InputNodes = new BVH_Node[count];

    /* PSEUDO CODE */
    // KDTree kd = new KDTree(InputPoints);
    // MinHeap heap = new MinHeap();
    // foreach A in InputPoints do {
    //  Cluster B = kd.findBestMatch(A);
    //  heap.add(d(A,B), new Pair(A,B));
    // }
    // while( kd.size() > 1 ) {
    //  Pair <A,B> = heap.removeMinPair();
    //  if (! kd.contains(A) ) {
    //      //A was already clustered with somebody
    //  } else if (! kd.contains(B) ) {
    //      //B is invalid, find new best match for A
    //      B = kd.findBestMatch(A);
    //      heap.add(d(A,B), new Pair(A,B));
    //  } else {
    //      kd.remove(A);
    //      kd.remove(B);
    //      Cluster C = new Cluster(A,B);
    //      kd.add(C);
    //      Cluster D = kd.findBestMatch(C);
    //      heap.add(d(C,D), new Pair(C,D));
    //  }
    // }

    /* The idea is to use a simple-to-build but lower quality clustering tree to bootstrap the consction of a higher quality tree */
    kd = kd_create(3);
    int triangles_per_node = 1;
    for(int t=0; t < numTriangles; t++){
        /* Make a BVH_Node for each triangle */
        BVH_Node* node = SetUpNodeFor_BottomUp(&triangles[t*triangles_per_node], triangles_per_node);
        // node->id = i;
        kd_insert3f(kd, node->triangles[0].c[0], node->triangles[0].c[1], node->triangles[0].c[2], node); // for nodes w/ multiple triangles, need to find 3D point differently
        // assert(kd_insert3f(kd, node->triangles[0].c[0], node->triangles[0].c[1], node->triangles[0].c[2], node)); // for nodes w/ multiple triangles, need to find 3D point differently
        // cerr << "added node at " << node->triangles[0].c[0] << ", " << node->triangles[0].c[1] << ", " <<node->triangles[0].c[2] << endl;

        node->id = new_id++;
        kd_size++;
        leafCount++;
        InputNodes[t] = *node;
        // cerr << "kd_size = " << kd_size << endl;
    }
    /* Maybe the best option is to merge nodes to get ones with mult triangles at this point. Similar process to while loop below,
        merging neighbor nodes to get nodes w/ 4 trianges instead of making neighboring child of the same parent. */


    /* The 'min heap' stores the best match for each active cluster along with the corresponding distance */
    std::priority_queue<Pair, vector<Pair>, ComparePair> pq;
    for(int i=0; i<numTriangles; i++){
        BVH_Node *A = &InputNodes[i];
        set = kd_nearest_range3f(kd, A->bbox.center.x, A->bbox.center.y, A->bbox.center.z, 3.0);
        // cerr << "A B res size " << kd_res_size(set) << endl;

        BVH_Node *B = kd_res_item_data(set);
        while(B->id == A->id){
            if(kd_res_next(set)){
                B = kd_res_item_data(set);
            }else{cerr << "well crapo. BottomUpConstructor line ~100" << endl;}
        }

        // cerr << "A's center is " << A->bbox.center << endl;
        // cerr << "A: " << *A;
        // cerr << "B: " << *B;

        //get distance between A,B
        dist = sqrt(SQ(A->bbox.center.x - B->bbox.center.x) + SQ(A->bbox.center.y - B->bbox.center.y) + SQ(A->bbox.center.z - B->bbox.center.z));

        Pair newPair = {dist, A, B};
        pq.push(newPair);
        // cerr << "pushed new pair" << endl;
        kd_res_free(set);
    }

    // cerr << "done pushing pairs" << endl;

    while(kd_size > 1){

        // cerr << "\nkd size = " << kd_size << endl;

        struct kdnode *foundA = (kdnode *)malloc(sizeof *foundA);
        struct kdnode *foundB = (kdnode *)malloc(sizeof *foundB);
        struct Pair pair = pq.top();
        pq.pop();

        // cerr << "pair.A: " << *pair.A;
        // cerr << "pair.B: " << *pair.B;

        if(! kd_contains(kd, pair.A, &foundA)){
            // cerr << "A not in kd" << endl;
            //A was already clustered with somebody
        }else if(! kd_contains(kd, pair.B, &foundB)){
            // cerr << "B invalid, find new match" << endl;
            //B is invalid, find new best match for A
            set = kd_nearest_range3f(kd, pair.A->bbox.center.x, pair.A->bbox.center.y, pair.A->bbox.center.z, 3.0);
            BVH_Node *C = kd_res_item_data(set);
            // cerr << "pair.A->id " << pair.A->id << ", C->id " << C->id << endl;
            while(pair.A->id == C->id){
                if(kd_res_next(set)){
                    C = kd_res_item_data(set);
                }else{cerr << "well crapo. BottomUpConstructor line ~140" << endl;}
            }
            // cerr << "NOW pair.A->id " << pair.A->id << ", C->id " << C->id << endl;
            dist = sqrt(SQ(pair.A->bbox.center.x - C->bbox.center.x) + SQ(pair.A->bbox.center.y - C->bbox.center.y) + SQ(pair.A->bbox.center.z - C->bbox.center.z));
            Pair newPair = {dist, pair.A, C};
            pq.push(newPair);
        } else {
            // cerr << "proceed as normal" << endl;

            // cerr << "foundA: " << foundA->data << endl;
            // cerr << "foundB: " << foundB->data << endl;

            foundA->deleted = true;
            foundB->deleted = true;
            kd_size -= 2;

            // cerr << "deleted A and B" << endl;

            BVH_Node *C = new BVH_Node();
            C->id = new_id++;
            C->children[0] = pair.A;
            C->children[1] = pair.B;
            C->parent = NULL;
            C->triangle_count = pair.A->triangle_count + pair.B->triangle_count;
            C->bbox = pair.A->bbox;
            C->bbox.expandToInclude(pair.B->bbox);
            pair.A->parent = C;
            pair.B->parent = C;

            /* GIVE FINAL IDs */
            if(pair.A->children[0]==NULL && pair.A->children[1]==NULL ){
                // LEAF
                pair.A->id = -1;
            }else{
                pair.A->id = inner_node_counter++;
            }
            if(pair.B->children[0]==NULL && pair.B->children[1]==NULL ){
                // LEAF
                pair.B->id = -1;
            }else{
                pair.B->id = inner_node_counter++;
            }


            // assert(kd_insert3f(kd, C->bbox.center.x, C->bbox.center.y, C->bbox.center.z, C));
            kd_insert3f(kd, C->bbox.center.x, C->bbox.center.y, C->bbox.center.z, C);
            kd_size++;
            // cerr << "added new node to kd" << endl;

            if(kd_size <= 1){ 
                C->id = inner_node_counter++;
                continue;
            }

            set = kd_nearest_range3f(kd, C->bbox.center.x, C->bbox.center.y, C->bbox.center.z, 10.0);
            // cerr << "C D res size " << kd_res_size(set) << endl;
            BVH_Node *D = kd_res_item_data(set);
            // cerr << "C->id " << C->id << ", D->id " << D->id << endl;
            while(D->id == C->id){
                if(kd_res_next(set)){
                    D = kd_res_item_data(set);
                }else{cerr << "well crapo. BottomUpConstructor line ~180" << endl;}
            }
            // cerr << "NOW C->id " << C->id << ", D->id " << D->id << endl;

            if(D != NULL){
                //get distance between C,D
                dist = sqrt(SQ(C->bbox.center.x - D->bbox.center.x) + SQ(C->bbox.center.y - D->bbox.center.y) + SQ(C->bbox.center.z - D->bbox.center.z));

                struct Pair newPair = {dist, C, D};
                pq.push(newPair);
            }
            kd_res_free(set);
        }
    }

    /* Only un-deleted BVH_Node in kd_tree is the root now */
    set = kd_nearest3f(kd, 0, 0, 0);
    *root = kd_res_item_data(set);

    /* CLEAN UP */
    kd_res_free(set);
    kd_free(kd);
    // Get rid of InputNodes   TODO

}