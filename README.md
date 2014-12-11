#Ray Tracing with BVHs 
####Kevin Beick 2014  
####Clark Honors College Thesis Project  

Source code for the thesis "Analyzing Performance of Bounding Volume Hierarchies for Ray Tracing" by Kevin Beick in partial fulfillment of the requirements for the degree of Bachelor of Arts from the Robert D. Clark Honors College at the University of Oregon.

Some code herein is borrowed from or inspired by other implementations, as noted in the appropriate source files.


OVERVIEW
==========
This project is an implemenation of the Ray Tracing algorithm that uses Bounding Volume Hierarchies. BVHs accelerate ray tracing, in partiular its intersection detection process. This code loads data from .obj files and stores the model as its constituent geometric objects. These objects inserted into a BVH and then the ray tracing algorithm uses the BVH to efficiently generate a 3D rendering of the model.

The code allows for some customization to the BVH and ray tracing process, letting the user define some of the parameters:
- construction method used to generate BVH (top-down or bottom-up)
- branching factor of the BVH (2,4,8)
- depth of the trace (primary rays only, include secondary rays, etc.) 
- universal opacity of the model(s)
- view point in space (camera always faces the origin)

Also included is support to record/stream various metrics of the BVH's performance:
- Build Time
- Avg traversal time per pixel
- Avg number of nodes traverse per pixel



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
   e.g., `./CHC\_Thesis.app ./models/buddha.obj  4   td    .5 0  -2     1   0.0`


