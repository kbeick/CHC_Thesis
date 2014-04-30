
int CalculateBestAxis(Triangle* triangles, int count, BBox total, int &splitPoints[])
{
    int return_axis;
    float min_cost = 100000000.f; // MAX_INT instead?
    
    for(int axis = 0; axis < 3; axis++){

        switch(axis){
            // Sort on X-axis
            case 0:{
                //cerr<<"Sort Begin"<<endl;
                std::sort(triangles, triangles+count, compX);
                //cerr<<"Sort end"<<endl;
                break;
            }
            // Sort on Y-axis   
            case 1:{
                std::sort(triangles, triangles+count, compY);
                break;
            }
            // Sort on Z-axis
            case 2:{
                std::sort(triangles, triangles+count, compZ);
            }
        }

        std::vector<BBox> BBoxes(children_per_node);
        int i;
        for(i=0; i<children_per_node; i++){
            BBoxes[i].clear();
        }

        float areas[children_per_node] = {};
        float total_area=total.surfaceArea();
        float costs[children_per_node];
        float total_cost = 0;

        int numDivs=100;
        int stride =count/numDivs;
        while(stride==0)
        {
            numDivs/=2;
            stride=count/numDivs;
        }
        for(i=stride;i<count;i+=stride)
        {   
            //cerr<<"Splitpoint "<<i <<endl;
            //advance(it, i);
            int a;
            for(a=0; a<children_per_node; a++){
                BBoxes[a].clear();

                // TODO, adjust/abstract this stuff
                // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                BBoxes[0]=triangles[0].bbox;
                for(int j=1; j<i;j++)
                {
                    BBoxes[0].expandToInclude(triangles[j].bbox);
                }

                BBoxes[1]=triangles[i].bbox;
                for(int j=i+1; j<count; j++)
                {
                    BBoxes[1].expandToInclude(triangles[j].bbox);
                }
                // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


                costs[a] = (BBoxes[a].surfaceArea()/total_area)*BBoxes[a].count;
                total_cost += costs[a];

            }

            if((total_cost+TRAVERSAL_COST) < min_cost)
            {   
                min_cost=TRAVERSAL_COST+total_cost;
                return_axis = axis;
                //TODO, how to hangle multiple split points?
                // splitPoints[]
                splitPoint=i;
                // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            }
        }
    }
    return return_axis;
}


void BuildBVH_topdown(Triangle* triangles, BVH_Node *current, BVH_Node *parent,
                            int children_per_node, int count, int depth)
{
    level++;

    current->parent = parent;
    current->triangle_count = count;

    //find the total bbox for the array
    current->bbox=triangles[0].bbox;
    for(int i=1; i<count;i++)
    {   
        current->bbox.expandToInclude(triangles[i].bbox);
    }

    //cerr<<"LeafCheck"<<endl;
    //At a leaf node, add triangles to the node and return
    if (count <= LEAF_SIZE){
        //cerr<<"At leaf"<<endl;
        current->triangles = triangles;
        current->triangle_count=count;
        for (int i=0; i<children_per_node; i++) current->children[i] = NULL;
        flatArrayCount+=current->triangle_count+(NON_LEAF_SIZE-2);
        childCount++;
        current->id=-1;
        return;
    }
    // ELSE, not a leaf node
    current->id=inner_node_counter;
    inner_node_counter++;
    flatArrayCount+=NON_LEAF_SIZE;
    // Not at a leaf node, sort along an axis and recurse CalculateBestAxis(triangles, count,current->bbox)
    //cerr<<"Sorting"<<endl;axis_id
    int splitPoints[children_per_node-1];
    int bestAxis;

    bestAxis=CalculateBestAxis(triangles, count, current->bbox, splitPoints);

    switch(bestAxis)
    {
        // Sort on X-axis
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
    //cerr<<"Done Sorting"<<endl;
    // inner node, so we have to create children for it
    for(i=0; i<children_per_node; i++){
        BVH_Node *node = new BVH_Node();
        current->children[i] = node;
    }

    int rightCount=count-splitPoint;
    // Find halfway point for when we split the triangle list
    for (i=0; i<sizeof(splitPoints); i++){
        BuildBVH_topdown(triangles+splitPoints[i], current->children[i], current, splitPoints[i], depth+1);
    }

    return;
}
