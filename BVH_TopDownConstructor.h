

int CalculateBestAxis(Triangle* triangles, int count, BBox total, int &splitPoint)
{

    int return_axis;
    float min_cost = 100000000.f; // MAX_INT instead?
    

    for(int axis = 0; axis < 3; axis++){

        switch(axis){
            // Sort on X-axis
            case 0:{
                //cerr<<"Sort Begin"<<endl;
                std::sort(triangles,triangles+ count, compX);
                //cerr<<"Sort end"<<endl;
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

        BBox left;
        BBox right;
        left.clear();
        right.clear();
        //total.clear();

        //cerr<<"bbox Begin"<<endl;
        float left_area=0;
        float right_area=0;
        float total_area=total.surfaceArea();
        float left_cost;
        float right_cost;
        int numDivs=100;
        int stride = count/numDivs;
        while(stride==0)
        {
            numDivs/=2;
            stride=count/numDivs;
        }
        //cerr<<"Count : "<<count<<" looking at "<<axis<<endl;
        for(int i=stride;i<count;i+=stride)
        {   
            //cerr<<"Splitpoint "<<i <<endl;
            //advance(it, i);
            left.clear();
            right.clear();
            left=triangles[0].bbox;
            for(int j=1; j<i;j++)
            {
                left.expandToInclude(triangles[j].bbox);
            }
            right=triangles[i].bbox;
            for(int j=i+1; j<count;j++)
            {
                right.expandToInclude(triangles[j].bbox);
            }
            left_cost = (left.surfaceArea()/total_area)*left.count;
            right_cost = (right.surfaceArea()/total_area)*right.count;
            if((left_cost + right_cost+TRAVERSAL_COST) < min_cost)
            {   
                min_cost=TRAVERSAL_COST+left_cost + right_cost;
                return_axis = axis;
                splitPoint=i;
                //cerr<<"Min cost : "<<min_cost<<" split "<<splitPoint<<endl;
            }
        }
    }
    //cerr<<" Count  "<<count<<"Min Cost  : "<<min_cost<< " On axis "<< return_axis<<" SplitPoint "<<splitPoint<< endl;
    return return_axis;
}


void BuildBVH_topdown(Triangle* triangles, BVH_Node *current, BVH_Node *parent,
                            int count, int depth)
{
    // cerr << "\ndepth " << depth << endl;

    current->parent = parent;
    current->triangle_count = count;

    //find the total bbox for the array
    current->bbox=triangles[0].bbox;
    for(int i=1; i<count; i++)
    {   
        current->bbox.expandToInclude(triangles[i].bbox);
    }
    // cerr << "found the total bbox for the array" << endl;

    /* AT A LEAF, MAKE A LEAF NODE AND RETURN */
    if (count <= LEAF_SIZE){
        // cerr<<"At LEAF with num triangles: " << count << endl;
        current->triangles = triangles;
        current->triangle_count=count;
        for(int i=0; i<MAX_BRANCHING_FACTOR; i++){ current->children[i] = NULL; }
        flatArrayCount += current->triangle_count+(NON_LEAF_SIZE-2);
        leafCount++;
        current->id=-1;
        return;
    }

    /* NOT A LEAF */   
    // cerr<<"not a leaf"<<endl;
    current->id = inner_node_counter;
    inner_node_counter++;
    flatArrayCount += NON_LEAF_SIZE;
    // Not at a leaf node, sort along an axis and recurse CalculateBestAxis(triangles, count,current->bbox)

    int splitPoint;
    int bestAxis;

    bestAxis=CalculateBestAxis(triangles, count,current->bbox,splitPoint);

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
    // cerr<<"Done Sorting"<<endl;
    // inner node, so we have to create 2 children for it
    BVH_Node *prev = new BVH_Node();
    BVH_Node *next = new BVH_Node();

    // Each node has room for up to ~MAX_BRANCHING_FACTOR~ children, but only has two for now.
    current->children[0] = prev;
    current->children[1] = next;
    for(int i=2; i<MAX_BRANCHING_FACTOR; i++) current->children[i] = NULL;

    int rightCount=count-splitPoint;

    BuildBVH_topdown(triangles, prev, current, splitPoint, depth+1);
    BuildBVH_topdown((triangles+splitPoint), next, current, rightCount, depth+1);
    return;
}
