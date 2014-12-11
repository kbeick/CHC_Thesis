##Clark Honors College Thesis
##Kevin Beick 2014


OVERVIEW
==========
This project is 


SOURCE FILES
==========
*CHC_Thesis.cxx ------------  
*globals.h -----------------  
*BVH.h ---------------------  
*BVH_TopDownConstructor.h --  
*BVH_BottomUpConstructor.h -  
*objLoader.h ---------------  
*shading.h -----------------  
*Ray.h ---------------------  
*BBox.h --------------------   
*Camera.h ------------------   
*Triangle.h ----------------   
*Stopwatch.h ---------------   
*utils.h -------------------  


COMPILING & RUNNING
==========
   I use CMake to compile.  
   After compiling, to run app...  
   USAGE: *./CHC\_Thesis.app* _**model filename** (path)_ _**children per node** (int)_ _**construction method** ('td' or 'bu')_ _**camera pos** (3 whitespace separated floats)_ _**depthOfTrace** (int)_ _**opacity** (float)_\n  
   e.g., `./CHC\_Thesis.app/Contents/MacOS/CHC\_Thesis ./models/buddha.obj  4   td    .5 0  -2     1   0.0`


