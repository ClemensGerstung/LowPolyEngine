#!/bin/bash

cmake ./
make -j $(getconf _NPROCESSORS_ONLN)