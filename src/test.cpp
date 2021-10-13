//#include "tests/var_test.h"
//#include "tests/heap_blocks_test.h"
//#include "tests/memory_manager_test.h"
//#include "tests/file_node_visitor_test.h"
//#include"tests/multithreading_test.h"
#include "tests/query_test.h"

//void foreachFNV(FileNodeVisitor& node, ui64 depth = 1) {
//  for(auto child : node) {
//    std::clog << std::string(depth, '|') << "Node index: " << child.getNodeIndex() << '\n';
//    std::clog << std::string(depth, '|') << "Node type: " << toTypeString(child.getType()) << '\n';
//    if(auto name = child.getName(); name)
//      std::clog << std::string(depth, '|') << "Name: " << *name << '\n';
//    if(child.isIterable())
//      foreachFNV(child, depth + 1);
//    else
//      std::clog << std::string(depth, '|') << "Node value: " << child.getVariable() << '\n';
//    std::clog << std::string(depth, '|') << "====\n";
//  }
//}

int main() {
  using namespace binom;
  try {

//    Path pth = {1,2,3,"name","name_2",ui8arr{1,2,3},ui16arr{1,2,3},ui32arr{1,2,3},ui64arr{1,2,3}};
//    std::clog << pth.toString() << '\n';
//    Path pth_2 = Path::fromString(pth.toString());
//    std::clog << pth_2.toString() << '\n';

    SerializedStorage ser_storage("test.binom");
    ser_storage = vobj {
      {"Hello", "World"},
      {ui16arr{1,2,3,4}, ui64arr{5,6,7,8}}
    };
    Variable var = ser_storage;
    std::clog << "Deserialized: " << var << '\n';


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
