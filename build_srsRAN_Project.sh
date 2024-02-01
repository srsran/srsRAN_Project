#!/bin/bash
mkdir build
mkdir static_libs
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../static_libs
make -j$(nproc)