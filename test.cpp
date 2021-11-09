#include "libbinom/include/binom.h"
#include "libbinom/include/lexer.h"
#include <fstream>

#include "tests/file_node_visitor_test.h"


int main() {

  // BinOM project namespace (all magic here)
  using namespace binom;
//  bufferArrayInsertTest();
//  setTest();

  DynamicStorage storage("test.db", vobj{
                           {"a", 1_ui8},
                           {"b", 2_ui16},
                           {"c", 3_ui32},
                           {"d", 4_ui64},
                           {"Hello", "World"},
                           {"Iterator", "Test"},
                           {"Array", varr{
                              "Hello",
                              "World",
                              1_ui8,
                              2_ui16,
                              3_ui32,
                              4_ui64
                            }}
                         }, true);
  FileNodeVisitor root = storage;
//  for(FileNodeVisitor node : root) {
//    std::clog << "name: " << *node.getName() << "; value: " << node.getVariable() << '\n';
//    if(node.isIterable())
//      for(FileNodeVisitor sub_node : node) {
//        std::clog << "|value: " << sub_node.getVariable() << '\n';
//      }
//  }

  for(auto it = root.beginFrom("Array"),
           end = root.end();
      it != end; ++it) {
    FileNodeVisitor node = *it;
    std::clog << "name: " << *node.getName() << "; value: " << node.getVariable() << '\n';
  }

  std::clog << "===============================================\n"
               "|                  Test complete              |\n"
               "===============================================\n";



}
