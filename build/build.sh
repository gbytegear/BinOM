#!/usr/bin/bash
cmake -G "MSYS Makefiles" .. -DCMAKE_BUILD_TYPE=Debug
make clean
make -j4
