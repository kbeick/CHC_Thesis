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

 //    Matrix*	ViewTransform()
	// {
	// 	Matrix* mat = new Matrix();
	// 	mat->A[0][0] = 1 / tan(angle/2);
	// 	mat->A[1][1] = 1 / tan(angle/2);
	// 	mat->A[2][2] = (far+near)/(far-near);
	// 	mat->A[3][2] = 2*far*near/(far-near);
	// 	mat->A[2][3] = -1;
		
	// 	return mat;
	// }
	
	// //
 //    Matrix*	CameraTransform()
	// {
	// 	double t[3] = { 0 - position[0], 0 - position[1], 0 - position[2] };
		
	// 	double v3[3] = {position[0] - focus[0], position[1] - focus[1], position[2] - focus[2]};
	// 	double* v1 = crossProd(up, v3);
	// 	double* v2 = crossProd(v3, v1);
	// 	// Normalize
	// 	double sqSum1,sqSum2,sqSum3 = 0;
	// 	for (int i=0; i<3; i++){
	// 		sqSum1 += (pow(v1[i],2));
	// 		sqSum2 += (pow(v2[i],2));
	// 		sqSum3 += (pow(v3[i],2));
	// 	}
	// 	double normalizer1 = sqrt(sqSum1);
	// 	double normalizer2 = sqrt(sqSum2);
	// 	double normalizer3 = sqrt(sqSum3);
	// 	for (int i = 0; i<3; i++){
	// 		v1[i] = v1[i]/normalizer1;
	// 		v2[i] = v2[i]/normalizer2;
	// 		v3[i] = v3[i]/normalizer3;
	// 	}
		
	// 	//cout << "v1: " << v1[0] << " " << v1[1] << " " << v1[2] << endl;
	// 	//cout << "v2: " << v2[0] << " " << v2[1] << " " << v2[2] << endl;
	// 	//cout << "v3: " << v3[0] << " " << v3[1] << " " << v3[2] << endl;
	// 	//cout << "Cam pos: " << position[0] << " " << position[1] << " " << position[2] << endl;
		
	// 	Matrix* mat = new Matrix();
		
	// 	for (int i = 0; i<3; i++) {
	// 		for (int j = 0; j<3; j++) {
	// 			if (i==0)
	// 				mat->A[j][i] = v1[j];
	// 			else if (i==1)
	// 				mat->A[j][i] = v2[j];
	// 			else
	// 				mat->A[j][i] = v3[j];
	// 		}
	// 		if (i==0)
	// 			mat->A[3][i] = v1[0]*t[0] + v1[1]*t[1] + v1[2]*t[2];
	// 		else if (i==1)
	// 			mat->A[3][i] = v2[0]*t[0] + v2[1]*t[1] + v2[2]*t[2];
	// 		else
	// 			mat->A[3][i] = v3[0]*t[0] + v3[1]*t[1] + v3[2]*t[2];
	// 	}
	// 	// Set last column to 0,0,0,1
	// 	for (int i = 0; i <3; i++)
	// 		mat->A[i][3] = 0;
	// 	mat->A[3][3] = 1;
		
	// 	return mat;
	// }
	
	
 //    Matrix*	DeviceTransform(int width, int height)
	// {
	// 	Matrix* mat = new Matrix();
		
	// 	mat->A[0][0] = double(width)/2;
	// 	mat->A[3][0] = double(width)/2;
	// 	mat->A[1][1] = double(height)/2;
	// 	mat->A[3][1] = double(height)/2;
	// 	mat->A[2][2] = 1.;
	// 	mat->A[3][3] = 1.;
		
	// 	return mat;
	// }
	
	double* crossProd(double a[], double b[])
	{
		double* result = new double[3];
		result[0] = a[1]*b[2] - a[2]*b[1];
		result[1] = b[0]*a[2] - a[0]*b[2];
		result[2] = a[0]*b[1] - a[1]*b[0];
		
		return result;
	}
};



// double SineParameterize(int curFrame, int nFrames, int ramp)
// {
//     int nNonRamp = nFrames-2*ramp;
//     double height = 1./(nNonRamp + 4*ramp/M_PI);
//     if (curFrame < ramp)
//     {
//         double factor = 2*height*ramp/M_PI;
//         double eval = cos(M_PI/2*((double)curFrame)/ramp);
//         return (1.-eval)*factor;
//     }
//     else if (curFrame > nFrames-ramp)
//     {
//         int amount_left = nFrames-curFrame;
//         double factor = 2*height*ramp/M_PI;
//         double eval =cos(M_PI/2*((double)amount_left/ramp));
//         return 1. - (1-eval)*factor;
//     }
//     double amount_in_quad = ((double)curFrame-ramp);
//     double quad_part = amount_in_quad*height;
//     double curve_part = height*(2*ramp)/M_PI;
//     return quad_part+curve_part;
// }

// Camera GetCamera(int frame, int nframes)
// {
//     double t = SineParameterize(frame, nframes, nframes/10);
//     Camera c;
//     c.near = 5;
//     c.far = 200;
//     c.angle = M_PI/6;
//     c.position[0] = 40*sin(2*M_PI*t);
//     c.position[1] = 40*cos(2*M_PI*t);
//     c.position[2] = 40;
//     c.focus[0] = 0;
//     c.focus[1] = 0;
//     c.focus[2] = 0;
//     c.up[0] = 0;
//     c.up[1] = 1;
//     c.up[2] = 0;
//     return c;
// }



#endif
