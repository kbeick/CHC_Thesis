##Clark Honors College Thesis
##Kevin Beick 2014


OVERVIEW
==========
This project is 


SOURCE FILES
==========
file | description 
--- | ---  
CHC_Thesis.cxx | holds main function including: read-in parameters, construct BVH, ray tracing, stream evaluation data, produce image file.
globals.h | defines static variables used throughout the project.
BVH.h | BVH_Node class and its auxiliaries plus bvhToFlatArray() function.
BVH_TopDownConstructor.h | function for Top Down construction of BVH plus its auxiliaries.  
BVH_BottomUpConstructor.h | function for Bottom Up construction of BVH plus its auxiliaries. 
objLoader.h | classes/functions related to the loading/storing of data from .obj files.
shading.h | contains functions related to determining pixels' coloring/lighting, including traverseFlatArray() function.
Ray.h | contains Ray class.
BBox.h | Bounding Box struct definition. 
Camera.h | contains Camera class.   
Triangle.h | contains the Triangle class and auxiliary functions related to Triangles (e.g., CreateTriangleArray()).
Stopwatch.h | contains the Stopwatch class.
utils.h | holds some general helper classes and functions that are used throughout the project. 


COMPILING & RUNNING
==========
   I use CMake to compile.  
   After compiling, to run app...  
   USAGE: *./CHC\_Thesis.app* _**model filename** (path)_ _**children per node** (int)_ _**construction method** ('td' or 'bu')_ _**camera pos** (3 whitespace separated floats)_ _**depthOfTrace** (int)_ _**opacity** (float)_  
   e.g., `./CHC\_Thesis.app/Contents/MacOS/CHC\_Thesis ./models/buddha.obj  4   td    .5 0  -2     1   0.0`


