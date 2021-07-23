#include "tests/var_test.h"
#include "tests/heap_blocks_test.h"
#include "tests/memory_manager_test.h"

int main() {
  try {
    var_test();
    test_heap_blocks();
    memory_manager_test();



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
