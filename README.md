Clark Honors College Thesis
Kevin Beick 2014
==========

OVERVIEW
==========
This project is 


SOURCE FILES
==========
CHC_Thesis.cxx ------------
globals.h -----------------
BVH.h ---------------------
BVH_TopDownConstructor.h --
BVH_BottomUpConstructor.h -
objLoader.h ---------------
shading.h -----------------
Ray.h ---------------------
BBox.h -------------------- 
Camera.h ------------------ 
Triangle.h ---------------- 
Stopwatch.h --------------- 
utils.h -------------------


COMPILING & RUNNING
==========
I use CMake to compile.
After compiling, to run app...
USAGE: ./CHC_Thesis.app <model filename> <children per node (int)> <construction method (td or bu)> <camera pos (3 whitespace separated floats)> <depthOfTrace (int)> <opacity (float)>\n
e.g., ./CHC_Thesis.app/Contents/MacOS/CHC_Thesis ./models/buddha.obj  4   td    .5 0  -2     1   0.0


