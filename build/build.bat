cmake -G "MSYS Makefiles" . -DCMAKE_BUILD_TYPE=Debug
make clean
make binom -j4
make test -j4
