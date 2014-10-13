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

BVH_Node* SetUpNodeFor_BottomUp(Triangle* triangles)
{
    /* ONE TRIANGLE PER NODE TO START */
    BVH_Node *node = new BVH_Node();
    node->triangle_count = 1;
    node->triangles = triangles;
    node->bbox = triangles[0].bbox;

    // cerr << "box is\n" << node->bbox << endl;

    return node;
}

void MakeNewPair(struct Pair *pair, BVH_Node *A, BVH_Node *B)
{
    //get distance between A,B
    float dist = sqrt(SQ(A->bbox.center.x - B->bbox.center.x) + SQ(A->bbox.center.y - B->bbox.center.y) + SQ(A->bbox.center.z - B->bbox.center.z));
    Pair newPair = {dist, A, B};
    *pair = newPair;
}

void LoadHeap(BVH_Node* input, int size, struct kdtree* tree, std::priority_queue<Pair, vector<Pair>, ComparePair>* pq)
{
    for(int i=0; i<size; i++){
        BVH_Node *A = &input[i];
        BVH_Node *B = kd_find_best_match(tree, A);

        Pair newPair;
        // if(A->id <= B->id)  { MakeNewPair(&newPair, A, B); }
        // else                { MakeNewPair(&newPair, B, A); }
        MakeNewPair(&newPair, A, B); 
        pq->push(newPair);
        // cerr << "pushed new pair" << endl;
        // *input[i] = 0;
    }
}

BVH_Node* ProduceParentNode(BVH_Node *A, BVH_Node *B, int *newID)
{
    BVH_Node *C = new BVH_Node();

    C->id = (newID != NULL ? *newID : 0);
    C->children[0] = A;
    C->children[1] = B;
    C->parent = NULL;
    C->triangle_count = A->triangle_count + B->triangle_count;
    C->triangles = new Triangle[C->triangle_count];
    for(int t=0; t<A->triangle_count; t++){
        C->triangles[t] = A->triangles[t];
    }
    for(int t=0; t<B->triangle_count; t++){
        C->triangles[A->triangle_count+t] = B->triangles[t];
    }

    C->bbox = A->bbox;
    C->bbox.expandToInclude(B->bbox);
    A->parent = C;
    B->parent = C;

    /* GIVE FINAL IDs */
    if(A->children[0]==NULL && A->children[1]==NULL ){
        // LEAF
        A->id = -1;
    }else{
        A->id = inner_node_counter++;
    }
    if(B->children[0]==NULL && B->children[1]==NULL ){
        // LEAF
        B->id = -1;
    }else{
        B->id = inner_node_counter++;
    }

    return C;
}

BVH_Node* MergeNodes(BVH_Node* A, BVH_Node* B)
{
    BVH_Node *mergedNode = new BVH_Node();
    // mergedNode->id = -1;
    mergedNode->id = (-1)*min(A->id, B->id);
    mergedNode->parent = NULL;
    mergedNode->triangle_count = A->triangle_count + B->triangle_count;
    mergedNode->triangles = new Triangle[mergedNode->triangle_count];
    for(int t=0; t<A->triangle_count; t++){
        mergedNode->triangles[t] = A->triangles[t];
    }
    for(int t=0; t<B->triangle_count; t++){
        mergedNode->triangles[A->triangle_count+t] = B->triangles[t];
    }
    mergedNode->bbox = A->bbox;
    mergedNode->bbox.expandToInclude(B->bbox);

    return mergedNode;
}


void BuildBVH_bottomup_BETTER(Triangle* triangles, BVH_Node **root, int count)
{
    /* PSEUDO CODE */
    // KDTree kd = new KDTree(InputPoints);
    // Cluster A = kd.getAnyElement();
    // Cluster B = kd.findBestMatch(A);
    // while( kd.size() > 1 ) {
    //  Cluster C = kd.findBestMatch(B);
    //  if (A == C){
    //      kd.remove(A);
    //      kd.remove(B);
    //      A = new Cluster(A,B);
    //      kd.add(A);
    //      Cluster B = kd.findBestMatch(A);
    //  } else {
    //      A = B;
    //      B = C;
    //  }
    // }

    BVH_Node* InputNodes = new BVH_Node[count];
    struct kdres *set;
    float dist;
    double dist_sq_AB, dist_sq_BC;
    int new_id = 1;

    struct kdtree *kd;
    int kd_size= 0;

    struct kdnode *foundA = (kdnode *)malloc(sizeof *foundA);
    struct kdnode *foundB = (kdnode *)malloc(sizeof *foundB);

    kd = kd_create(3);

    for(int t=0; t < numTriangles; t++){
        /* Make a BVH_Node for each triangle */
        BVH_Node* node = SetUpNodeFor_BottomUp(&triangles[t]);
        // node->id = i;
        kd_insert3f(kd, node->bbox.center.x, node->bbox.center.y, node->bbox.center.z, node);
        // cerr << "added node at " << node->triangles[0].c[0] << ", " << node->triangles[0].c[1] << ", " <<node->triangles[0].c[2] << endl;

        node->id = new_id++;
        leafCount++;
        kd_size++;
        InputNodes[t] = *node;
    }

    /* GET ANY NODE */
    set = kd_nearest3f(kd, 0, 0, 0);
    BVH_Node *A =  kd_res_item_data(set);
    BVH_Node *B = kd_find_best_match_with_sq(kd, A, &dist_sq_AB);

    while(kd_size > 1){
        // cerr << "-------------------------------------\n------------------------------------- " << endl;
        cerr << "\nkd size = " << kd_size << endl;

        BVH_Node *C = kd_find_best_match_with_sq(kd, B, &dist_sq_BC);

        // cerr << "A: center " <<A->bbox.center << endl << A->bbox << endl;
        // cerr << "B: center " <<B->bbox.center << endl << B->bbox << endl;
        // cerr << "C: center " <<C->bbox.center << endl << C->bbox << endl;

        // cerr << "dist_sq_AB: " << dist_sq_AB << ". dist_sq_BC: " << dist_sq_BC << endl;

        if(A->id == C->id || dist_sq_AB <= dist_sq_BC){ //In the exceptional case that three clusters are exactly equally good matches (i.e. d(A,B) = d(B,C) = d(C,A)), the pseudocode
            // could possibly run into an infinite loop, but this is easily fixed. One can change the condition to allow A and B to be clustered
            // as long as C is no better of a match (i.e. d(A,B) ≤ d(B,C)).
            kd_contains(kd, A, &foundA); // get the kd node for A
            kd_contains(kd, B, &foundB); // get the kd node for B
            foundA->deleted = true; // "delete" the kd node for A
            foundB->deleted = true; // "delete" the kd node for B
            kd_size -= 2;

            // cerr << "A has " << A->triangle_count << " triangles. B has " << B->triangle_count << " triangles. " << endl;
            // cerr << "A: ";
            // for(int q=0; q<A->triangle_count; q++){
            //     cerr << A->triangles[q].id << " ,";
            // }
            // cerr << "\nB: ";
            // for(int q=0; q<B->triangle_count; q++){
            //     cerr << B->triangles[q].id << " ,";
            // }
            // cerr << endl;

            /* if these can combine to make a new leaf node, do it */
            if(A->triangle_count + B->triangle_count <= LEAF_SIZE){
                // cerr << "  merging" << endl;
                A = MergeNodes(A, B);
                leafCount--;
            }else{ /* else make em children of a new parent node */
                // cerr << "  make em children on new parent" << endl;
                A = ProduceParentNode(A, B, &new_id);
                new_id++;
            }

            kd_insert3f(kd, A->bbox.center.x, A->bbox.center.y, A->bbox.center.z, A);
            kd_size++;
            // cerr << "kd size is now " << kd_size << endl;

            // cerr << "new A:\n" << *A << endl;

            if(kd_size <= 1){ 
                A->id = inner_node_counter++;
                // cerr << "did this\n";

                *root = A;
                // cerr << "ROOT: \n" << **root << endl;

                continue;
            }

            B = kd_find_best_match_with_sq(kd, A, &dist_sq_AB);

        }else{
            // cerr << "A != C" << endl;
            A = B;
            B = C;
        }

    }

    /* Only non-deleted BVH_Node in kd_tree is the root now */
    // set = kd_nearest3f(kd, 0, 0, 0);
    // *root = kd_res_item_data(set);

    // cerr << "ROOT: \n" << **root << endl;

    /* CLEAN UP */
    kd_res_free(set);
    kd_free(kd);
    // Get rid of InputNodes   TODO
}


/*
    nodes need an id (inner_node_counter ?)
    flatArrayCount ??
    node parent pointer
*/
void BuildBVH_bottomup(Triangle* triangles, BVH_Node **root, int count)
{
    BVH_Node* InputNodes = new BVH_Node[count];
    struct kdres *set;
    float dist;
    int new_id = 1;

    struct kdtree *first_tree, *kd;
    int first_tree_size = 0;
    int kd_size= 0;

    struct kdnode *foundA = (kdnode *)malloc(sizeof *foundA);
    struct kdnode *foundB = (kdnode *)malloc(sizeof *foundB);

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
    first_tree = kd_create(3);
    kd = kd_create(3);
    for(int t=0; t < numTriangles; t++){
        /* Make a BVH_Node for each triangle */
        BVH_Node* node = SetUpNodeFor_BottomUp(&triangles[t]);
        // node->id = i;
        kd_insert3f(first_tree, node->triangles[0].c[0], node->triangles[0].c[1], node->triangles[0].c[2], node);
        // cerr << "added node at " << node->triangles[0].c[0] << ", " << node->triangles[0].c[1] << ", " <<node->triangles[0].c[2] << endl;

        node->id = new_id++;
        leafCount++;
        first_tree_size++;
        InputNodes[t] = *node;
    }

    /* The 'min heap' stores the best match for each active cluster along with the corresponding distance */
    std::priority_queue<Pair, vector<Pair>, ComparePair> first_pq;
    LoadHeap(InputNodes, first_tree_size, first_tree, &first_pq);
    // for(int i=0; i<numTriangles; i++){
    //     BVH_Node *A = &InputNodes[i];
    //     BVH_Node *B = kd_find_best_match(first_tree, A);

    //     // cerr << "A's center is " << A->bbox.center << endl;
    //     // cerr << "A: " << *A;
    //     // cerr << "B: " << *B;

    //     Pair newPair;
    //     MakeNewPair(&newPair, A, B);
    //     first_pq.push(newPair);
    //     // cerr << "pushed new pair" << endl;
    //     InputNodes[i] = NULL;
    // }
    cerr << "done pushing pairs" << endl;


    /* Maybe the best option is to merge nodes to get ones with mult triangles at this point. Similar process to while loop below,
        merging neighbor nodes to get nodes w/ 4 trianges instead of making neighboring child of the same parent. */
    while(first_tree_size > 1){ /* 1:1 triangles:nodes */

        // cerr << "first_tree_size = " << first_tree_size << endl;

        struct Pair pair = first_pq.top();
        first_pq.pop();

        // cerr << "\nPOPPED " << pair.A->id << " and " << pair.B->id << endl;

        if(! kd_contains(first_tree, pair.A, &foundA)){
            //A was already clustered with somebody
            // cerr << "A was already clustered with somebody" << endl;
        }else if(! kd_contains(first_tree, pair.B, &foundB)){
            //B is invalid, find new best match for A
            // cerr << "B invalid, find new match" << endl;
            BVH_Node *C = kd_find_best_match(first_tree, pair.A);
            Pair newPair;
            MakeNewPair(&newPair, pair.A, C);
            first_pq.push(newPair);
        } else {
            // cerr << "proceed as normal" << endl;
            foundA->deleted = true;
            foundB->deleted = true;
            first_tree_size -= 2;

            if(pair.A->triangle_count + pair.B->triangle_count <= LEAF_SIZE){
            /* Want to merge these nodes */
                // cerr << "  merging" << endl;
                BVH_Node *mergedNode = MergeNodes(pair.A, pair.B);
                leafCount--; /* merged two into one */

                if(mergedNode->triangle_count == LEAF_SIZE){
                    // cerr << "    proper leaf, goes into second phase" << endl;
                /* This is a proper leaf, add it to second phase datastructs */
                    mergedNode->id = kd_size;
                    kd_insert3f(kd, mergedNode->bbox.center.x, mergedNode->bbox.center.y, mergedNode->bbox.center.z, mergedNode);
                    InputNodes[kd_size++] = *mergedNode;
                }else{
                    // cerr << "  stays in first phase" << endl;
                    kd_insert3f(first_tree, mergedNode->bbox.center.x, mergedNode->bbox.center.y, mergedNode->bbox.center.z, mergedNode);
                    first_tree_size++;

                    BVH_Node *buddy = kd_find_best_match(first_tree, mergedNode);
                    if(buddy != NULL){
                        Pair newPair;
                        MakeNewPair(&newPair, mergedNode, buddy);
                        first_pq.push(newPair);
                    }
                }

            }else if(first_tree_size <= 0){
            /* If this pair is the last two nodes in the tree */
                // cerr << "  this is last two nodes in tree" << endl;
                kd_insert3f(kd, pair.A->triangles[0].c[0], pair.A->triangles[0].c[1], pair.A->triangles[0].c[2], pair.A);
                InputNodes[kd_size++] = *pair.A;
                kd_insert3f(kd, pair.B->triangles[0].c[0], pair.B->triangles[0].c[1], pair.B->triangles[0].c[2], pair.B);
                InputNodes[kd_size++] = *pair.B;

            /* We want to set the larger of the two nodes into the next pq, we're done with it in this phase */
            }else if(pair.A->triangle_count >= pair.B->triangle_count){
                // cerr << "  A goes into next phase, B stays in first tree/pq" << endl;
                /* A moves on, B goes into first tree/pq again */
                kd_insert3f(kd, pair.A->triangles[0].c[0], pair.A->triangles[0].c[1], pair.A->triangles[0].c[2], pair.A);
                InputNodes[kd_size++] = *pair.A;

                foundB->deleted = false;
                first_tree_size++;

                BVH_Node *buddy = kd_find_best_match(first_tree, pair.B);
                if(buddy != NULL){
                    Pair newPair;
                    MakeNewPair(&newPair, pair.B, buddy);
                    first_pq.push(newPair);
                }
            }else{
                // cerr << "  B goes into next phase, A stays in first tree/pq" << endl;
                /* B moves on, A goes into first tree/pq again */
                kd_insert3f(kd, pair.B->triangles[0].c[0], pair.B->triangles[0].c[1], pair.B->triangles[0].c[2], pair.B);
                InputNodes[kd_size++] = *pair.B;

                foundA->deleted = false;
                first_tree_size++;

                BVH_Node *buddy = kd_find_best_match(first_tree, pair.A);
                if(buddy != NULL){
                    Pair newPair;
                    MakeNewPair(&newPair, pair.A, buddy);
                    first_pq.push(newPair);
                }

            }
        }
    }
    if(first_tree_size==1){
        set = kd_nearest3f(first_tree, 0, 0, 0);
        BVH_Node *loner = kd_res_item_data(set); 
        kd_insert3f(kd, loner->triangles[0].c[0], loner->triangles[0].c[1], loner->triangles[0].c[2], loner);
        InputNodes[kd_size++] = *loner;
    }

    cerr << "\n\nStarting Phase 2: kd size is " << kd_size << endl;


    std::priority_queue<Pair, vector<Pair>, ComparePair> pq;
    LoadHeap(InputNodes, kd_size, kd, &pq);

    while(kd_size > 1){

        // cerr << "\nkd size = " << kd_size << endl;
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
            BVH_Node *C = kd_find_best_match(kd, pair.A);
            // cerr << "NOW pair.A->id " << pair.A->id << ", C->id " << C->id << endl;
            Pair newPair;
            MakeNewPair(&newPair, pair.A, C);
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

            kd_insert3f(kd, C->bbox.center.x, C->bbox.center.y, C->bbox.center.z, C);
            kd_size++;
            // cerr << "added new node to kd" << endl;

            if(kd_size <= 1){ 
                C->id = inner_node_counter++;
                continue;
            }

            BVH_Node *D = kd_find_best_match(kd, C);
            // cerr << "NOW C->id " << C->id << ", D->id " << D->id << endl;

            if(D != NULL){
                Pair newPair;
                MakeNewPair(&newPair, C, D);
                pq.push(newPair);
            }
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