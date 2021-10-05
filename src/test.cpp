//#include "tests/var_test.h"
//#include "tests/heap_blocks_test.h"
//#include "tests/memory_manager_test.h"
//#include "tests/file_node_visitor_test.h"
//#include"tests/multithreading_test.h"
#include "tests/query_test.h"

void foreachFNV(FileNodeVisitor& node, ui64 depth = 1) {
  for(auto child : node) {
    std::clog << std::string(depth, '|') << "Node index: " << child.getNodeIndex() << '\n';
    std::clog << std::string(depth, '|') << "Node type: " << toTypeString(child.getType()) << '\n';
    if(auto name = child.getName(); name)
      std::clog << std::string(depth, '|') << "Name: " << *name << '\n';
    if(child.isIterable())
      foreachFNV(child, depth + 1);
    else
      std::clog << std::string(depth, '|') << "Node value: " << child.getVariable() << '\n';
    std::clog << std::string(depth, '|') << "====\n";
  }
}

int main() {
  using namespace binom;
  try {
//    var_test();
//    test_heap_blocks();
//    memory_manager_test();
//    file_node_visitor_test();
//    file_storage_multithreading_test();
//    query_test();


//    std::string str_inp = "[1][2][3]name{byte:1,2,3}{word:1,2,3}{dword:1,2,3}{qword:1,2,3}[3][2][1]";
//    std::clog << "Input:                " << str_inp << '\n';
//    Path pth = Path::fromString("[1][2][3]name{byte:1,2,3}{word:1,2,3}{dword:1,2,3}{qword:1,2,3}[3][2][1]");
//    std::string str = pth.toString();
//    std::clog << "Path from input:      " << str << '\n';
//    Path pth_2 = Path::fromString(str);
//    std::clog << "Path->string->Path:   " << pth_2.toString() << '\n';


//    Variable var = vobj{
//    {"usr", varr{
//      vobj{
//        {"login", "root"},
//        {"password", "root"},
//        {"access_lvl", 0xFF_ui8},
//        {"user_data", vobj{}},
//        {"attributes", varr{"full_access", "unchangeable"}}
//      },
//      vobj{
//        {"login", "guest"},
//        {"password", "guest"},
//        {"access_lvl", 0x00_ui8},
//        {"user_data", vobj{}},
//        {"attributes", varr{"ro_access", "unchangeable"}}
//      },
//      vobj{
//        {"login", "admin"},
//        {"password", "admin"},
//        {"access_lvl", 0x7f_ui8},
//        {"user_data", vobj{}},
//        {"attributes", varr{"unchangeable"}}
//      }
//    }},
//    {"grp", varr{
//      vobj{
//        {"name", "system"},
//        {"attributes", varr{"unremovable"}},
//        {"users", varr{"root", "guest", "admin"}}
//      }
//    }}
//  };
//    SerializedStorage s_storage("data.binom");
//    s_storage = var;
//    FileStorage f_storage("data.binomdb", var, true);

//    std::clog << "Serialized storage:\n" << s_storage << "\n\n"
//                 "File storage:\n" << f_storage.getRoot().getVariable() << "\n\n";


//    FileNodeVisitor node = f_storage.getRoot();
//    std::clog << "Test file storage getName:\n"
//                 "Root type: " << toTypeString(node.getType()) << '\n';
//    foreachFNV(node);


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
