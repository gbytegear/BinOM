#include "tests/var_test.h"
#include "tests/heap_blocks_test.h"

namespace binom {
void test() {
  HeapMap hmap;

  hmap.expandMemory(4096);
//  hmap.allocBlock(255);
//  hmap.allocBlock(1024);
  hmap.occupyBlock(5, 10);
  hmap.occupyBlock(35, 40);
  hmap.occupyBlock(15, 20);

  hmap.check();
}
}

int main() {
  try {

    test();
//    test_heap_blocks();



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
