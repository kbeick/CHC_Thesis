cmake_minimum_required(VERSION 2.8)

PROJECT(CHC_Thesis)
SET(VTK_DIR /Users/kevinbeick/Software/VTK-5.10)

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


add_executable(CHC_Thesis MACOSX_BUNDLE ${My_Sources})

if(VTK_LIBRARIES)
  target_link_libraries(CHC_Thesis ${VTK_LIBRARIES})
else()
  target_link_libraries(CHC_Thesis vtkHybrid)
endif()

