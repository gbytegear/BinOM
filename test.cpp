#include "libbinom/include/binom.h"
#include "libbinom/include/lexer.h"
#include <fstream>

#include "tests/file_node_visitor_test.h"


int main() {

  // BinOM project namespace (all magic here)
  using namespace binom;

  DynamicStorage storage("test.db", vobj{
                           {"hello","world"},
                           {"first",varr{
                              vobj{
                                {"third", "value"}
                              }
                            }}
                         }, true);
  FileNodeVisitor root = storage;
  std::clog << root["hello"].getVariable() << '\n';
  root.setVariable("hello", "anything");
  std::clog << root["hello"].getVariable() << '\n';
  std::clog << "Contains {first, 0, third}: " << root.contains({"first", 0, "third"}) << '\n'
            << "Contains {first, 0, isn't_exist}: " << root.contains({"first", 0, "isn't_exist"}) << '\n'
            << "Contains {first, 0, third, fourth}: " << root.contains({"first", 0, "third", "fourth"}) << '\n';

//  for(auto it = root.begin(), end = root.end();
//      it != end; ++it) {
//    FileNodeVisitor node = *it;
//    std::clog << "name: " << *node.getName() << "; value: " << node.getVariable() << '\n';
//  }

  std::clog << "===============================================\n"
               "|                  Test complete              |\n"
               "===============================================\n";



}
