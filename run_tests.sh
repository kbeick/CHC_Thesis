#!/bin/sh

# say "go home Derek! you're bugging the crap outta me! Gosh!"

touch log_data_output.txt

make

./RayTracingThesis.app/Contents/MacOS/RayTracingThesis ./models/cube/cube.obj 2 td 0 0 4 .5 .5

say "all done"