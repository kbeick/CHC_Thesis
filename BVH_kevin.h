#include <list>
#include <vector>
#include <stack>
#include <queue>
#include "Triangle.h"

#define LEAF_SIZE 4				// Number of Triangles in a leaf
#define NON_LEAF_SIZE 16
#define LEAF_FLAG 0xFF800000
#define TRAVERSAL_COST .125f	//this doesn't seem to have a effect

static int inner_node_counter = 0;
int level=0;

static int flatArrayCount=1;
static int childCount=0;
int children_per_node;

using namespace std;

class BVH_Node{
	
	public:

		int id;

		int triangle_count;

		BBox bbox;

		BVH_Node *parent;
		std::vector<BVH_Node*> children;

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
	// BuildBVH_topdown(triangles, prev, current, splitPoint, depth+1);
	// BuildBVH_topdown((triangles+splitPoint), next, current, rightCount, depth+1);

	return;
}

void BuildBVH_bottomup(Triangle* triangles, BVH_Node *current, BVH_Node *parent,
							int children_per_node, int count, int depth)
{}

float* bvhToFlatArray(BVH_Node *root, int &size, int children_per_node){
	// INSTANTIATE LOCALS
	root->id=0;
	stack<BVH_Node*> tree;
	queue<BVH_Node*> leaf_nodes;
	tree.push(root);
	BVH_Node *current;
	//cerr<<"Inners "<<inner_node_counter<<endl;

	int elements_per_inner_node = 0;
	if (children_per_node == 2)			elements_per_inner_node =  16; // 2 for padding
	else if (children_per_node == 8)	elements_per_inner_node =  56; // 0 for padding
	else if (children_per_node == 32)	elements_per_inner_node = 224; // 0 for padding
	else cerr<<"\nERROR~~~~~~~~~~ 2,8,or 32 children per node allowed only\n"; return NULL;

	size = elements_per_inner_node * inner_node_counter + 8*childCount;

	float *flat_array= new float[size];
	int non_leaf_count=0;
	int leaf_count=0;
	int currentIndex=0;
	bool isLeaf=false;
	//cerr<<"Begining while "<<size<<endl;

	// GET ALL NODES FROM TREE
	while(!tree.empty()){
		current=tree.top();
		tree.pop();
		//cerr<<"Begin Node: "<<current->id<<" "<<currentIndex<<" "<<current<<endl;
		current->id=currentIndex;

		//if(current->parent==NULL) cerr<<"Must be root because I have no parent"<<endl;
		//else cerr<<"At node "<<current->id<<" My Parent is "<<current->parent->id<<endl;
		bool tmp=true;
		for(int i=0; i<children_per_node; i++){
			tmp = (current->children[i] != NULL) && tmp;
			if( tmp == false) break;
		}
		isLeaf = not(tmp);

		//if(!isLeaf) cerr<<"inner node "<<non_leaf_count<<endl;
		//if(isLeaf) {leaf_count++;cerr<<"leaf node "<<leaf_count<<endl;}
	
		//cerr<<"I have h this many triangles : "<<current->triangle_count<<endl;	
		if(!isLeaf){
			currentIndex--; //Cuz 1st time thru it's already where it should be, so this counteracts first increment
			for(int i=0; i<children_per_node; i++){
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
			for(i=0; i<children_per_node; i++){
				flat_array[++currentIndex] = -1; // to be set to the ids of children
			}
			if(children_per_node==2) { // Other cases don't need padding
				flat_array[++currentIndex] = -1;//padding
				flat_array[++currentIndex] = -1;//padding
			}

		}
		else if(isLeaf){
			flat_array[currentIndex] = LEAF_FLAG;
			//cerr<<"Beginning Leaf index "<<currentIndex<<endl;
			flat_array[++currentIndex] = current->triangle_count;
			//cerr<<currentIndex<<endl;//list<Triangle>::iterator it=current->triangles.begin(); it != current->triangles.end(); ++it
			for(int i=0;i<current->triangle_count;i++)
			{
				flat_array[++currentIndex] = current->triangles[i].id;
				//cerr<<currentIndex<<endl;
			}
			//int padding=14 - current->triangles.size();// make each node fit into 16 btyes
			int padding=6 - current->triangle_count;
			//cerr<<"Padding "<<padding<<" "<<current->triangles.size()<<endl;

			for(int i=0;i<padding;i++) flat_array[++currentIndex] =-1;
				//cerr<<currentIndex<<endl;
		}
		
		//tell your parent where you are
		if(current->id!=0) //root has no parents 
		{
			for(int i=0; i<children_per_node; i++){
				if(current->parent->children[i]==current){
					flat_array[current->parent->id+6*children_per_node+i]=current->id;
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
		for(int i=0; i<children_per_node; i++){
			if (current->children[i]!=NULL) tree.push(current->children[i]);
		}
		// if (current->prev!=NULL) tree.push(current->prev);
		// if (current->next!=NULL) tree.push(current->next);
	}
	return flat_array;
}


