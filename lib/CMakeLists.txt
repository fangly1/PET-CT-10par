cmake_minimum_required(VERSION 2.6)

aux_source_directory(. LM_SRC_LIST)
        
message(STATUS "lib This is a BINARY dir " ${PROJECT_BINARY_DIR})
message(STATUS "lib This is a SOURCE dir " ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/lib/)

add_library(lm ${LM_SRC_LIST})
