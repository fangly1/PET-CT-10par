#!/bin/bash
echo "change to build directory..."
if [ -d "./build" ]; then
    cd ./build
    echo "clean evetyhing in build directory..."
    rm -rf *
fi
echo "clean evetyhing done..."
