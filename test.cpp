#include "binom/includes/binom.h"

#include <ctime>

const char SEPARATOR[] = "=========================================================================\n\n";

using namespace binom;

void testAnything() {
  Object test_obj = {
    {ui64arr{65535,65535}, "big name"},
    {ui64arr{255,65535}, "big name"},
    {"test", "prop"}
  };

  std::cout << test_obj << '\n';
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
