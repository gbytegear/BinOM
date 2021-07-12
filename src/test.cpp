#include "examples/multithread.h"

int main() {
  try {

    test();

    std::clog << "=========================================================================\n"
                 "|                    Test completed successfully                        |\n"
                 "=========================================================================\n";

  } catch(Exception& except) {
    std::cerr << except.full() << std::endl;
  } catch(std::exception& except) {
    std::cerr << except.what() << std::endl;
  }

  return 0;
}
