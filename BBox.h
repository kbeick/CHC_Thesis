#ifndef BBox_h
#define BBox_h

#include <limits>
#include <cmath>
#include "utils.h"

//adapted from Brandon Pelfrey
//https://raw2.github.com/brandonpelfrey/Fast-BVH/


struct BBox {
	Vec3f min, max, extent, center;
	int count;
	BBox() 
	{
		
	}
	BBox(const Vec3f& min, const Vec3f& max): min(min), max(max)
	{ 
		extent = max - min; 
	 	center = (max+min)*.5;
	}
	BBox(const Vec3f& p): min(p), max(p)
	{ 
		extent = max - min; 
	 	center = (max+min)*.5;
	}

	void expandToInclude(const Vec3f& p)
	{
		min.x = min.x > p.x ? p.x : min.x;
		min.y = min.y > p.y ? p.y : min.y;
		min.z = min.z > p.z ? p.z : min.z;
		max.x = max.x < p.x ? p.x : max.x;
		max.y = max.y < p.y ? p.y : max.y;
		max.z = max.z < p.z ? p.z : max.z;
		extent = max - min;
	 	center = (max+min)*.5;
	}

	void clear()
	{
		min.x= 1000000;
		min.y= 1000000;
		min.z= 1000000;
		max.x=-1000000;
		max.y=-1000000;
		max.z=-1000000; 
		extent.x=0;
		extent.y=0;
		extent.z=0;
		center.x=0;
		center.y=0;
		center.z=0;
		count=0;
	}

	void expandToInclude(const BBox& b)
	{
	 	expandToInclude(b.min);
	 	expandToInclude(b.max);
	 	extent = max - min;
	 	center = (max+min)*.5;
	 	count++;
	}
    int maxDimension() const 
    {
		int result = 0;
		//if(extent.y > extent.x) result = 1;
		//if(extent.z > extent.y) result = 2;
		return result;
	}
 	float surfaceArea() const 
 	{
 		Vec3f extent = max - min; 
		return 2.f*( extent.x*extent.z + extent.x*extent.y + extent.y*extent.z );
	}


	bool intersect(const Ray &ray, float *tnear, float *tfar) const 
	{

	  float tmin, tmax, tymin, tymax, tzmin, tzmax;
	  Vec3f parameters[]={min,max};

	  tmin =  ((parameters[  ray.sign[0]].x - ray.source.x) * ray.invDir.x);
	  tmax =  ((parameters[1-ray.sign[0]].x - ray.source.x) * ray.invDir.x);
	  
	  tymin = ((parameters[  ray.sign[1]].y - ray.source.y) * ray.invDir.y);
	  tymax = ((parameters[1-ray.sign[1]].y - ray.source.y) * ray.invDir.y);
	  
	  if ( (tmin > tymax) || (tymin > tmax) ) 
	    return false;
	  if (tymin > tmin)
	    tmin = tymin;
	  if (tymax < tmax)
	    tmax = tymax;
	  tzmin = ((parameters[  ray.sign[2]].z - ray.source.z) * ray.invDir.z);
	  tzmax = ((parameters[1-ray.sign[2]].z - ray.source.z) * ray.invDir.z);
	  
	  if ( (tmin > tzmax) || (tzmin > tmax) ) 
	    return false;
	  if (tzmin > tmin)
	    tmin = tzmin;
	  if (tzmax < tmax)
	    tmax = tzmax;
	  
	  *tnear=tmin;
	  *tfar =tmax;
	  return ( (tmin < std::numeric_limits<float>::max()) && (tmax > std::numeric_limits<float>::min()) );
	}

	bool intersect(const Vec3f rayDir,const Vec3f rayOrigin, const Vec3f inv_direction,
		const int * sign , float *tnear, float *tfar) const 
	{
	  float tmin, tmax, txmin, txmax, tymin, tymax, tzmin, tzmax;
	  Vec3f parameters[]={min,max};

	  // cerr << "rayOrigin " << rayOrigin << endl;
	  // cerr << "inv_direction " << inv_direction << endl;  

	  tmin = txmin = ((parameters[  sign[0]].x - rayOrigin.x) * inv_direction.x);
	  tmax = txmax = ((parameters[1-sign[0]].x - rayOrigin.x) * inv_direction.x);
	  tymin =		 ((parameters[  sign[1]].y - rayOrigin.y) * inv_direction.y);
	  tymax = 		 ((parameters[1-sign[1]].y - rayOrigin.y) * inv_direction.y);
	  
	  if ( (tmin > tymax) || (tymin > tmax) ) {
	  	
	    return false;
	  }
	  if (tymin > tmin)
	    tmin = tymin;
	  if (tymax < tmax)
	    tmax = tymax;
	  
	  tzmin = ((parameters[  sign[2]].z - rayOrigin.z) * inv_direction.z);
	  tzmax = ((parameters[1-sign[2]].z - rayOrigin.z) * inv_direction.z);
	  if ( (tmin > tzmax) || (tzmin > tmax) ) {
	  	
	    return false;
	  }
	  if (tzmin > tmin)
	    tmin = tzmin;
	  if (tzmax < tmax)
	    tmax = tzmax;

	  *tnear = tmin;
	  *tfar = tmax;

	  // INTERSECTION WITHIN SOME DISTANCE PARAMS
	  return ( (tmin < std::numeric_limits<float>::max()) && (tmax > std::numeric_limits<float>::min()) );
	}

	bool intersect2(const Vec3f rayDir,const Vec3f rayOrigin, const Vec3f inv_direction, const int * sign , float *tnear, float *tfar) const 
	{

	  float tmin, tmax, tymin, tymax, tzmin, tzmax;
	  Vec3f parameters[]={min,max};
	  bool hit=true;
	  //Vec3f inv_direction( 1.0/rayDir.x, 1.0/rayDir.y, 1.0/rayDir.z );

	  tmin =  ((parameters[  sign[0]].x - rayOrigin.x) * inv_direction.x);
	  tmax =  ((parameters[1-sign[0]].x - rayOrigin.x) * inv_direction.x);
	  tymin = ((parameters[  sign[1]].y - rayOrigin.y) * inv_direction.y);
	  tymax = ((parameters[1-sign[1]].y - rayOrigin.y) * inv_direction.y);
	  if ( (tmin > tymax) || (tymin > tmax) ) 
	    hit= false;
	  if (tymin > tmin)
	    tmin = tymin;
	  if (tymax < tmax)
	    tmax = tymax;
	  tzmin = ((parameters[  sign[2]].z - rayOrigin.z) * inv_direction.z);
	  tzmax = ((parameters[1-sign[2]].z - rayOrigin.z) * inv_direction.z);
	  if ( (tmin > tzmax) || (tzmin > tmax) ) 
	    hit= false;
	  if (tzmin > tmin)
	    tmin = tzmin;
	  if (tzmax < tmax)
	    tmax = tzmax;
	  *tnear=tmin;
	  *tfar =tmax;
	  return hit;
	}
};

ostream& operator<<(ostream& out, const BBox& x ) 
{
	out << "minX is  " << x.min.x << endl;
	out << "maxX is  " << x.max.x << endl;
	out << "minY is  " << x.min.y << endl;
	out << "maxY is  " << x.max.y << endl;
	out << "minZ is  " << x.min.z << endl;
	out << "maxZ is  " << x.max.z << endl;
	
	return out;
}


#endif