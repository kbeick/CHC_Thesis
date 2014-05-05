#ifndef BVH_H
#define BVH_H

#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <assert.h>
#include "Triangle.h"

#define MAX_BRANCHING_FACTOR 8
#define LEAF_SIZE 4				// Number of Triangles in a leaf
#define NON_LEAF_SIZE 16
#define LEAF_FLAG 0xFF800000
#define TRAVERSAL_COST .125f	//this doesn't seem to have a effect

static int inner_node_counter = 0;
int level=0;

static int flatArrayCount=1;
static int leafCount=0;
// int branching_factor;

using namespace std;

class BVH_Node{
	
	public:

		int id;

		int triangle_count;

		BBox bbox;

		BVH_Node *parent;
		BVH_Node *children[MAX_BRANCHING_FACTOR];

		Triangle* triangles;

		int parent_id;

		int axis_id;	// axis_id conventions will be: 
						// x = 0 
						// y = 1 
						// z = 2
};

ostream& operator<<(ostream& out, const BVH_Node& x ) 
{
	out << "id is " << x.id << endl;
	out << "minX is  " << x.bbox.min.x << endl;
	out << "maxX is  " << x.bbox.max.x << endl;
	out << "minY is  " << x.bbox.min.y << endl;
	out << "maxY is  " << x.bbox.max.y << endl;
	out << "minZ is  " << x.bbox.min.z << endl;
	out << "maxZ is  " << x.bbox.max.z << endl;
	
	return out;
}

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
		int stride =count/numDivs;
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
	// branching_factor = _branching_factor;
	// Set initial (dummy) values
	// level++;
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

	// cerr<<"LeafCheck"<<endl;
	//At a leaf node, add triangles to the node and return
	if (count <= LEAF_SIZE){
		// cerr<<"At LEAF with count " <<count <<endl;
		current->triangles = triangles;
		current->triangle_count=count;
		for(int i=0; i<MAX_BRANCHING_FACTOR; i++) current->children[i] = NULL;
		flatArrayCount+=current->triangle_count+(NON_LEAF_SIZE-2);
		leafCount++;
		current->id=-1;
		return;
	}
    current->id=inner_node_counter;
	inner_node_counter++;
	flatArrayCount+=NON_LEAF_SIZE;
	// Not at a leaf node, sort along an axis and recurse CalculateBestAxis(triangles, count,current->bbox)
	// cerr<<"Sorting"<<endl;
	int splitPoint;
	int bestAxis;

	bestAxis=CalculateBestAxis(triangles, count,current->bbox,splitPoint);

	switch(bestAxis)
	{	// Sort on X-axis
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

void BuildBVH_bottomup(Triangle* triangles, BVH_Node *current, BVH_Node *parent,
							int count, int depth)
{
	// branching_factor = _branching_factor;
}

// void collapseBVH(BVH_Node *current, int current_branching_factor, int new_branching_factor)
// {
// 	assert(2*current_branching_factor == new_branching_factor);

// 	for(int i=0; i<current_branching_factor; i++){
// 		current->children[i]->children[0];
// 	}


// }


float* bvhToFlatArray(BVH_Node *root, int &size, int branching_factor){
	assert (branching_factor==2 || branching_factor==4 || branching_factor==8);
	// INSTANTIATE LOCALS
	root->id=0;
	stack<BVH_Node*> tree;
	queue<BVH_Node*> leaf_nodes;
	tree.push(root);
	BVH_Node *current;
	// cerr<<"\nInners "<<inner_node_counter<<endl;

	// branching_factor*6 (min,max x,y,z of each child) + branching_factor(refer to each child) 
	int elements_per_inner_node = branching_factor*7;
	int elements_per_leaf_node = 8;

	size = elements_per_inner_node*inner_node_counter + elements_per_leaf_node*leafCount;

	float *flat_array= new float[size];
	int non_leaf_count=0;
	int leaf_count=0;
	int currentIndex=0;
	// cerr<<"Begining while "<<size<<endl;

	// GET ALL NODES FROM TREE
	while(!tree.empty()){
		bool isLeaf=true;
		current=tree.top();
		tree.pop();
		// cerr<<"\nBegin Node: "<<current->id<<" "<<currentIndex<<" "<<current<<endl;
		current->id=currentIndex;

		// if(current->parent==NULL) cerr<<"Must be root because I have no parent"<<endl;
		// else cerr<<"At node "<<current->id<<" My Parent is "<<current->parent->id<<endl;

		// Is a leaf iff all children are NULL
		for(int i=0; i<branching_factor; i++){
			// cerr << "child: " << current->children[i] << endl;
			// cerr << "NULL? " << (current->children[i] == NULL) << endl;
			isLeaf = (current->children[i] == NULL) && isLeaf;
			if( isLeaf == false) break;
		}
	
		// cerr<<"I have this many triangles : "<<current->triangle_count<<endl;	
		if(!isLeaf){
			// cerr << "NOT LEAF" << endl;
			currentIndex--; //Cuz 1st time thru it's already where it should be, so this counteracts first increment
			//#pragma unroll
			for(int i=0; i<branching_factor; i++){
				flat_array[++currentIndex] = current->children[i]->bbox.min.x;
				//cerr<<currentIndex<<endl;
				flat_array[++currentIndex] = current->children[i]->bbox.min.y;
				//cerr<<currentIndex<<endl;
				flat_array[++currentIndex] = current->children[i]->bbox.min.z;
				//cerr<<currentIndex<<endl;
				flat_array[++currentIndex] = current->children[i]->bbox.max.x;
				//cerr<<currentIndex<<endl;
				flat_array[++currentIndex] = current->children[i]->bbox.max.y;
				//cerr<<currentIndex<<endl;
				flat_array[++currentIndex] = current->children[i]->bbox.max.z;
				//cerr<<currentIndex<<endl;
			}
			for(int i=0; i<branching_factor; i++){
				flat_array[++currentIndex] = -1; // to be set to the ids of children
			}

		}
		else if(isLeaf){
			// cerr << "LEAF" << endl;
			flat_array[currentIndex] = LEAF_FLAG;
			// cerr<<"Beginning Leaf index "<<currentIndex<<endl;
			flat_array[++currentIndex] = current->triangle_count;
			// cerr<<currentIndex<< " gets # triangles: " << flat_array[currentIndex] <<endl;//list<Triangle>::iterator it=current->triangles.begin(); it != current->triangles.end(); ++it
			for(int i=0;i<current->triangle_count;i++)
			{
				flat_array[++currentIndex] = current->triangles[i].id;
				// cerr<<currentIndex<< " gets " << flat_array[currentIndex] <<endl;
			}
			//int padding=14 - current->triangles.size();// make each node fit into 16 btyes
			int padding=6 - current->triangle_count;
			// cerr<<"Padding "<<padding<<" "<<current->triangles->size()<<endl;

			for(int i=0;i<padding;i++){
				flat_array[++currentIndex] =-1;
				// cerr<<currentIndex<< " gets -1" <<endl;
			}
		}
		
		//tell your parent where you are
		if(current->id!=0) //root has no parents 
		{
			for(int i=0; i<branching_factor; i++){
				if(current->parent->children[i]==current){
					flat_array[current->parent->id+6*branching_factor+i]=current->id;
				}
			}
			// else cerr<<"Node "<<current->id<<" is an oprhan"<<endl;

			// if(current->parent->prev==current){
			// 	//cerr<<"I am the left child updating my location"<<endl;
			// 	flat_array[current->parent->id+12]=current->id;
			// }
			// else if(current->parent->next==current){
			// 	//cerr<<"I am the right child updating my location"<<endl;
			// 	flat_array[current->parent->id+13]=current->id;
			// }
			// else cerr<<"Node "<<current->id<<" is an oprhan"<<endl;
		}
		++currentIndex;
		for(int i=0; i<branching_factor; i++){
			if (current->children[i]!=NULL) tree.push(current->children[i]);
		}
		// if (current->prev!=NULL) tree.push(current->prev);
		// if (current->next!=NULL) tree.push(current->next);
	}
	return flat_array;
}


#endif
