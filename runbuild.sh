#!/bin/bash
echo "change to build directory..."
cd ./build
echo "clean evetyhing..."
rm -rf *
echo "cmake now..."
cmake ..
#echo "make program..."
make
