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
//    query_test();

    Variable var = vobj{
    {"usr", varr{
      vobj{
        {"login", "root"},
        {"password", "root"},
        {"access_lvl", 0xFF_ui8},
        {"user_data", vobj{}},
        {"attributes", varr{"full_access", "unchangeable"}}
      },
      vobj{
        {"login", "guest"},
        {"password", "guest"},
        {"access_lvl", 0x00_ui8},
        {"user_data", vobj{}},
        {"attributes", varr{"ro_access", "unchangeable"}}
      },
      vobj{
        {"login", "admin"},
        {"password", "admin"},
        {"access_lvl", 0x7f_ui8},
        {"user_data", vobj{}},
        {"attributes", varr{"unchangeable"}}
      }
    }},
    {"grp", varr{
      vobj{
        {"name", "system"},
        {"attributes", varr{"unremovable"}},
        {"users", varr{"root", "guest", "admin"}}
      }
    }}
  };
    SerializedStorage s_storage("data.binom");
    s_storage = var;
    FileStorage f_storage("data.binomdb", var, true);

    std::clog << "Serialized storage:\n" << s_storage << "\n\n"
                 "File storage:\n" << f_storage.getRoot().getVariable() << "\n\n";


    FileNodeVisitor node = f_storage.getRoot();//({"usr", 0});
    std::clog << "Test file storage getName:\n"
                 "Root type: " << toTypeString(node.getType()) << '\n';
    for(FileNodeVisitor child : node) {
      std::clog << *child.getName() << '\n';
    }


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
