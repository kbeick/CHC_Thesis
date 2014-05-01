
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
    
    
    ray(Vec3f start, Vec3f head);

    void normalize();
    
    ~ray(){}
    
};

ray::ray( Vec3f start, Vec3f unitDir )
{ 
    
    this->source.x = start.x;
    this->source.y = start.y;
    this->source.z = start.z;
    this->unitDir.x = unitDir.x;
    this->unitDir.y = unitDir.y;
    this->unitDir.z = unitDir.z;
    
    //this->normalize();
}

void ray::normalize()
{
    double norm = sqrt( pow(unitDir.x, 2.) + pow(unitDir.y, 2.) + pow(unitDir.z, 2.) );
    
    unitDir.x = unitDir.x/norm;
    unitDir.y = unitDir.y/norm;
    unitDir.z = unitDir.z/norm;
    
}


#endif