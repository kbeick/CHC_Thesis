cmake_minimum_required(VERSION 2.8)

PROJECT(RayTracingThesis)
SET(VTK_DIR /Users/kevin\ beick/Software/VTK5.10.1)
#SET(VTK_DIR /Users/kevinbeick/Software/VTK5.10.1)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


file(GLOB_RECURSE My_Sources "./*.cxx")
file(GLOB_RECURSE My_Headers "./*.h")

set (Foo_INCLUDE_DIRS "")
foreach (_headerfile ${My_Headers})
    get_filename_component(_dir ${_headerfile} PATH)
    list (APPEND Foo_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES Foo_INCLUDE_DIRS)


add_executable(RayTracingThesis MACOSX_BUNDLE ${My_Sources})

if(VTK_LIBRARIES)
  target_link_libraries(RayTracingThesis ${VTK_LIBRARIES})
else()
  target_link_libraries(RayTracingThesis vtkHybrid)
endif()

