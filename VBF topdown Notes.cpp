stride
count



float getCosts(float box_costs[], int BF)
{
    float result = TRAVERSAL_COST;
    for(int i=0; i<BF; i++){ result += box_costs[i]; }
    return result;
}


BF == 2
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
    float new_cost = getCosts(box_costs, branching_factor)
    if(new_cost < min_cost){   
        min_cost = new_cost;
        return_axis = axis;
        for(int bf=0; bf<branching_factor-1; bf++){  splitPoint[bf] = loop[bf+1]; }
    }
    /* ------------------------------------------------------------------------------------------------------------------------- */
}


BF == 4
for(int i = stride;  i < count-(2*stride);  i+=stride)
{   
    for(int j = i+stride;  j < count-(1*stride);  j+=stride)
    { 
        for(int k = j+stride;  k < count-(0*stride);  k+=stride)
        {    
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
            float new_cost = getCosts(box_costs, branching_factor)
            if(new_cost < min_cost){   
                min_cost = new_cost;
                return_axis = axis;
                for(int bf=0; bf<branching_factor-1; bf++){  splitPoint[bf] = loop[bf+1]; }
            }
            /* ------------------------------------------------------------------------------------------------------------------------- */
        }
    }
}

BF == 8
for(int a = stride;  a < count-(6*stride);  a+=stride)
{   
    for(int b = a+stride;  b < count-(5*stride);  b+=stride)
    { 
        for(int c = b+stride;  c < count-(4*stride);  c+=stride)
        {   
            for(int d = c+stride;  d < count-(3*stride);  d+=stride)
            {   
                for(int e = d+stride;  e < count-(2*stride);  e+=stride)
                { 
                    for(int f = e+stride;  f < count-(1*stride);  f+=stride)
                    {    
                        for(int g = f+stride;  g < count-(0*stride);  g+=stride)
                        {  
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
                            float new_cost = getCosts(box_costs, branching_factor)
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



















BF == 2
for(int i=stride; i<count; i+=stride)
{   
    // DO TEH STUFF
    for(int bf=0; bf<branching_factor; bf++){
        bboxes[bf].clear();
    }

    /* Assign the boxes */
    bboxes[0] = triangles[0].bbox;
    for(int tri_it=0+1; tri_it<i; tri_it++) { bboxes[0].expandToInclude(triangles[tri_it].bbox); }

    bboxes[1] = triangles[i].bbox;
    for(int tri_it=i+1; tri_it<count; tri_it++) { bboxes[1].expandToInclude(triangles[tri_it].bbox); }

    /* Calc costs for each box */
    for(int sa=0; sa<branching_factor; sa++){  box_costs[sa] = (bboxes[sa].surfaceArea()/total_area)*bboxes[sa].count;  }

    /* Check if this configuration of boxes is better than previous */
    float new_cost = getCosts(box_costs, branching_factor)
    if(new_cost < min_cost){   
        min_cost = new_cost;
        return_axis = axis;
        splitPoint[0] = i;
    }
}


BF == 4
for(int i = stride;  i < count-(2*stride);  i+=stride)
{   
    for(int j = i+stride;  j < count-(1*stride);  j+=stride)
    { 
        for(int k = j+stride;  k < count-(0*stride);  k+=stride)
        {    
            // DO TEH STUFF
            for(int bf=0; bf<branching_factor; bf++){
                bboxes[bf].clear();
            }

            /* Assign the boxes */
            bboxes[0] = triangles[0].bbox;
            for(int tri_it=0+1; tri_it<i; tri_it++) { bboxes[0].expandToInclude(triangles[tri_it].bbox); }

            bboxes[1] = triangles[i].bbox;
            for(int tri_it=i+1; tri_it<j; tri_it++) { bboxes[1].expandToInclude(triangles[tri_it].bbox); }

            bboxes[2] = triangles[j].bbox;
            for(int tri_it=j+1; tri_it<k; tri_it++) { bboxes[2].expandToInclude(triangles[tri_it].bbox); }

            bboxes[3] = triangles[k].bbox;
            for(int tri_it=k+1; tri_it<count; tri_it++) { bboxes[3].expandToInclude(triangles[tri_it].bbox); }


            /* Calc costs for each box */
            for(int sa=0; sa<branching_factor; sa++){  box_costs[sa] = (bboxes[sa].surfaceArea()/total_area)*bboxes[sa].count;  }

            /* Check if this configuration of boxes is better than previous */
            float new_cost = getCosts(box_costs, branching_factor)
            if(new_cost < min_cost){   
                min_cost = new_cost;
                return_axis = axis;
                splitPoint[0] = i;
                splitPoint[1] = j;
                splitPoint[2] = k;
            }
        }
    }
}

BF == 8
for(int a = stride;  a < count-(6*stride);  a+=stride)
{   
    for(int b = a+stride;  b < count-(5*stride);  b+=stride)
    { 
        for(int c = b+stride;  c < count-(4*stride);  c+=stride)
        {   
            for(int d = c+stride;  d < count-(3*stride);  d+=stride)
            {   
                for(int e = d+stride;  e < count-(2*stride);  e+=stride)
                { 
                    for(int f = e+stride;  f < count-(1*stride);  f+=stride)
                    {    
                        for(int g = f+stride;  g < count-(0*stride);  g+=stride)
                        {  
                            // DO TEH STUFF
                            for(int bf=0; bf<branching_factor; bf++){
                                bboxes[bf].clear();
                            }

                            /* Assign the boxes */
                            bboxes[0] = triangles[0].bbox;
                            for(int tri_it=0+1; tri_it<a; tri_it++) { bboxes[0].expandToInclude(triangles[tri_it].bbox); }

                            bboxes[1] = triangles[a].bbox;
                            for(int tri_it=a+1; tri_it<b; tri_it++) { bboxes[1].expandToInclude(triangles[tri_it].bbox); }

                            bboxes[2] = triangles[b].bbox;
                            for(int tri_it=b+1; tri_it<c; tri_it++) { bboxes[2].expandToInclude(triangles[tri_it].bbox); }

                            bboxes[3] = triangles[c].bbox;
                            for(int tri_it=c+1; tri_it<d; tri_it++) { bboxes[3].expandToInclude(triangles[tri_it].bbox); }

                            bboxes[4] = triangles[d].bbox;
                            for(int tri_it=d+1; tri_it<e; tri_it++) { bboxes[4].expandToInclude(triangles[tri_it].bbox); }

                            bboxes[5] = triangles[e].bbox;
                            for(int tri_it=e+1; tri_it<f; tri_it++) { bboxes[5].expandToInclude(triangles[tri_it].bbox); }

                            bboxes[6] = triangles[f].bbox;
                            for(int tri_it=f+1; tri_it<g; tri_it++) { bboxes[6].expandToInclude(triangles[tri_it].bbox); }

                            bboxes[7] = triangles[g].bbox;
                            for(int tri_it=g+1; tri_it<count; tri_it++) { bboxes[7].expandToInclude(triangles[tri_it].bbox); }


                            /* Calc costs for each box */
                            for(int sa=0; sa<branching_factor; sa++){  box_costs[sa] = (bboxes[sa].surfaceArea()/total_area)*bboxes[sa].count;  }

                            /* Check if this configuration of boxes is better than previous */
                            float new_cost = getCosts(box_costs, branching_factor)
                            if(new_cost < min_cost){   
                                min_cost = new_cost;
                                return_axis = axis;
                                splitPoint[0] = a;
                                splitPoint[1] = b;
                                splitPoint[2] = c;
                                splitPoint[3] = d;
                                splitPoint[4] = e;
                                splitPoint[5] = f;
                                splitPoint[6] = g;
                            }
                        }
                    }
                }
            }
        }
    }
}





















BF == 2
for(int i=stride; i<count; i+=stride)
{   
    // DO TEH STUFF
}


BF == 4
for(int i = stride;  i < count-(2*stride);  i+=stride)
{   
    for(int j = i+stride;  j < count-(1*stride);  j+=stride)
    { 
        for(int k = j+stride;  k < count-(0*stride);  k+=stride)
        {   
            // DO TEH STUFF
        }
    }
}

BF == 8
for(int a = stride;  a < count-(6*stride);  a+=stride)
{   
    for(int b = a+stride;  b < count-(5*stride);  b+=stride)
    { 
        for(int c = b+stride;  c < count-(4*stride);  c+=stride)
        {   
            for(int d = c+stride;  d < count-(3*stride);  d+=stride)
            {   
                for(int e = d+stride;  e < count-(2*stride);  e+=stride)
                { 
                    for(int f = e+stride;  f < count-(1*stride);  f+=stride)
                    {    
                        for(int g = f+stride;  g < count-(0*stride);  g+=stride)
                        {  
                            // DO TEH STUFF
                        }
                    }
                }
            }
        }
    }
}