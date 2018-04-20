cmake_minimum_required(VERSION 2.6)
project(dp)

#add configure file
configure_file (
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_SOURCE_DIR}/config.h"
    )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -ggdb -Wall -std=gnu99 -fopenmp")    

message(STATUS "This is a BINARY dir " ${PROJECT_BINARY_DIR})
message(STATUS "This is a SOURCE dir " ${PROJECT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/ouoput)
message(STATUS "EXECUTABLE_OUTPUT_PATH " ${EXECUTABLE_OUTPUT_PATH})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/ouoput)
message(STATUS "LIBRARY_OUTPUT_PATH " ${LIBRARY_OUTPUT_PATH})

add_subdirectory(lib)
add_subdirectory(src)