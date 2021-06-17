#include "binom/includes/binom.h"

#include <ctime>
#include <functional>

const char SEPARATOR[] = "=========================================================================\n\n";

using namespace binom;

void testAnything() {
  time_t t;
  Object f_block = {
    {"index", 0_ui32},
    {"nonce", 0_ui64},
    {"data", "data"},
    {"hash", ""},
    {"time", 0_i64}
  };
  Object block = {
    {"index", 1_ui32},
    {"nonce", 0_ui64},
    {"data", "data"},
    {"hash", std::hash<std::string>{}(std::move((char*)f_block.serialize().unfree()))},
    {"time", 0_i64}
  };

  std::cout << toTypeString(block["hash"].type()) << '\n';

}

int main() {
  try {

    testAnything();

    std::clog << "=========================================================================\n"
              << "|                    Test completed successfully                        |\n"
              << "=========================================================================\n";

    throw Exception(ErrCode::any, "Test");

  } catch(Exception& except) {
    std::cerr << except.full() << std::endl;
  } catch(std::exception& except) {
    std::cerr << except.what() << std::endl;
  }

  return 0;
}
