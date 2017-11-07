#!/bin/bash

mkdir build
cd build
cmake ./
make -j $(getconf _NPROCESSORS_ONLN)