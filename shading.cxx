#include <cmath>
#include "utils.h"

struct LightingParameters
{
	public:
    LightingParameters(void)
    {
        double x = .5;
        double y = .5;
        double z = .5;
		lightDir.x = x / sqrt(x*x+y*y+z*z);
		lightDir.y = y / sqrt(x*x+y*y+z*z);
		lightDir.z = -z / sqrt(x*x+y*y+z*z);
		Ka = 0.8;
		Kd = 0.7;
		Ks = 1.2;
		gamma = .7;
    };
	
	
    Vec3f lightDir; // The direction of the light source
    double Ka;           // The coefficient for ambient lighting.
    double Kd;           // The coefficient for diffuse lighting.
    double Ks;           // The coefficient for specular lighting.
    double gamma;        // The exponent term for specular lighting.
};
