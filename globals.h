#ifndef GLOBALS
#define GLOBALS

#include <string>

#include "Camera.h"
#include "Triangle.h"
#include "BBox.h"

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
        Ks = 4.2;
        gamma = 7.7;
    };
    
    
    Vec3f lightDir; // The direction of the light source
    float Ka;           // The coefficient for ambient lighting.
    float Kd;           // The coefficient for diffuse lighting.
    float Ks;           // The coefficient for specular lighting.
    float gamma;        // The exponent term for specular lighting.
};


// RUN TIME CONSTANTS
std::string USAGE_MSG = "USAGE: ./CHC_Thesis.app <model filename> <children per node (int)> <construction method (td or bu)> <camera pos (3 whitespace separated floats)> <depthOfTrace (int)> <opacity (float)>\n";

static bool PRODUCE_IMAGE = true;
static int IMAGE_WIDTH =  300;
static int IMAGE_HEIGHT = 300;
static double GLOBAL_REFLECT_COEF = 0.4;    /* Global Reflectivity */

static int branching_factor;                // Children per Node
static int construction_method = 0;         // TOPDOWN==1 ; BOTTOMUP==2
static double opacity = 0.0;                // Global Opacity
static int depthOfTrace = 0;                // Depth of Ray Tracing, number of reflections accounted for

/* COMMAND LINE-DEFINED PARAMETERS FOR BVH */
ObjReader* objReader;                       // Object Reader for file
Vec3f campos;                               // Camera Position

/* GLOBAL VARIABLES */
LightingParameters* lp = new LightingParameters();
Camera *c;
Triangle* tris;
static int numTriangles;
static float* flat_array;

/* AUXILIARY HELPER */
static BBox *EMPTY_NODE_BBOX;

#endif