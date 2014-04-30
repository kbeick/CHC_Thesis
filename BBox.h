#ifndef BBox_h
#define BBox_h

//adapted from Brandon Pelfrey
//https://raw2.github.com/brandonpelfrey/Fast-BVH/


struct BBox {
	eavlVector3 min, max, extent;
	int count;
	EAVL_HOSTDEVICE BBox() 
	{
		
	}
	EAVL_HOSTDEVICE BBox(const eavlVector3& min, const eavlVector3& max): min(min), max(max)
	{ 
		extent = max - min; 
	}
	EAVL_HOSTDEVICE BBox(const eavlVector3& p): min(p), max(p)
	{ 
		extent = max - min; 
	}

	EAVL_HOSTDEVICE void expandToInclude(const eavlVector3& p)
	{
		min.x = min.x > p.x ? p.x : min.x;
		min.y = min.y > p.y ? p.y : min.y;
		min.z = min.z > p.z ? p.z : min.z;
		max.x = max.x < p.x ? p.x : max.x;
		max.y = max.y < p.y ? p.y : max.y;
		max.z = max.z < p.z ? p.z : max.z;
		extent = max - min;
	}

	EAVL_HOSTDEVICE void clear()
	{
		min.x=1000000;
		min.y=1000000;
		min.z=1000000;
		max.x=-1000000;
		max.y=-1000000;
		max.z=-1000000; 
		extent.x=0;
		extent.y=0;
		extent.z=0;
		count=0;


	}

	EAVL_HOSTDEVICE void expandToInclude(const BBox& b)
	{
	 	expandToInclude(b.min);
	 	expandToInclude(b.max);
	 	extent = max - min;
	 	count++;
	}
    EAVL_HOSTDEVICE int maxDimension() const 
    {
		int result = 0;
		//if(extent.y > extent.x) result = 1;
		//if(extent.z > extent.y) result = 2;
		return result;
	}
 	EAVL_HOSTDEVICE float surfaceArea() const 
 	{
 		eavlVector3 extent = max - min; 
		return 2.f*( extent.x*extent.z + extent.x*extent.y + extent.y*extent.z );
	}



	EAVL_HOSTDEVICE bool intersect(const eavlVector3 rayDir,const eavlVector3 rayOrigin, const eavlVector3 inv_direction, const int * sign , float *tnear, float *tfar) const 
	{

	  float tmin, tmax, tymin, tymax, tzmin, tzmax;
	  eavlVector3 parameters[]={min,max};
	  //eavlVector3 inv_direction( 1.0/rayDir.x, 1.0/rayDir.y, 1.0/rayDir.z );
	  

	  tmin =  ((parameters[  sign[0]].x - rayOrigin.x) * inv_direction.x);
	  tmax =  ((parameters[1-sign[0]].x - rayOrigin.x) * inv_direction.x);
	  tymin = ((parameters[  sign[1]].y - rayOrigin.y) * inv_direction.y);
	  tymax = ((parameters[1-sign[1]].y - rayOrigin.y) * inv_direction.y);
	  if ( (tmin > tymax) || (tymin > tmax) ) 
	    return false;
	  if (tymin > tmin)
	    tmin = tymin;
	  if (tymax < tmax)
	    tmax = tymax;
	  tzmin = ((parameters[  sign[2]].z - rayOrigin.z) * inv_direction.z);
	  tzmax = ((parameters[1-sign[2]].z - rayOrigin.z) * inv_direction.z);
	  if ( (tmin > tzmax) || (tzmin > tmax) ) 
	    return false;
	  if (tzmin > tmin)
	    tmin = tzmin;
	  if (tzmax < tmax)
	    tmax = tzmax;
	  *tnear=tmin;
	  *tfar =tmax;
	  return true;
	}

	EAVL_HOSTDEVICE bool intersect2(const eavlVector3 rayDir,const eavlVector3 rayOrigin, const eavlVector3 inv_direction, const int * sign , float *tnear, float *tfar) const 
	{

	  float tmin, tmax, tymin, tymax, tzmin, tzmax;
	  eavlVector3 parameters[]={min,max};
	  bool hit=true;
	  //eavlVector3 inv_direction( 1.0/rayDir.x, 1.0/rayDir.y, 1.0/rayDir.z );
	  

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




#endif