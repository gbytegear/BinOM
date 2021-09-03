//#include "tests/var_test.h"
//#include "tests/heap_blocks_test.h"
//#include "tests/memory_manager_test.h"
//#include "tests/file_node_visitor_test.h"
//#include"tests/multithreading_test.h"
#include "tests/query_test.h"

int main() {
  using namespace binom;
  try {
//    var_test();
//    test_heap_blocks();
//    memory_manager_test();
//    file_node_visitor_test();
//    file_storage_multithreading_test();
    query_test();


    std::clog << "=========================================================================\n"
                 "|                    Test completed successfully                        |\n"
                 "=========================================================================\n";
    return 0;

  } catch(Exception& except) {
    std::cerr << except.full() << std::endl;
  } catch(std::exception& except) {
    std::cerr << except.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception!\n";
  }

  std::cerr << "=========================================================================\n"
               "|                   !!! Test ended abnormally !!!                       |\n"
               "=========================================================================\n";

}
