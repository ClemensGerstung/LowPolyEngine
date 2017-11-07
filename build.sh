#!/bin/bash

mkdir build
cd build
mkdir linux
cd linux
cmake ../../../ -DCMAKE_BUILD_TYPE=Release
make -j $(getconf _NPROCESSORS_ONLN)
cd ..
mkdir win
cd win
mkdir x86
cd x86
cmake ../../../ -DCMAKE_TOOLCHAIN_FILE=../../../external/glfw/CMake/x86_64-w64-mingw32.cmake -DCMAKE_BUILD_TYPE=Release
make -j $(getconf _NPROCESSORS_ONLN)
cd ..
mkdir x64
cd x64
cmake ../../../ -DCMAKE_TOOLCHAIN_FILE=../../../external/glfw/CMake/amd64-mingw32msvc.cmake -DCMAKE_BUILD_TYPE=Release
make -j $(getconf _NPROCESSORS_ONLN)
cd ..