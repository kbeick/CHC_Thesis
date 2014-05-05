#ifndef Triangle_h
#define Triangle_h

#include <iostream>
#include "ray.h"
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
		unsigned int mortonCode;
		BBox bbox;
		

		Vec3f v1() const
		{
			return Vec3f(X[0], Y[0], Z[0]);
		}
		Vec3f v2() const
		{
			return Vec3f(X[1], Y[1], Z[1]);
		}
		Vec3f v3() const
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
		}

		bool intersect(const ray &r, float* isectData) const
		{
		    Vec3f edge1 = v2() - v1();
		    Vec3f edge2 = v3() - v1();
		    Vec3f pvec = crossProduct(r.unitDir.x, r.unitDir.y, r.unitDir.z, edge2.x, edge2.y, edge2.x);
		    float det = dotProduct(edge1, pvec);
		    if (det == 0) return false;
		    // ^Alternatively, could check if det is less than some small EPSILON
		    float invDet = 1.0 / det;
		    Vec3f tvec = r.source - v1();
		    float u = dotProduct(tvec, pvec) * invDet;
		    if (u < 0.0 || u > 1.0) return false;
		    Vec3f qvec = crossProduct(tvec, edge1);
		    float v = dotProduct(r.unitDir, qvec) * invDet;
		    if (v < 0.0 || u + v > 1.0) return false;
		    float t = dotProduct(edge2, qvec) * invDet;

		    if(t > 0.0){
		    	*isectData = t;
		    	return true;
		    }
		    return false;
		}

		int id;
};

void CreateTriangleArray(Triangle *triangles, int numTriangles, float *verts)
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