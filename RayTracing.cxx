//
//  RayTracing.cxx
//  
//
//  Created by Kevin
//
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <vtkDataSet.h>
#include <vtkImageData.h>
#include <vtkPNGWriter.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPoints.h>
#include <vtkUnsignedCharArray.h>
#include <vtkFloatArray.h>
#include <vtkCellArray.h>

#include <cmath>
#include <limits>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include "BVH_kevin.h"
#include "BBox.h"
#include "ray.h"
#include "shading.h"
#include "Triangle.h"
#include "utils.h"
#include "objLoader.h"

#define TOPDOWN  (1)
#define BOTTOMUP (2)


using std::cerr;
using std::cout;
using std::endl;

string USAGE_MSG = "USAGE: <executable> <model filename> <children per node (int)> <construction method (td or bu)> <camera pos (3 whitespace separated floats)> <reflection coef (float)> <opacity (float)>\n";


bool PRODUCE_IMAGE = true;
int IMAGE_WIDTH = 300;
int IMAGE_HEIGHT = 300;

LightingParameters* lp = new LightingParameters();


Triangle* tris;
int numTriangles;

// PARAMETERS FOR BVH
ObjReader* objReader;        // Object Reader for file
int branching_factor = 0;          // Children per Node
int construction_method = 0;        // TOPDOWN==1 ; BOTTOMUP==2
Vec3f campos;                   // Camera Position
double reflec = 0.0;                // Global Reflectivity
double opacity = 0.0;               // Global Opacity

// Declare our collection of Triangles
// std::vector<Triangle> Triangles(numShapes);
void
print_params()
{
    cerr << "~~~~read in params:" << endl;
    cerr << "~~~~constr method is: " << construction_method << endl;
    cerr << "~~~~got camera pos: " << campos[0] << ", " << campos[1] << ", " << campos[2] << endl;
}

void
SetConstructionMethod(char* input)
{
    if(strcmp(input, "td")==0) construction_method = TOPDOWN;
    else if(strcmp(input, "bu")==0) construction_method = BOTTOMUP;
    else{
        cerr << input;
        throw std::invalid_argument( "construction method must be either td or bu (top down or bottom up)\n" );
    }
}

// ---------------------------------------------
// ------------------ CLASSES ------------------
// ---------------------------------------------

class Screen
{
public:
    unsigned char *buffer;
    double *depthBuffer;
    int width, height;
    
    // return the element number of buffer corresponding to pixel at (c,r)
    int pixel(int c, int r){
        return 3*( (IMAGE_WIDTH*r)+c );
    }
};


// -----------------------------------------------------------
// ---------------- IMAGE FUNCTIONS --------------------------
// -----------------------------------------------------------

vtkImageData *
NewImage(int width, int height)
{
    vtkImageData *image = vtkImageData::New();
    image->SetDimensions(width, height, 1);
    image->SetWholeExtent(0, width-1, 0, height-1, 0, 0);
    image->SetUpdateExtent(0, width-1, 0, height-1, 0, 0);
    image->SetNumberOfScalarComponents(3);
    image->SetScalarType(VTK_UNSIGNED_CHAR);
    image->AllocateScalars();
    
    return image;
}

void
WriteImage(vtkImageData *img, const char *filename)
{
    std::string full_filename = filename;
    full_filename += ".png";
    vtkPNGWriter *writer = vtkPNGWriter::New();
    writer->SetInput(img);
    writer->SetFileName(full_filename.c_str());
    writer->Write();
    writer->Delete();
}

// ------------------------------------------------------------
// ------------------------------------------------------------

// ray* findRefraction(ray* normal, double refracAmount)
// {
//     Vec3f v = Vec3f( normal->source.x-campos[0], normal->source.y-campos[1], normal->source.z-campos[2] );
    
//     double VdotN = dotProduct(v, normal->unitDir);
//     Vec3f refracDir = Vec3f( v.x/refracAmount + (VdotN/refracAmount - sqrt(1 - (1/refracAmount)*(1/refracAmount)*(1-VdotN*VdotN)))*normal->unitDir.x,
//                             v.y/refracAmount + (VdotN/refracAmount - sqrt(1 - (1/refracAmount)*(1/refracAmount)*(1-VdotN*VdotN)))*normal->unitDir.y,
//                             v.z/refracAmount + (VdotN/refracAmount - sqrt(1 - (1/refracAmount)*(1/refracAmount)*(1-VdotN*VdotN)))*normal->unitDir.z );
    
//     return new ray( normal->source, refracDir );
// }


// double* getRefractedImage(ray* refraction, int currShape)
// {
//     double nearest_t = 1e9;
//     double* color = new double[3];
    
//     // See if any objects obscure the light
//     for (int i = 0; i< numTriangles; i++) {
//         if (i == currShape) {continue;}
        
//         double* Sol = quadraticSolver(1.0 ,
//                                       2*(refraction->unitDir[0]*( refraction->source[0] - tris[i].center[0] ) +
//                                          refraction->unitDir[1]*( refraction->source[1] - tris[i].center[1] ) +
//                                          refraction->unitDir[2]*( refraction->source[2] - tris[i].center[2] )) ,
//                                       (refraction->source[0] - tris[i].center[0])*(refraction->source[0] - tris[i].center[0]) +
//                                       (refraction->source[1] - tris[i].center[1])*(refraction->source[1] - tris[i].center[1]) +
//                                       (refraction->source[2] - tris[i].center[2])*(refraction->source[2] - tris[i].center[2]) -
//                                       tris[i].radius*tris[i].radius );
//         if (Sol[0] < 0.0) {
//             continue;
//         }else{
//             if (Sol[1] < nearest_t && Sol[1] > 0.0) {
//                 nearest_t = Sol[1];
//                 color[0] = tris[i].color[0];
//                 color[1] = tris[i].color[1];
//                 color[2] = tris[i].color[2];
//             }
//         }
//     }
//     return color;
    
// }


// ray* findReflection(ray* sphNormal)
// {
//     double v[3] = { sphNormal->source[0]-campos[0], sphNormal->source[1]-campos[1], sphNormal->source[2]-campos[2] };
    
//     double VdotN = dotProduct(v, sphNormal->unitDir);
//     double reflDir[3] = { v[0]-2*VdotN*sphNormal->unitDir[0], v[1]-2*VdotN*sphNormal->unitDir[1], v[2]-2*VdotN*sphNormal->unitDir[2] };
    
//     return new ray( sphNormal->source, reflDir );
// }


// double* Illumination(ray* reflected, int currShape)
// {
//     double nearest_t = 1e9;
//     double* color = new double[3];
    
//     // See if any objects obscure the light
//     for (int i = 0; i< numTriangles; i++) {
//         if (i == currShape) {continue;}
        
//         double* Sol = quadraticSolver(1.0 ,
//                                       2*(reflected->unitDir[0]*( reflected->source[0] - tris[i].center[0] ) +
//                                          reflected->unitDir[1]*( reflected->source[1] - tris[i].center[1] ) +
//                                          reflected->unitDir[2]*( reflected->source[2] - tris[i].center[2] )) ,
//                                       (reflected->source[0] - tris[i].center[0])*(reflected->source[0] - tris[i].center[0]) +
//                                       (reflected->source[1] - tris[i].center[1])*(reflected->source[1] - tris[i].center[1]) +
//                                       (reflected->source[2] - tris[i].center[2])*(reflected->source[2] - tris[i].center[2]) -
//                                       tris[i].radius*tris[i].radius );
//         if (Sol[0] < 0.0) {
//             continue;
//         }else{
//             if (Sol[1] < nearest_t && Sol[1] > 0.0) {
//                 nearest_t = Sol[1];
//                 color[0] = tris[i].color[0];
//                 color[1] = tris[i].color[1];
//                 color[2] = tris[i].color[2];
//             }
//         }
//     }
//     return color;
    
// }


// /*
//  *
//  */
// bool checkShadow(ray* refl, int currShape)
// {
//     // See if any objects obscure the light
//     for (int i = 0; i< tris.size(); i++) {
//         if (i == currShape) {continue;}
        
//         double* Sol = quadraticSolver(1.0 ,
//                                       2*(refl->unitDir[0]*( refl->source[0] - tris[i].center[0] ) +
//                                          refl->unitDir[1]*( refl->source[1] - tris[i].center[1] ) +
//                                          refl->unitDir[2]*( refl->source[2] - tris[i].center[2] )) ,
//                                       (refl->source[0] - tris[i].center[0])*(refl->source[0] - tris[i].center[0]) +
//                                       (refl->source[1] - tris[i].center[1])*(refl->source[1] - tris[i].center[1]) +
//                                       (refl->source[2] - tris[i].center[2])*(refl->source[2] - tris[i].center[2]) -
//                                       tris[i].radius*tris[i].radius );
//         if (Sol[0] < 0.0) {
//             // then nothing in the way
//             continue;
//         }else{
//             return true;
//         }
//     }
//     return false;
// }


// /*
//  * Returns the Phong shaing value
//  */
// double CalculateShading(LightingParameters* lp, double* viewDirection, ray* normalRay, int currShape)
// {
//     double* normal = normalRay->unitDir;
//     double* point = normalRay->source;
    
//     // Calc dot product of Incoming light and normal
//     double LdotN = dotProduct(lp->lightDir, normal);
    
//     // Calc the diffuse lighting contribution: diffuse = abs(LightDirec dot sufaceNormal)
//     double diffuse = abs( LdotN );
    
//     // Calc the strength of the directly refelcted light
//     double* refl = new double[3];
//     refl[0] = 2 * LdotN * normal[0]  - lp->lightDir[0];
//     refl[1] = 2 * LdotN * normal[1]  - lp->lightDir[1];
//     refl[2] = 2 * LdotN * normal[2]  - lp->lightDir[2];
    
//     // Calc the specular lighting contribution
//     double spec;
//     if (dotProd(refl, viewDirection) < 0.0)
//         spec = 0.0;
//     else
//         spec = pow(dotProduct(refl, viewDirection), lp->gamma);
    
//     //cerr << "normal: " << "(" << normal[0] << ", " << normal[1] << ", " << normal[2] << "),  and Reflection: " << refl[0] << ", " << refl[1] << ", " << refl[2] << ") " << "with Ambient: " << lp->Ka << " diffuse " << lp->Kd*diffuse << " spec: " << lp->Ks*spec << " Total Shading: " << lp->Ka + lp->Kd*diffuse + lp->Ks*spec << endl;
    
//     //  ----------- Find shadows --------------
//     // Direction to light source
//     double reflec[] = {-(lp->lightDir[0]*1500-point[0]), -(lp->lightDir[1]*1500-point[1]), -(lp->lightDir[2]*1500-point[2])};
//     // Reflection ray
//     ray* reflection = new ray(point, reflec);
//     reflection->normalize();
    
//     bool shadow = checkShadow(reflection, currShape);
    
//     return lp->Ka + lp->Kd*diffuse + lp->Ks*spec*(!shadow);
// }



// /*
//  *
//  */
// double* getPixelColor(const ray* Ray)
// {
//     double nearest_t = 1e9;
//     double* color = new double[3];

//     // For each object in scene, find the smallest, non-neg, real t:
//     for (int i = 0; i < objReader->totalTriangles; i++) {

//         //TODO
//         int intersectResult = intersect3D_RayTriangle( &ray, tris[i],  )

//         Triangle.intersect(ray, interstionData)


//         if (Sol[0] < 0.0) { continue; }
//         else{
//             // ------------------------- INTERSECTS OBJECT ----------------------
//             // CHECK IF THIS OBJECT IS CLOSER
//             if (Sol[1] < nearest_t && Sol[1] > 0.0) {
//                 nearest_t = Sol[1];
                
//                 // --- GET VIEW DIRECTION -----
//                 double camDir[] = {0.0-campos[0], 0.0-campos[1], 0.0-campos[2]};
//                 ray* viewDir = new ray(campos, camDir);
//                 viewDir->normalize();

//                 // --- GET NORMAL @ INTERSECT POINT
//                 ray* sphNormal = tris[i].normalAtPt(intpnt);
//                 sphNormal->normalize();
                
//                 // --- FIND SHADING -----------
//                 double PhShade = CalculateShading(lp, viewDir->unitDir, sphNormal, i);
        
//                 // --- FIND REFLECTIONS -------
//                 ray* reflected = findReflection(sphNormal);
//                 reflected->normalize();
//                 double* ill = Illumination(reflected, i);
                
//                 // --- FIND REFRACTIONS -------
//                 double* refrColor;
//                 if (tris[i].refrac != 0.0) {
//                     ray* refracted = findRefraction(sphNormal, tris[i].refrac);
//                     refracted->normalize();
//                     refrColor = getRefractedImage(refracted, i);
//                 }
                
//                 // --- APPLY SHADING ----------
//                 color[0] = (unsigned char) ceil441( std::min(255.0, tris[i].color[0]*PhShade));
//                 color[1] = (unsigned char) ceil441( std::min(255.0, tris[i].color[1]*PhShade));
//                 color[2] = (unsigned char) ceil441( std::min(255.0, tris[i].color[2]*PhShade));
                
//                 // --- APPLY OPACITY ----------
//                 color[0] = (unsigned char) ceil441( color[0]*(1-tris[i].opacity));
//                 color[1] = (unsigned char) ceil441( color[1]*(1-tris[i].opacity));
//                 color[2] = (unsigned char) ceil441( color[2]*(1-tris[i].opacity));
                
//                 // --- APPLY REFLECTIONS ------
//                 color[0] = (unsigned char) ceil441( std::min(255.0, color[0] + tris[i].reflec * ill[0]));
//                 color[1] = (unsigned char) ceil441( std::min(255.0, color[1] + tris[i].reflec * ill[1]));
//                 color[2] = (unsigned char) ceil441( std::min(255.0, color[2] + tris[i].reflec * ill[2]));
                
//                 // --- APPLY REFRACTIONS ------
//                 color[0] = (unsigned char) ceil441( std::min(255.0, color[0] + tris[i].opacity*refrColor[0]));
//                 color[1] = (unsigned char) ceil441( std::min(255.0, color[1] + tris[i].opacity*refrColor[1]));
//                 color[2] = (unsigned char) ceil441( std::min(255.0, color[2] + tris[i].opacity*refrColor[2]));
                
//             }else break;
//         }
//     }
    
//     return color;
// }


/*
 *
 */
Vec3f* getUnitDirToPixel(int x, int y)
{
    Vec3f* result = new Vec3f;
    
    float angle = 2*tan(M_PI/4);
    
    result->x = (2*x+1-IMAGE_WIDTH)/2 *angle/IMAGE_WIDTH; 
    result->y = (2*y+1-IMAGE_HEIGHT)/2 *angle/IMAGE_HEIGHT;  
    result->z = -1.5;
    
    return result;
}

bool traverseFlatArray(float* flat_array, int idx, ray* ray, Vec3f* color, int d)
{
    cerr << "\nBox at " << idx << endl;
    // LEAF NODE
    if (flat_array[idx++] == LEAF_FLAG){
        cerr << "LEAF NODE" << endl;
        int triangle_count = flat_array[idx++];
        // Intersect Triangles
        for(int i=0; i<triangle_count; i++){
            cerr << "triangle id = " << flat_array[idx+i] << endl;
            cerr << "~still need to intersect triangles" << endl;
            // Calc color specs for closest intersected Triangle

        }
        color->x = 255;
        color->y = 255;
        color->z = 255;
        return true;
    }
    // INNER NODE
    else{
        cerr << "INNER NODE" << endl;

        std::vector<float> mins (branching_factor, 0);// idx == which BBox, val == tnear of BBox (if intersect else 0)
        // float mins[branching_factor] = {};// idx == which BBox, val == tnear of BBox (if intersect else 0)

        // Intersect bounding boxes
        for(int i=0; i<branching_factor; i++){
            float min_x = flat_array[i+idx++];
            float min_y = flat_array[i+idx++];
            float min_z = flat_array[i+idx++];
            float max_x = flat_array[i+idx++];
            float max_y = flat_array[i+idx++];
            float max_z = flat_array[i+idx++];

            // Construct Bounding Box
            Vec3f min = Vec3f(min_x, min_y, min_z);
            Vec3f max = Vec3f(max_x, max_y, max_z);
            BBox* box = new BBox(min, max);

            // Intersect Bounding Box
            float tnear;
            cerr << "a " << tnear << endl;
            float tfar;
            // bool bb = box->intersect(*ray, &tnear, &tfar);
            bool bb = box->intersect(ray->unitDir, ray->source, ray->invDir, ray->sign, &tnear, &tfar);
            if ( bb ){
                cerr << "intersection! " << tnear << endl;
                mins[i] = tnear;
                cerr << "~~intsection, mins["<<i<<"] got " << mins[i] << endl;
            }
            delete box;
        }
        // Traverse closest intersected Bounding Box
        // bool still_searching = true;
        while(1){
            // cerr << "another iteration of while" << endl;
            int least_box = -1;
            for(int i=0; i<branching_factor; i++){
                if( mins[i] != 0){
                    if(least_box == -1 || mins[i] < mins[least_box] ){ least_box=i;
                        // cerr << "New least box "<<least_box << endl;
                    }
                }
            }
            if( least_box == -1){ return false; } //No BBox intersection

            cerr << "going to box at " << idx+least_box << endl;
            if (traverseFlatArray(flat_array, idx+least_box, ray, color, d+1)){ return true; }
            else{ mins[least_box] = 0; }
        }
        cerr << "shouldn't have gotten here...." << endl;
    }
}


int main(int argc, char** argv)
{
    cerr << "\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    // HANDLE AND SET BVH PARAMETERS
    if (argc != 9){ cerr << USAGE_MSG; }
    try{
        objReader = new ObjReader(argv[1]);
        branching_factor = atoi( argv[2] );
        SetConstructionMethod(argv[3]);
        campos[0] = atof(argv[4]);
        campos[1] = atof(argv[5]);
        campos[2] = atof(argv[6]);
        reflec = atof(argv[7]);
        opacity = atof(argv[8]);
    }catch (const std::exception &exc){
        // catch anything thrown within try block that derives from std::exception
        std::cerr << exc.what();
        cerr << USAGE_MSG; 
        exit(0);
    }

    print_params();

    vtkImageData *image;
    Screen* screen = new Screen;
    if (PRODUCE_IMAGE){
        image = NewImage(IMAGE_WIDTH, IMAGE_HEIGHT);
        unsigned char *buffer =
            (unsigned char *) image->GetScalarPointer(0,0,0);
        int npixels = IMAGE_WIDTH*IMAGE_HEIGHT;
    
        //Initialize buffer to be all black
        for (int i = 0 ; i < npixels*3 ; i++)
            buffer[i] = 0;
        
        screen->buffer = buffer;
        screen->width = IMAGE_WIDTH;
        screen->height = IMAGE_HEIGHT;
    }

    numTriangles = objReader->totalTriangles;
    cerr << "numTriangles is: " << numTriangles << endl;

    float* verts;
    float* normals;

    // GET RAW DATA FROM objReader
    objReader->getRawData(verts, normals);

    Triangle* tris = new Triangle[numTriangles];
    CreateTriangleArray(tris, numTriangles, verts);

    // BUILD BVH
    BVH_Node *root = new BVH_Node();
    root->id = 0;
    root->parent = NULL;
    // Call Specified BVH Constructor
    if (construction_method == TOPDOWN)       BuildBVH_topdown(tris, root, root->parent, numTriangles, 0);
    else if (construction_method == BOTTOMUP) BuildBVH_bottomup(tris, root, root->parent, numTriangles, 0);
    // TODO, implement BuildBVH_bottomup()!!
    
    int flat_array_len;
    float* flat_array = bvhToFlatArray(root, flat_array_len, branching_factor);

    for(int a=0; a<flat_array_len; a++){
        if(a==56 || a==64 || a==72 || a==80 || a==88) cerr << "idx " << a << " ";
        cerr << flat_array[a] << endl;
    }

    printf("Made it this far! NOW TO TRAVERSE!\n");

    // ------------------------------- DO RAY TRACING ------------------------------
    // -----------------------------------------------------------------------------
    
    // For each pixel
    for (int x = 0; x < IMAGE_WIDTH; x++) {
        for (int y = 0; y < IMAGE_HEIGHT; y++) {
            // cerr << "pixel " << x << "," << y << " :: ";
            int pixel = screen->pixel(x,y);

            // ----CALCULATE THE RAY FROM CAMERA TO PIXEL-----
            ray* curRay = new ray( campos, *getUnitDirToPixel(x,y) );
            cerr << *curRay << endl;
            // curRay->normalize();
            // -----------------------------------------------
            // cerr << "***\nthe direction of myray " << x<<","<<y << "is: " << curRay->unitDir[0] << ", "  << curRay->unitDir[1] << ", "  << curRay->unitDir[2] << endl;
            
            // ----TRAVERSE THE BVH
            int d=0;

            Vec3f* color = new Vec3f(0,0,0);
            bool b = traverseFlatArray(flat_array, 0, curRay, color, d);


    //         // Get the color for this pixel
    //         // double* color = getPixelColor(curRay);
            
            if(PRODUCE_IMAGE){
                // Assign colors to pixel
                screen->buffer[pixel] = color->x;
                screen->buffer[pixel+1] = color->y;
                screen->buffer[pixel+2] = color->z;
            }
            delete color;
            delete curRay;
        }
    }
    if( PRODUCE_IMAGE ) WriteImage(image, "myOutput") ;
}