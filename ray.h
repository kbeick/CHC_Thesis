
#ifndef __RAY__
#define __RAY__

#include <iostream>
#include <cmath>

#include "utils.h"




class Ray
{
public:
    
    Vec3f source;
    Vec3f unitDir;
    Vec3f invDir;
    int sign[3];
    
    
    Ray(Vec3f start, Vec3f dir);
    ~Ray(){}

    void normalize();
    
};

Ray::Ray( Vec3f start, Vec3f dir )
{ 

    // cerr << "in Ray constructor, start is " << start << " while dir is " << dir << endl;
    
    this->source.x = start.x;
    this->source.y = start.y;
    this->source.z = start.z;


    double norm = sqrt( pow(dir.x, 2.) + pow(dir.y, 2.) + pow(dir.z, 2.) );
    // cerr << "norm is " << norm << endl;
    this->unitDir.x = dir.x/norm;
    this->unitDir.y = dir.y/norm;
    this->unitDir.z = dir.z/norm;

    this->invDir.x = 1/dir.x;
    this->invDir.y = 1/dir.y;
    this->invDir.z = 1/dir.z;
    
    this->sign[0] = (invDir.x < 0);
    this->sign[1] = (invDir.y < 0);
    this->sign[2] = (invDir.z < 0);
}

Ray*
RayFromPoints(Vec3f start, Vec3f head)
{
    /* Find Dir */
    Vec3f dir = Vec3f(head.x-start.x, head.y-start.y, head.z-start.z);
    /* Call other Constructor */
    return new Ray(start, dir);
}

void Ray::normalize()
{
    double norm = sqrt( pow(unitDir.x, 2.) + pow(unitDir.y, 2.) + pow(unitDir.z, 2.) );
    
    unitDir.x = unitDir.x/norm;
    unitDir.y = unitDir.y/norm;
    unitDir.z = unitDir.z/norm;
    
}

ostream& operator<<(ostream& out, const Ray& x ) 
{
    out << "source is " << x.source << endl;
    out << "unitDir is  " << x.unitDir << endl;
    out << "invDir is  " << x.invDir << endl;
    out << "sign is  " << x.sign[0] << ", " << x.sign[1] << ", " << x.sign[2] << endl;
    // out << "maxY is  " << x.bbox.max.y << endl;
    // out << "minZ is  " << x.bbox.min.z << endl;
    // out << "maxZ is  " << x.bbox.max.z << endl;
    
    return out;
}


// Ray
// operator+(const Ray ray, const Vec3f &r) const
// {
//     Vec3f newHead = Vec3f(ray->source.x+ray->unitDir.x, ray->source.y+ray->unitDir.y, ray->source.z+ray->unitDir.z);

//     Ray result = ray;
//     Ray result = Ray(ray->source, )


//     return Vec3f(x-r.x, y-r.y, z-r.z);
// }


#endif