
#ifndef __RAY__
#define __RAY__

#include <iostream>
#include <cmath>

#include "utils.h"




class ray
{
public:
    
    Vec3f source;
    Vec3f unitDir;
    Vec3f invDir;
    int sign[3];
    
    
    ray(Vec3f start, Vec3f head);
    ~ray(){}

    void normalize();
    
};

ray::ray( Vec3f start, Vec3f unitDir )
{ 
    
    this->source.x = start.x;
    this->source.y = start.y;
    this->source.z = start.z;

    double norm = sqrt( pow(unitDir.x, 2.) + pow(unitDir.y, 2.) + pow(unitDir.z, 2.) );
    this->unitDir.x = unitDir.x/norm;
    this->unitDir.y = unitDir.y/norm;
    this->unitDir.z = unitDir.z/norm;

    invDir = Vec3f(1/this->unitDir.x, 1/this->unitDir.y, 1/this->unitDir.z);
    norm = sqrt( pow(invDir.x, 2.) + pow(invDir.y, 2.) + pow(invDir.z, 2.) );
    this->invDir.x = invDir.x/norm;
    this->invDir.y = invDir.y/norm;
    this->invDir.z = invDir.z/norm;
    
    this->sign[0] = (invDir.x < 0);
    this->sign[1] = (invDir.y < 0);
    this->sign[2] = (invDir.z < 0);
}

void ray::normalize()
{
    double norm = sqrt( pow(unitDir.x, 2.) + pow(unitDir.y, 2.) + pow(unitDir.z, 2.) );
    
    unitDir.x = unitDir.x/norm;
    unitDir.y = unitDir.y/norm;
    unitDir.z = unitDir.z/norm;
    
}

ostream& operator<<(ostream& out, const ray& x ) 
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


#endif