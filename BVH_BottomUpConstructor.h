/*  
Bottom-up construction inspired by:
http://iss.ices.utexas.edu/Publications/Papers/RT2008.pdf
*/

#include "./kdtree-0.5.6/kdtree.c"
#include <vector>
#include <stack>
#include <queue>

#include "BVH.h"


BVH_Node* SetUpNodeFor_BottomUp(Triangle* triangles)
{
    /* ONE TRIANGLE PER NODE TO START */
    BVH_Node *node = new BVH_Node();
    node->triangle_count = 1;
    node->triangles = triangles;
    node->bbox = triangles[0].bbox;

    return node;
}

/* Makes the given BVH_Nodes (A,B) children of a new BVH_Nodes. return it */
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

/* 
 * Combines the properties + attributes of two BVH_Nodes into one 
 * Resulting Node has NULL parent pointer,
 */
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


void BuildBVH_bottomup(Triangle* triangles, BVH_Node **root, int count)
{
    /* PSEUDO CODE - locally-ordered agglomerative cluserting */
    /* (http://iss.ices.utexas.edu/Publications/Papers/RT2008.pdf) */

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

    /* Make a BVH_Node for each triangle */
    for(int t=0; t < numTriangles; t++){
        BVH_Node* node = SetUpNodeFor_BottomUp(&triangles[t]);
        kd_insert3f(kd, node->bbox.center.x, node->bbox.center.y, node->bbox.center.z, node);
        // cerr << "added node at " << node->triangles[0].c[0] << ", " << node->triangles[0].c[1] << ", " <<node->triangles[0].c[2] << endl;

        node->id = new_id++;
        leafCount++;
        kd_size++;
        InputNodes[t] = *node;
    }

    cerr << "done making nodes" << endl;

    /* GET ANY NODE */
    set = kd_nearest3f(kd, 0, 0, 0);
    BVH_Node *A =  kd_res_item_data(set);
    BVH_Node *B = kd_find_best_match_with_sq(kd, A, &dist_sq_AB);

    while(kd_size > 1){
        BVH_Node *C = kd_find_best_match_with_sq(kd, B, &dist_sq_BC);

        if(A->id == C->id || dist_sq_AB <= dist_sq_BC){ //In the exceptional case that three clusters are exactly equally good matches (i.e. d(A,B) = d(B,C) = d(C,A)), the pseudocode
            // could possibly run into an infinite loop, but this is easily fixed. One can change the condition to allow A and B to be clustered
            // as long as C is no better of a match (i.e. d(A,B) ≤ d(B,C)).
            kd_contains(kd, A, &foundA); // get the kd node for A
            kd_contains(kd, B, &foundB); // get the kd node for B
            /* NOTE
                kd nodes are not actually removed from the tree,
                they are just marked as "deleted." 
                Thus the kd tree gets VERY cluttered with big sets
            */
            foundA->deleted = true; // "delete" the kd node for A
            foundB->deleted = true; // "delete" the kd node for B
            kd_size -= 2;

            /* if these can combine to make a new leaf node, do it */
            if(A->triangle_count + B->triangle_count <= LEAF_SIZE){
                A = MergeNodes(A, B);
                leafCount--;
            }else{ /* else make em children of a new parent node */
                A = ProduceParentNode(A, B, &new_id);
                new_id++;
            }

            kd_insert3f(kd, A->bbox.center.x, A->bbox.center.y, A->bbox.center.z, A);
            kd_size++;

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
    /* CLEAN UP */
    kd_res_free(set);
    kd_free(kd);
    // Get rid of InputNodes   TODO
}


/*
 * Not very efficiently written, but it'll do
 */
void PostProcessBottomUp(BVH_Node *node)
{
    if(node == NULL){return;}
    bool hasChildren = false;

    // cerr << "PostProcessBottomUp" << endl;

    for(int i=0; i<branching_factor; i++){
        if(node->children[i] != NULL){
            hasChildren = true;
        }
    }
    if(!hasChildren){
        node->id = -1;
        return;
    }

    node->id = inner_node_counter++;
    // cerr << "node id is " << node->id << endl;

    for(int i=0; i<branching_factor; i++){
        PostProcessBottomUp(node->children[i]);
    }

    if(hasChildren){
        for(int i=0; i<branching_factor; i++){
            if(node->children[i] == NULL){
                node->children[i] = new BVH_Node();
                node->children[i]->id = -1;
                node->children[i]->bbox = *EMPTY_NODE_BBOX;
                node->children[i]->triangle_count = 0;
                node->children[i]->parent = node;
                leafCount++;
                emptyNodeCount++;
            }
        }
    }

}


void giveChildrenToParent(BVH_Node* parent, std::vector<BVH_Node*> parentsChildren, int numParentsChildren, std::vector<BVH_Node*> children, int numChildren)
{
    for (int i = 0; i < numChildren; ++i)
    {
        parent->children[i] = children[i];
        parent->children[i]->parent = parent;
    }

    for (int i = 0; i < numParentsChildren; ++i)
    {
        parent->children[numChildren + i] = parentsChildren[i];
        parent->children[numChildren + i]->parent = parent;
    }

    for (int i = numChildren+numParentsChildren; i < MAX_BRANCHING_FACTOR; ++i)
    {
        parent->children[i] = NULL;
    }
}


/*
 * Takes the BVH and collapses the tree such that each node has at most (branching_factor) children
 */
void BVH_Bottomup_Collapser(BVH_Node *root)
{
    if(branching_factor != 4 && branching_factor != 8 ){ return; }
    if(root == NULL){ return; }

    std::queue<BVH_Node*> q;
    std::stack<BVH_Node*> stack;
    BVH_Node* curr;
    std::vector<BVH_Node*> children;
    std::vector<BVH_Node*> parentsChildren;
    
    int numChildren = 0;
    int numParentsChildren = 0; // NOT INCLUDING CURR!!!

    /* PSEUDO CODE */
    // push tree onto stack, breadth first
    // While (!stack.empty()){
    //     curr = stack.pop()
    //     if (curr.children == NULL || curr.parent==NULL){   continue    }
    //     if (curr.parent.numChildren + curr.numChildren - 1 <= 4(BF) )
    //         curr.parent.children += curr.left & curr.right
    // }

    /* Push tree onto stack, breadth first */
    q.push(root);
    while(!q.empty()){
        curr = q.front();
        q.pop();
        for (int i = 0; i < MAX_BRANCHING_FACTOR; ++i){
            if(curr->children[i] != NULL){  q.push(curr->children[i]);  }
        }
        stack.push(curr);
    }
    /* For each node... */
    while(!stack.empty()){
        for(int i=0; i<MAX_BRANCHING_FACTOR; i++){ 
            children.erase (children.begin(), children.end());
            parentsChildren.erase (parentsChildren.begin(), parentsChildren.end());
        }

        curr = stack.top();
        stack.pop();

        if(curr->parent == NULL){ continue; }
        /* Get children info */
        numChildren = 0;
        numParentsChildren = 0;
        for(int i=0; i<MAX_BRANCHING_FACTOR; i++){
            if(curr->children[i] != NULL){
                children.push_back(curr->children[i]);
                numChildren++;
            }
            if( curr->parent->children[i] == NULL 
                || curr->parent->children[i]->id == curr->id)
                { continue;
            }
            parentsChildren.push_back(curr->parent->children[i]);
            numParentsChildren++;
        }
        /* Decide if we want to give curr's children to curr's parent */
        if(numChildren==0){ continue; }
        if(numChildren + numParentsChildren <= branching_factor){
            /* we do! */
            giveChildrenToParent(curr->parent, parentsChildren, numParentsChildren, children, numChildren);
        }
    }

    /* Re-id nodes */
    inner_node_counter = 0;
    PostProcessBottomUp(root);
}
