
COLLAPSE BF 4
Build BF2 BVH

enqueue tree, breadth first
reverse queue (front of queue is deepest leaf, end is root)

While (!queue.empty()){
    curr = queue.pop()

    if (curr.children == NULL || curr.parent==NULL){   continue    }

    if (curr.parent.numChildren + curr.numChildren - 1 <= 4(BF) )
        curr.parent.children += curr.left & curr.right
}






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




