cmake_minimum_required(VERSION 2.6)
#add configure file

aux_source_directory(. PROG_SRC_LIST)
     
message(STATUS "src This is a BINARY dir " ${PROJECT_BINARY_DIR})
message(STATUS "src This is a SOURCE dir " ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/src/)
include_directories(${PROJECT_SOURCE_DIR}/lib/)

add_executable(dp ${PROG_SRC_LIST})

target_link_libraries(${PROJECT_NAME} m lm)