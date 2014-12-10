#ifndef __CAM__
#define __CAM__

#include <math.h>
#include <cmath>
#include "utils.h"

class Camera
{
  public:
    double          near, far;
    double          angle;
    Vec3f*          position;
    Vec3f*          focus;
    Vec3f*          up;

	
	double* crossProd(double a[], double b[])
	{
		double* result = new double[3];
		result[0] = a[1]*b[2] - a[2]*b[1];
		result[1] = b[0]*a[2] - a[0]*b[2];
		result[2] = a[0]*b[1] - a[1]*b[0];
		
		return result;
	}
};

#endif
