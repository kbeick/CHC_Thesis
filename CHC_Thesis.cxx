//  Kevin Beick's Clark Honors College Thesis Project - 2014
//  Author: Kevin Beick

/*  Main.cxx
    - Run command line interface
    - Set parameters
    - Construct BVH
    - Ray Tracing
    - streams evaluation data
    - produce image file
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

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
#include <vtkImageCast.h>

#include <numeric>
#include <cmath>
#include <vector>
#include <string>

#include "globals.h"

#include "BVH.h"
#include "BVH_BottomUpConstructor.h"
#include "BVH_TopDownConstructor.h"

#include "BBox.h"
#include "Ray.h"
#include "Shading.h"
#include "Triangle.h"
#include "utils.h"
#include "objLoader.h"
#include "Stopwatch.h"
#include "Camera.h"

using std::cerr;
using std::cout;
using std::endl;



/* ----------------------------------------------------------------------------- */
/* ---------- PARAMETERIZATION ---------- */
/* ----------------------------------------------------------------------------- */

void
print_params()
{
    cerr << "~~~~read in params:" << endl;
    cerr << "~~~~constr method is: " << construction_method << endl;
    cerr << "~~~~got camera pos: " << campos[0] << ", " << campos[1] << ", " << campos[2] << endl;
}

int
SetConstructionMethod(char* input)
{
    int construction_method = -1;
    if(strcmp(input, "td")==0) construction_method = TOPDOWN;
    else if(strcmp(input, "bu")==0) construction_method = BOTTOMUP;
    else{
        // cerr << input << endl;
        throw std::invalid_argument( "ERROR: Construction method must be either td or bu (top down or bottom up)\n" );
    }
    return construction_method;
}

Camera*
SetUpCamera()
{
    Camera *c = new Camera();
    c->near = 1;
    c->far = 200;
    c->angle = M_PI/6;
    c->focus = new Vec3f(0,0,0);
    c->up = new Vec3f(0,-1,0);
    return c;
}

/* ----------------------------------------------------------------------------- */
/* ---------- IMAGE PRODUCTION ---------- */
/* ----------------------------------------------------------------------------- */

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

/* ----------------------------------------------------------------------------- */
/* ---------- MAIN ---------- */
/* ----------------------------------------------------------------------------- */

int main(int argc, char** argv)
{
    Stopwatch* stopwatch = new Stopwatch();

    /* PREFATORY DATA LOG STUFF */
    ofstream data_log;
    data_log.open("log_data_output_-_3.txt", ios_base::app);
    data_log << "___________________________________\n";
    data_log << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    /* SET UP CAMERA */
    c = SetUpCamera();

    /* HANDLE AND SET PARAMETERS */
    if (argc != 9 && argc != 10){ cerr << USAGE_MSG; exit(0);}
    cerr << "PROCESSING " << argv[1] << endl;
    try{
        objReader = new ObjReader(argv[1]);
        branching_factor = atoi( argv[2] );
        construction_method = SetConstructionMethod(argv[3]);
        c->position = new Vec3f(atof(argv[4]),atof(argv[5]),atof(argv[6]));
        depthOfTrace = atof(argv[7]);
        opacity = atof(argv[8]);

    }catch (const std::exception &exc){
        // catch anything thrown within try block that derives from std::exception
        std::cerr << exc.what();
        cerr << USAGE_MSG; 
        exit(0);
    }

    data_log << "File: " << argv[1] <<
        "  Branching Factor: " << branching_factor <<
        "  Constr Method: " << argv[3] <<
        "  Campos: " << *c->position <<
        "  Depth of Trace: " << depthOfTrace <<
        "  Opacity: " << opacity <<
        endl;

    char *outFileName;
    if(argv[9]){ outFileName = argv[9]; }
    else{ outFileName = (char*)"myOutput"; }

    EMPTY_NODE_BBOX = new BBox(*c->position * 1.1, *c->position * 1.1);

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

    /* GET RAW DATA FROM objReader */
    objReader->extractNormals();

    numTriangles = objReader->totalTriangles;
    data_log << "NUMBER OF TRIANGLES: " << numTriangles << endl;

    float* verts;
    float* normals;
    objReader->getRawData(verts, normals);
    tris = new Triangle[numTriangles];
    CreateTriangleArray(tris, numTriangles, verts, normals);

    /* INITIALIZE BVH */
    BVH_Node *root = new BVH_Node();
    root->id = 0;
    root->parent = NULL;

    /* START TIMER */
    stopwatch->reset();

    /* CALL SPECIFIED BVH CONSTRUCTOR */
    if (construction_method == TOPDOWN){         
        BuildBVH_topdown(tris, root, root->parent, numTriangles, 0);
    }
    else if (construction_method == BOTTOMUP){   
        BuildBVH_bottomup(tris, &root, numTriangles); 
        if(branching_factor!=2){ BVH_Bottomup_Collapser(root); }
    }

    data_log << "BUILD TIME  " << stopwatch->read() << endl;
    //cerr << "\nFINISHED BUILDING TREE " << (construction_method == TOPDOWN ? "top down" : "bottom up" ) << endl;
    
    /* GENERATE FLAT ARRAY REP OF BVH */
    int flat_array_len;
    flat_array = bvhToFlatArray(root, &flat_array_len, branching_factor);
    // cerr << "flat_array_len " << flat_array_len << endl;

    /* PRINT FLAT ARRAY TO COMMAND LINE */
    // for(int a=0; a<flat_array_len; a++){
    //     if(flat_array[a]==LEAF_FLAG){ cerr << "idx " << a << ": "; }
    //     cerr << flat_array[a] << endl;
    // }

    
    /* ----------------------------------------------------------------------------- */
    /* ---------- DO RAY TRACING ---------- */
    /* ----------------------------------------------------------------------------- */
    printf("\n~~~~~~ RAY TRACING ~~~~~~\n");
    
    int npixels = IMAGE_WIDTH*IMAGE_HEIGHT;
    double traversal_times[npixels];
    std::vector<int> node_visit_data (npixels, 0);

    /* PREP WORK */
    Ray* look = RayFromPoints(*c->position, *c->focus);

    Vec3f* u = crossProductNormalized(look->unitDir, *c->up);
    Vec3f* v = crossProductNormalized(look->unitDir, *u);

    Vec3f x = (*u) * (2*tan((c->angle)/2)/IMAGE_WIDTH);
    Vec3f y = (*v) * (2*tan((c->angle)/2)/IMAGE_HEIGHT);

    /* FOR EACH PIXEL... */
    for (int h = 0; h < IMAGE_HEIGHT; h++) {
        for (int w = 0; w < IMAGE_WIDTH; w++) {
            int pixel = screen->pixel(w,h);

            Vec3f x_comp = x*((2*w+1-IMAGE_WIDTH)/2);
            Vec3f y_comp = y*((2*h+1-IMAGE_HEIGHT)/2);

            /* ---- CALCULATE THE RAY FROM CAMERA TO PIXEL ---- */
            Vec3f rayVector = look->unitDir + x_comp + y_comp;
            Ray* curRay = new Ray(*c->position, rayVector);
            // cerr << "\n\npixel " << w << "," << h << " :: ";

            /* ---- TRAVERSE THE BVH ---- */
            Vec3f* color = new Vec3f(0,0,0);    /* Pixel Color */
            stopwatch->reset();                 /* Start Timer */

            traverseFlatArray(flat_array, 0, curRay, color, depthOfTrace, &node_visit_data[pixel/3]);
            
            /* Record Traversal Time */
            traversal_times[w*IMAGE_HEIGHT + h] = stopwatch->read();

            // cerr << "traversal complete" <<endl;
            
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
    /* LOG EVAL METRICS TO DATA FILE */
    double avg_time = std::accumulate(traversal_times,traversal_times+npixels,0.0) / npixels;
    data_log << "AVG TRAVERSAL TIME PER PIXEL:  " << avg_time << endl;
    double avg_num_visits = std::accumulate(node_visit_data.begin(),node_visit_data.end(),0.0) / npixels;
    data_log << "AVG # NODES VISTED PER PIXEL:  " << avg_num_visits << endl;
    if(construction_method == BOTTOMUP){
        data_log << "AVG # CHILDREN PER INNER NODE: " << (double)branching_factor - ((double)emptyNodeCount/(double)inner_node_counter) << endl;
    }

    if( PRODUCE_IMAGE ) WriteImage(image, outFileName) ;
    data_log.close();
}