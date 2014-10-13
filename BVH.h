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
	out << "triangle_count " << x.triangle_count << endl;
	out << x.bbox << endl;
	return out;
}


void printBVH(BVH_Node* node)
{
	if(node == NULL){return;}

	cout << endl << *node;
	for (int i = 0; i < MAX_BRANCHING_FACTOR; ++i)
	{
		printBVH(node->children[i]);
	}
}

float* bvhToFlatArray(BVH_Node *root, int *size, int branching_factor){
	assert (branching_factor==2 || branching_factor==4 || branching_factor==8);

    // printBVH(root);

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

	*size = elements_per_inner_node*inner_node_counter + elements_per_leaf_node*leafCount;

	// cerr << "size IS " << *size << endl;

	float *flat_array= new float[*size];
	int non_leaf_count=0;
	int leaf_count=0;
	int currentIndex=0;
	// cerr<<"Begining while "<<size<<endl;

	// GET ALL NODES FROM TREE
	while(!tree.empty()){
		bool isLeaf=true;
		current=tree.top();
		tree.pop();
		// cerr<<"\nBegin Node: "<< current->id <<" <- " << currentIndex << ".  " << current <<endl;
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
			if (current->children[i]!=NULL) { 
				// cerr << "PUSHING " << *current->children[i] << endl;
				tree.push(current->children[i]);
			}
		}
		// cerr << "\nPRINTING\n" << endl;
	 //    printBVH(root);
		// cerr << "DONE PRINTING\n\n" << endl;
	}
	return flat_array;
}


#endif
