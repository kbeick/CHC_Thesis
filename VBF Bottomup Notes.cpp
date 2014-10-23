// COLLAPSE BF 4
// Build BF2 BVH
// Start at left most leaf 'L'
// If sibling has children
//     L.parent

// Recursion(Node curr){
//     if curr.children == NULL:  ( return )


//     Go to curr.left child
//     go to curr.right child
//     if curr.parent.numChildren <= 3
//         curr.parent.children += curr.left & curr.right
// }


/* 
 * Returns a vector list of the grandchildren - and if particular child doesn't have own children, that child - of BVH_Node 'curr'
 */
vector<Nodes> GetDescendentNodes(BVH_Node curr){
    std::vector<BVH_Node> v;
    for(int i=0; i<branching_factor; i++){
        if( curr.children[i] == NULL ) { continue; }

        bool hasGChild = false;
        for(int j=0; j<branching_factor; j++){
            if( curr.children[i].children[j] == NULL ) { continue; }

            hasGChild = true;
            /* ADD TO VECTOR */
            v.add(curr.children[i].children[j])
        }
        if( !hasGChild ){ v.add(curr.children[i]); }
    }
}


Recursion(node curr){
    if curr == NULL {continue}
    if (curr.numChildren != 0,4(BF)){
        /* find best way to get 4(BF) children */
        // consider all grandchildren's bboxes - find by grouping/splitting

        vector<Nodes> gchildren = GetDescendentNodes(curr)
        count = gchildren.count()

        BBox bboxes[MAX_BRANCHING_FACTOR];
        float total_area=total.surfaceArea();
        float box_costs[MAX_BRANCHING_FACTOR] = {};
        float new_cost = 0.0;
        int splitPoint[BF_1];

        for(int i = 1;   i < count-(2);  i+=1){   
        for(int j = i+1; j < count-(1);  j+=1){ 
        for(int k = j+1; k < count-(0);  k+=1){    
        /* ------------------------------------------------------------------------------------------------------------------------- */
            int loop[] = {0,i,j,k,count};

            /* Do not want a split that groups more than (branching_factor) nodes together */
            // bool tooBig = false
            // for(int lp=0; lp < branching_factor; lp++){
            //     tooBig = tooBig || (loop[lp+1]-loop[lp] > branching_factor );
            // }
            // if(tooBig){ continue; }


            for(int bf=0; bf<branching_factor; bf++){
                bboxes[bf].clear();
                /* assign the boxes */
                bboxes[bf] = grandchildren[loop[bf]].bbox;
                for(int tri_it=loop[bf]+1; tri_it<loop[bf+1]; tri_it++) { bboxes[bf].expandToInclude(grandchildren[tri_it].bbox); }
                /* Calc costs for each box */
                box_costs[bf] = (bboxes[bf].surfaceArea()/total_area)*bboxes[bf].count; 
            }
            /* Check if this configuration of boxes is better than previous */
            new_cost = getCosts(box_costs, branching_factor);
            if(new_cost < min_cost){   
                min_cost = new_cost;
                for(int bf=0; bf<branching_factor-1; bf++){  splitPoint[bf] = loop[bf+1]; }
            }
        /* ------------------------------------------------------------------------------------------------------------------------- */
        }
        }
        }
        /* use the splitPoints to create new children for curr */

        // curr.children[0].bbox = gchildren[0].bbox;
        // curr.children[0].triangle_count = gchildren[0].triangle_count
        curr.children[0] = gchildren[0];
        for(int i=1; i < splitPoint[0]; i++){
            // curr.children[0].bbox.expandToInclude(gchildren[i].bbox);
            // curr.children[0].triangle_count += gchildren[i].triangle_count;
            curr.children[0] = MergeNodes(curr.children[0], gchildren[i]);
        }


        // curr.children[1].bbox = gchildren[splitPoint[0]].bbox;
        // curr.children[1].triangle_count = gchildren[splitPoint[0]].triangle_count
        curr.children[1] = gchildren[splitPoint[0]];
        for(int i=splitPoint[0]+1; i < splitPoint[1]; i++){
            // curr.children[1].bbox.expandToInclude(gchildren[i].bbox);
            // curr.children[1].triangle_count += gchildren[i].triangle_count;
            curr.children[1] = MergeNodes(curr.children[1], gchildren[i]);
        }

        
        // curr.children[2].bbox = gchildren[splitPoint[1]].bbox;
        // curr.children[2].triangle_count = gchildren[splitPoint[1]].triangle_count
        curr.children[2] = gchildren[splitPoint[1]];
        for(int i=splitPoint[1]+1; i < splitPoint[2]; i++){
            // curr.children[2].bbox.expandToInclude(gchildren[i].bbox);
            // curr.children[2].triangle_count += gchildren[i].triangle_count;
            curr.children[2] = MergeNodes(curr.children[2], gchildren[i]);
        }

        
        // curr.children[3].bbox = gchildren[splitPoint[2]].bbox;
        // curr.children[3].triangle_count = gchildren[splitPoint[2]].triangle_count
        curr.children[3] = gchildren[splitPoint[2]];
        for(int i=splitPoint[2]+1; i < count; i++){
            // curr.children[3].bbox.expandToInclude(gchildren[i].bbox);
            // curr.children[3].triangle_count += gchildren[i].triangle_count;
            curr.children[3] = MergeNodes(curr.children[3], gchildren[i]);
        }

        /* SET PARENT POINTER */
        for(int i=0; i<branching_factor; i++){ curr->children[i]->parent = curr; }

    }
    for(1->BF){
        Recursion(curr->children[i])
    }

    /* TO DO - reset node ids */
    // inner_node_counter = 0;

}


COLLAPSE BF 4  Another option
Build BF2 BVH

/* Maybe don't do this part? */
enqueue tree, breadth first
reverse queue (front of queue is deepest leaf, end is root)

While (!queue.empty()){
    curr = queue.pop()

    if (curr.children == NULL || curr.parent==NULL){   continue    }

    if (curr.parent.numChildren + curr.numChildren - 1 <= 4(BF) )
        curr.parent.children += curr.left & curr.right
}

/* Go thru tree and find nodes w/o proper num children, fix em */
Recursion(root)









// BF = 4

// Node A = kd_get_any()
// Node B = kd_find_best_match(A)

// while(kd_size > 1){

//     C = kd_find_best_match(B)

//     if(A->id == C->id){ /* THEN A & B ARE A GOOD MATCH */

//         Node AB = MergeNodes(A,B)
//         C = kd_find_best_match(AB)
//         D = kd_find_best_match(C)

//         if(D->id == AB->id || A || B){ /* THEN A B & C ARE BEST MATCH */
//             Node ABC = MergeNodes(AB,C)
//             D = kd_find_best_match(ABC)
//             ABCD is a THE ANSWER!
//             A = ABCD
//             kd_insert3f(A)
//             Node B = kd_find_best_match(A)
//         }else{
//             A = C
//             B = D
//             /* AND START OVER */
//         }





//         // if(A->triangle_count + B->triangle_count <= LEAF_SIZE){
//         //     A = MergeNodes(A, B);
//         //     leafCount--;
//         // }else{ 

//         // }

//         // kd_insert3f(A);

//         // B = kd_find_best_match_with_sq(kd, A, &dist_sq_AB);

//     }else{
//         A = B;
//         B = C;
//     }



// }



// FOUND PROXIMATE PAIR OF NODES
//     ~Pair can be merged into one Node [number of tris < LEAF_SIZE]
//     ~Pair 



// BRUTE FORCE - FIND CLOSEST PAIR-> TRIP-> QUAD

// Find AB
// find C, closest to AB : C -> AB
//     if AB <-> C then find closest D to ABC
//         if ABC <-> D then ABCD done~~~
//         else
//     else ([ D -> C ])
//         if C -> D
//         else





// kd_of_




