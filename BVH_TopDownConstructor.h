/*  
Original source code borrowed from Matt Larsen; Univerisity of Oregon, 2014
Adapted for use by Kevin Beick in his thesis project "Analyzing Performance of Bounding Volume Hierarchies for Ray Tracing" 
*/


float getCosts(float box_costs[], int BF)
{
    float result = TRAVERSAL_COST;
    for(int f=0; f<BF; f++)
    {
        result += box_costs[f];
    }
    return result;
}


int CalculateBestAxis(Triangle* triangles, int count, BBox total, int* splitPoint)
{
    int return_axis;
    float min_cost = 100000000.f; // MAX_INT instead?
    
    for(int axis = 0; axis < 3; axis++){

        switch(axis){
            // Sort on X-axis
            case 0:{
                //cerr<<"Sort Begin"<<endl;
                std::sort(triangles, triangles+ count, compX);
                //cerr<<"Sort end"<<endl;
                break;
            }
            // Sort on Y-axis   
            case 1:{
                std::sort(triangles, triangles+ count, compY);
                break;
            }
            // Sort on Z-axis
            case 2:{
                std::sort(triangles, triangles+ count, compZ);
            }
        }

        BBox bboxes[MAX_BRANCHING_FACTOR];
        float total_area=total.surfaceArea();
        float box_costs[MAX_BRANCHING_FACTOR] = {};
        float new_cost = 0.0;

        int numDivs=200/branching_factor;       // Change numDivs based on BF, so higher BF doesn't take forever!
        int stride = count/numDivs;
        while(stride==0)
        {
            numDivs/=2;
            stride=count/numDivs;
        }

        switch(branching_factor){
            /* BRANCHING FACTOR OF 2 */
            case 2:{
                    for(int i=stride; i<count; i+=stride)
                    {   
                        /* ------------------------------------------------------------------------------------------------------------------------- */
                        int loop[] = {0,i,count};
                        for(int bf=0; bf<branching_factor; bf++){
                            bboxes[bf].clear();
                            /* Assign the boxes */
                            bboxes[bf] = triangles[loop[bf]].bbox;
                            for(int tri_it=loop[bf]+1; tri_it<loop[bf+1]; tri_it++) { bboxes[bf].expandToInclude(triangles[tri_it].bbox); }
                            /* Calc costs for each box */
                            box_costs[bf] = (bboxes[bf].surfaceArea()/total_area)*bboxes[bf].count;  
                        }

                        /* Check if this configuration of boxes is better than previous */
                        new_cost = getCosts(box_costs, branching_factor);

                        if(new_cost < min_cost){  
                            min_cost = new_cost;
                            return_axis = axis;
                            for(int bf=0; bf<branching_factor-1; bf++){  splitPoint[bf] = loop[bf+1]; }
                        }
                        /* ------------------------------------------------------------------------------------------------------------------------- */
                    }
                    break;
            }
            /* BRANCHING FACTOR OF 4 */
            case 4:{
                    for(int i = 0+stride;  i < count-(2*stride);  i+=stride){   
                    for(int j = i+stride;  j < count-(1*stride);  j+=stride){ 
                    for(int k = j+stride;  k < count-(0*stride);  k+=stride){    
                    /* ------------------------------------------------------------------------------------------------------------------------- */
                        int loop[] = {0,i,j,k,count};
                        for(int bf=0; bf<branching_factor; bf++){
                            bboxes[bf].clear();
                            /* Assign the boxes */
                            bboxes[bf] = triangles[loop[bf]].bbox;
                            for(int tri_it=loop[bf]+1; tri_it<loop[bf+1]; tri_it++) { bboxes[bf].expandToInclude(triangles[tri_it].bbox); }
                            /* Calc costs for each box */
                            box_costs[bf] = (bboxes[bf].surfaceArea()/total_area)*bboxes[bf].count; 
                        }
                        /* Check if this configuration of boxes is better than previous */
                        new_cost = getCosts(box_costs, branching_factor);
                        if(new_cost < min_cost){   
                            min_cost = new_cost;
                            return_axis = axis;
                            for(int bf=0; bf<branching_factor-1; bf++){  splitPoint[bf] = loop[bf+1]; }
                        }
                    /* ------------------------------------------------------------------------------------------------------------------------- */
                    }
                    }
                    }
                    break;
            }
            /* BRANCHING FACTOR OF 8 */
            case 8:{
                for(int a = 0+stride;  a < count-(6*stride);  a+=stride){ 
                for(int b = a+stride;  b < count-(5*stride);  b+=stride){ 
                for(int c = b+stride;  c < count-(4*stride);  c+=stride){   
                for(int d = c+stride;  d < count-(3*stride);  d+=stride){   
                for(int e = d+stride;  e < count-(2*stride);  e+=stride){ 
                for(int f = e+stride;  f < count-(1*stride);  f+=stride){    
                for(int g = f+stride;  g < count-(0*stride);  g+=stride){  
                /* ------------------------------------------------------------------------------------------------------------------------- */
                    int loop[] = {0,a,b,c,d,e,f,g,count};
                    for(int bf=0; bf<branching_factor; bf++){
                        bboxes[bf].clear();
                        /* Assign the boxes */
                        bboxes[bf] = triangles[loop[bf]].bbox;
                        for(int tri_it=loop[bf]+1; tri_it<loop[bf+1]; tri_it++) { bboxes[bf].expandToInclude(triangles[tri_it].bbox); }
                        /* Calc costs for each box */
                        box_costs[bf] = (bboxes[bf].surfaceArea()/total_area)*bboxes[bf].count; 
                    }

                    /* Check if this configuration of boxes is better than previous */
                    new_cost = getCosts(box_costs, branching_factor);
                    if(new_cost < min_cost){   
                        min_cost = new_cost;
                        return_axis = axis;
                        for(int bf=0; bf<branching_factor-1; bf++){  splitPoint[bf] = loop[bf+1]; }
                    }
                /* ------------------------------------------------------------------------------------------------------------------------- */
                }
                }
                }
                }
                }
                }
                }
            }
        }
    }
    return return_axis;
}


void BuildBVH_topdown(Triangle* triangles, BVH_Node *current, BVH_Node *parent,
                            int count, int depth)
{
    current->parent = parent;
    current->triangle_count = count;

    //find the total bbox for the array
    current->bbox=triangles[0].bbox;
    for(int i=1; i<count; i++)
    {   
        current->bbox.expandToInclude(triangles[i].bbox);
    }

    /* AT A LEAF, MAKE A LEAF NODE AND RETURN */
    if (count <= LEAF_SIZE){
        current->triangles = triangles;
        current->triangle_count=count;
        for(int i=0; i<MAX_BRANCHING_FACTOR; i++){ current->children[i] = NULL; }
        flatArrayCount += current->triangle_count+(NON_LEAF_SIZE-2);
        leafCount++;
        current->id=-1;
        return;
    }

    /* NOT A LEAF */   
    // Not at a leaf node, sort along an axis and recurse CalculateBestAxis(triangles, count,current->bbox)
    current->id = inner_node_counter;
    inner_node_counter++;
    flatArrayCount += NON_LEAF_SIZE;

    int BF_1 = branching_factor-1;
    int splitPoint[BF_1];
    for(int i=0; i<BF_1; i++){ splitPoint[i]=-1; }
    int bestAxis;

    bestAxis=CalculateBestAxis(triangles, count,current->bbox, splitPoint);

    switch(bestAxis)
    {   // Sort on X-axis
        case 0:{
            std::sort(triangles,triangles+ count, compX);

            break;
        }
        // Sort on Y-axis   
        case 1:{
            std::sort(triangles,triangles+ count, compY);

            break;
        }
        // Sort on Z-axis
        case 2:{
            std::sort(triangles,triangles+ count, compZ);
        }
    }

    for(int i=0; i<branching_factor; i++){
        // make some children!
        current->children[i] = new BVH_Node();
    }for(int i=branching_factor; i<MAX_BRANCHING_FACTOR; i++){
        // The unused children ...
        current->children[i] = NULL;
    }

    BuildBVH_topdown(triangles, current->children[0], current, splitPoint[0], depth+1);

    for(int i=1; i<branching_factor; i++){
        int split_size =  i<BF_1 ? splitPoint[i]-splitPoint[i-1] : count - splitPoint[i-1];
        BuildBVH_topdown(triangles+splitPoint[i-1], current->children[i], current, split_size, depth+1);
    }

    return;
}
