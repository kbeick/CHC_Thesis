#ifndef GLOBALS
#define GLOBALS

#include "Camera.h"

#define TOPDOWN  (1)
#define BOTTOMUP (2)


struct LightingParameters
{
    public:
    LightingParameters(void)
    {
        float x = 1;
        float y = 1;
        float z = 1;
        lightDir.x = x / sqrt(x*x+y*y+z*z);
        lightDir.y = y / sqrt(x*x+y*y+z*z);
        lightDir.z = -z / sqrt(x*x+y*y+z*z);
        Ka = 0.3;
        Kd = 0.7;
        Ks = 5.2;
        gamma = 5.7;
    };
    
    
    Vec3f lightDir; // The direction of the light source
    float Ka;           // The coefficient for ambient lighting.
    float Kd;           // The coefficient for diffuse lighting.
    float Ks;           // The coefficient for specular lighting.
    float gamma;        // The exponent term for specular lighting.
};


// RUN TIME CONSTANTS
string USAGE_MSG = "USAGE: ./CHC_Thesis.app <model filename> <children per node (int)> <construction method (td or bu)> <camera pos (3 whitespace separated floats)> <numReflections (int)> <opacity (float)>\n";

bool PRODUCE_IMAGE = true;
int IMAGE_WIDTH =  300;
int IMAGE_HEIGHT = 300;
double GLOBAL_REFLECT_COEF = 0.2;        /* Global Reflectivity */

int branching_factor = 0;           // Children per Node
int construction_method = 0;        // TOPDOWN==1 ; BOTTOMUP==2
double opacity = 0.0;               // Global Opacity

LightingParameters* lp = new LightingParameters();
Camera *c;
Triangle* tris;
int numTriangles;
float* flat_array;

#endif