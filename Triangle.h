#ifndef Triangle_h
#define Triangle_h

#include <iostream>
#include "Ray.h"
#include "utils.h"
#include "BBox.h"

using namespace std;

class Triangle{

	public:

		float X[3];
		float Y[3];
		float Z[3];
		float c[3];
		double xNorms[3];
		double yNorms[3];
		double zNorms[3];
		Ray *normal;
		unsigned int mortonCode;
		BBox bbox;
		

		Vec3f v0() const
		{
			return Vec3f(X[0], Y[0], Z[0]);
		}
		Vec3f v1() const
		{
			return Vec3f(X[1], Y[1], Z[1]);
		}
		Vec3f v2() const
		{
			return Vec3f(X[2], Y[2], Z[2]);
		}


		void calcCentroid()
		{
			c[0]=(X[0]+X[1]+X[2])/3.0f;
			c[1]=(Y[0]+Y[1]+Y[2])/3.0f;
			c[2]=(Z[0]+Z[1]+Z[2])/3.0f;
		}

		float getXMin()
		{
			float temp=X[0];
			if(X[1]<temp) temp=X[1];
			if(X[2]<temp) temp=X[2];
			return temp;
		}

		float getZMin()
		{
			float temp=Z[0];
			if(Z[1]<temp) temp=Z[1];
			if(Z[2]<temp) temp=Z[2];
			return temp;
		}

		float getYMin()
		{
			float temp=Y[0];
			if(Y[1]<temp) temp=Y[1];
			if(Y[2]<temp) temp=Y[2];
			return temp;
		}

		float getXMax()
		{
			float temp=X[0];
			if(X[1]>temp) temp=X[1];
			if(X[2]>temp) temp=X[2];
			return temp;
		}
		float getZMax()
		{
			float temp=Z[0];
			if(Z[1]>temp) temp=Z[1];
			if(Z[2]>temp) temp=Z[2];
			return temp;
		}
		float getYMax()
		{
			float temp=Y[0];
			if(Y[1]>temp) temp=Y[1];
			if(Y[2]>temp) temp=Y[2];
			return temp;
		}
		void setBbox()
		{
			bbox.min=Vec3f(getXMin(),getYMin(),getZMin());
			bbox.max=Vec3f(getXMax(),getYMax(),getZMax());
			bbox.extent=bbox.max-bbox.min;
	 		bbox.center = (bbox.max+bbox.min)*.5;
		}

		bool intersect(const Ray &r, float* isectData) const
		{
			// cerr << "here in intersect. Ray is " << r;
			// cerr << "this triangle is " << endl;
			// print();

		    Vec3f edge1 = v1() - v0();
		    Vec3f edge2 = v2() - v0();
		    // cerr << "edge1 " << edge1 << "   edge2 " << edge2 << endl;
		    Vec3f* pvec = crossProduct(r.unitDir.x, r.unitDir.y, r.unitDir.z, edge2.x, edge2.y, edge2.z);
		    // cerr << "pvec " << *pvec << endl;
			float det = dotProduct(edge1, *pvec);

			/* NON-CULLING */
			// cerr << "now here .... det = " << det << endl;

		    /* if determinant is near zero ray lies in plane of triangle */
		    if (det < 0.000001 && det > -0.000001) return false;
		    // ^Alternatively, could check if det is less than some small EPSILON
		    float invDet = 1.0 / det;
		    Vec3f tvec = r.source - v0(); /* Distance from v0 to ray origin */
		    float u = dotProduct(tvec, *pvec) * invDet;

			// cerr << "and here .... u =" << u << endl;

		    if (u < 0.0 || u > 1.0) return false;

			// cerr << "and here" << endl;

			Vec3f* qvec = crossProduct(tvec, edge1);
		    float v = dotProduct(r.unitDir, *qvec) * invDet;
		    if (v < 0.0 || u + v > 1.0) return false;
		    float t = dotProduct(edge2, *qvec) * invDet;

		    // cerr << "Triangle intersect got t = " << t << endl;

		    if(t > 0.000001){
		    	*isectData = t;
		    	return true;
		    }
		    return false;
		}

		void print() const
		{
			cerr << "ID " << id << endl;
			cerr << "X values are  " << this->X[0] << " " << this->X[1] << " " << this->X[2] << endl;
			cerr << "Y values are  " << this->Y[0] << " " << this->Y[1] << " " << this->Y[2] << endl;
			cerr << "Z values are  " << this->Z[0] << " " << this->Z[1] << " " << this->Z[2] << endl;
		}

		int id;
};

void GetTriangleFromID(Triangle *triangles, int numTriangles, int ID, Triangle *result){
	for(int i=0; i<numTriangles; i++){
		if(triangles[i].id == ID){ *result = triangles[i]; }
	}
}

void CreateTriangleArray(Triangle *triangles, int numTriangles, float *verts, float *norms)
{	
	//triangles= new Triangle[numTriangles];
	for(int triIndex = 0; triIndex < numTriangles; triIndex++){
		//Triangle t;	
		triangles[triIndex].X[0] = verts[triIndex*9];
	    triangles[triIndex].Y[0] = verts[triIndex*9+1];
	    triangles[triIndex].Z[0] = verts[triIndex*9+2];
	    
	    triangles[triIndex].X[1] = verts[triIndex*9+3];
	    triangles[triIndex].Y[1] = verts[triIndex*9+4];
	    triangles[triIndex].Z[1] = verts[triIndex*9+5];
	    
	    triangles[triIndex].X[2] = verts[triIndex*9+6];
	    triangles[triIndex].Y[2] = verts[triIndex*9+7];
	    triangles[triIndex].Z[2] = verts[triIndex*9+8];
		
		triangles[triIndex].id=triIndex;
		triangles[triIndex].calcCentroid();
		triangles[triIndex].setBbox();

		/* add normals */
		triangles[triIndex].xNorms[0] = norms[triIndex*9];
	    triangles[triIndex].yNorms[0] = norms[triIndex*9+1];
	    triangles[triIndex].zNorms[0] = norms[triIndex*9+2];
	    
	    triangles[triIndex].xNorms[1] = norms[triIndex*9+3];
	    triangles[triIndex].yNorms[1] = norms[triIndex*9+4];
	    triangles[triIndex].zNorms[1] = norms[triIndex*9+5];
	    
	    triangles[triIndex].xNorms[2] = norms[triIndex*9+6];
	    triangles[triIndex].yNorms[2] = norms[triIndex*9+7];
	    triangles[triIndex].zNorms[2] = norms[triIndex*9+8];

	    // Ray v1Norm = Ray(Vec3f(triangles[triIndex].c[0], triangles[triIndex].c[1], triangles[triIndex].c[2]),
	    // 				 Vec3f(triangles[triIndex].xNorms[0], triangles[triIndex].yNorms[0], triangles[triIndex].zNorms[0]));

	    // Norm direction is being determined by only vertex
	    triangles[triIndex].normal = new Ray(Vec3f(triangles[triIndex].c[0], triangles[triIndex].c[1], triangles[triIndex].c[2]),
	    									 Vec3f(triangles[triIndex].xNorms[0], triangles[triIndex].yNorms[0], triangles[triIndex].zNorms[0]));

	}
}


// void
// CreateTriangleList(list<Triangle> *triangles, int numTriangles, float *xVerts, float *yVerts, float *zVerts)
// {
// 	for(int triIndex = 0; triIndex < numTriangles; triIndex++){
// 		Triangle t;
		
// 	    t.X[0] = xVerts[triIndex*9];
// 	    t.Y[0] = xVerts[triIndex*9+1];
// 	    t.Z[0] = xVerts[triIndex*9+2];
	    
// 	    t.X[1] = xVerts[triIndex*9+3];
// 	    t.Y[1] = xVerts[triIndex*9+4];
// 	    t.Z[1] = xVerts[triIndex*9+5];
	    
// 	    t.X[2] = xVerts[triIndex*9+6];
// 	    t.Y[2] = xVerts[triIndex*9+7];
// 	    t.Z[2] = xVerts[triIndex*9+8];
// 	    t.calcCentroid();
// 	    t.setBbox();
		
// 		t.id=triIndex;
		
// 		triangles->push_back(t);
// 	}
// }


bool compCenterX(const Triangle & a, const Triangle & b){
	return a.c[0] < b.c[0];
}

bool compCenterY(const Triangle & a, const Triangle & b){
	return a.c[1] < b.c[1];
}
bool compCenterZ(const Triangle & a, const Triangle & b){
	return a.c[2] < b.c[2];
}

bool compX(const Triangle & a, const Triangle & b){
	float a_minX = a.X[0];
	float b_minX = b.X[0];

	for(int i = 1; i < 3; i++){
		if(a_minX > a.X[i]){
			a_minX = a.X[i];
		}
		if(b_minX > b.X[i]){
			b_minX = b.X[i];
		}
	}
	return a_minX < b_minX;
}

bool compY(const Triangle & a, const Triangle & b){
	float a_minY = a.Y[0];
	float b_minY = b.Y[0];

	for(int i = 1; i < 3; i++){
		if(a_minY > a.Y[i]){
			a_minY = a.Y[i];
		}
		if(b_minY > b.Y[i]){
			b_minY = b.Y[i];
		}
	}
	return a_minY < b_minY;
}

bool compZ(const Triangle & a, const Triangle & b){
	float a_minZ = a.Z[0];
	float b_minZ = b.Z[0];

	for(int i = 1; i < 3; i++){
		if(a_minZ > a.Z[i]){
			a_minZ = a.Z[i];
		}
		if(b_minZ > b.Z[i]){
			b_minZ = b.Z[i];
		}
	}
	return a_minZ < b_minZ;
}



ostream& operator<<(ostream& out, const Triangle& x ) 
{
	out << "X values are  " << x.X[0] << " " << x.X[1] << " " << x.X[2] << endl;
	out << "Y values are  " << x.Y[0] << " " << x.Y[1] << " " << x.Y[2] << endl;
	out << "Z values are  " << x.Z[0] << " " << x.Z[1] << " " << x.Z[2] << endl;
	return out;
}

#endif