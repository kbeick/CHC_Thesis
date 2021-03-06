/*  
Original source code borrowed from Matt Larsen; Univerisity of Oregon, 2014
Adapted for use by Kevin Beick in his thesis project "Analyzing Performance of Bounding Volume Hierarchies for Ray Tracing" 
*/

#ifndef BVH_H
#define BVH_H

#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <assert.h>
#include "Triangle.h"

#define MAX_BRANCHING_FACTOR 8
#define LEAF_SIZE 4				// Max Number of Triangles in a leaf
#define NON_LEAF_SIZE 16
#define LEAF_FLAG 0xFF800000
#define TRAVERSAL_COST .125f

static int inner_node_counter = 0;
int level=0;

static int flatArrayCount=1;
static int leafCount=0;
static int emptyNodeCount = 0;

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
	if(x.parent!=NULL){out << "parent_id is " << x.parent->id << endl;}
	out << "triangle_count " << x.triangle_count << endl;
	out << x.bbox << endl;
	return out;
}


void printBVH(BVH_Node* node, int level)
{
	if(node == NULL){return;}

	cout << "level ";
	for(int l=0; l<level; l++){ cout << "(*)"; }
	cout << endl << *node << endl;

	level++;

	for (int i = 0; i < MAX_BRANCHING_FACTOR; ++i)
	{
		printBVH(node->children[i], level);
	}
}


void printBVH_depth(BVH_Node* node, int level)
{
	if(node->children[0] == NULL){
		for(int i=0; i<level; i++){cout << "   ";}
		cout << node->triangle_count << endl;
		return;
	}

	for (int i = branching_factor-1; i >= branching_factor/2; --i)
	{
		printBVH_depth(node->children[i], level+1);
	}
	
	for(int i=0; i<level; i++){cout << "   ";}
	cout << node->triangle_count << endl;

	for (int i = branching_factor/2-1; i >= 0; --i)
	{
		printBVH_depth(node->children[i], level+1);
	}
}



float* bvhToFlatArray(BVH_Node *root, int *size, int branching_factor){
	assert (branching_factor==2 || branching_factor==4 || branching_factor==8);

	// INSTANTIATE LOCALS
	root->id=0;
	stack<BVH_Node*> tree;
	queue<BVH_Node*> leaf_nodes;
	tree.push(root);
	BVH_Node *current;

	// branching_factor*6 (min,max x,y,z of each child) + branching_factor(refer to each child) 
	int elements_per_inner_node = branching_factor*7;
	int elements_per_leaf_node = 8;

	*size = elements_per_inner_node*inner_node_counter + elements_per_leaf_node*leafCount;

	float *flat_array= new float[*size];
	int non_leaf_count=0;
	int leaf_count=0;
	int currentIndex=0;

	// GET ALL NODES FROM TREE
	while(!tree.empty()){
		bool isLeaf=true;
		current=tree.top();
		tree.pop();
		current->id=currentIndex;

		// Is a leaf iff all children are NULL
		for(int i=0; i<branching_factor; i++){
			isLeaf = (current->children[i] == NULL) && isLeaf;
			if( isLeaf == false) break;
		}
		
		if(!isLeaf){
			// cerr << "NOT LEAF" << endl;
			currentIndex--; //Cuz 1st time thru it's already where it should be, so this proactively counteracts first increment
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
				flat_array[++currentIndex] = -1; // to be set to the ids of children later
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
			int padding = 6 - current->triangle_count;
			// cerr<<"Padding "<<padding<<" "<<current->triangles->size()<<endl;

			for(int i=0;i<padding;i++){
				flat_array[++currentIndex] =-1;
				// cerr<<currentIndex<< " gets -1" <<endl;
			}
		}
		
		/* tell your parent where you are */
		if(current->id!=0) //root has no parents 
		{
			for(int i=0; i<branching_factor; i++){
				if(current->parent->children[i]==current){
					flat_array[current->parent->id+6*branching_factor+i]=current->id;
				}
			}
		}

		++currentIndex;
		for(int i=0; i<branching_factor; i++){
			if (current->children[i]!=NULL) { 
				tree.push(current->children[i]);
			}
		}
	}
	return flat_array;
}


#endif
