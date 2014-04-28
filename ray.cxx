//
//  ray.cpp
//  
//
//  Created by Kevin Beick on 6/3/13.
//
//


#include <iostream>
#include <cmath>

#include "ray.h"

using std::cerr;
using std::cout;
using std::endl;

ray::ray( vec3f start, vec3f unitDir )
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