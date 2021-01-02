# Company: HiTEM Engineering Inc.
# Description: CMakeLists.txt for ring module
# Author: David Park

set(LIB_NAME ring)
project(${LIB_NAME})

#include_directories(include)

file(GLOB_RECURSE SRC_FILES *.c)
file(GLOB_RECURSE HDR_FILES *.h)

add_library(${LIB_NAME} ${SRC_FILES} ${HDR_FILES})


