#include "toolkit/cli.h"
using namespace binom;
int main(int argc, char* argv[]){try{CLI(argc, argv);}catch(const Exception& except) {std::cerr << except.full() << '\n';}}
