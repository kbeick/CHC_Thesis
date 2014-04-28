
#ifndef __RAY__
#define __RAY__

#include "utils.h"




class ray
{
public:
    
    vec3f source;
    vec3f unitDir;
    
    
    ray(vec3f start, vec3f head);

    void normalize();
    
    ~ray(){}
    
};



#endif