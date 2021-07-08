#include "examples/binom_tcp_io.h"

const char SEPARATOR[] = "=========================================================================\n\n";


int main() {
  try {

    std::clog << "=========================================================================\n"
                 "|                    Test completed successfully                        |\n"
                 "=========================================================================\n";

    throw Exception(ErrCode::any, "Test");

  } catch(Exception& except) {
    std::cerr << except.full() << std::endl;
  } catch(std::exception& except) {
    std::cerr << except.what() << std::endl;
  }

  return 0;
}
