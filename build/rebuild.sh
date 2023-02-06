#!/usr/bin/bash
cmake -G "MSYS Makefiles" .. -DCMAKE_BUILD_TYPE=Debug
make binom -j4
make test -j4
