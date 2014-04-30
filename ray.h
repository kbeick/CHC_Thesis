
#ifndef __RAY__
#define __RAY__

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



#endif