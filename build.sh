#!/bin/bash

mkdir build
cd build
mkdir linux
cd linux
cmake ../../../ -DCMAKE_BUILD_TYPE=Release
make -j $(getconf _NPROCESSORS_ONLN)
