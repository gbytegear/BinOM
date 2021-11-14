#include "libbinom/include/binom.h"
#include "libbinom/include/lexer.h"
#include <fstream>

#include "tests/file_node_visitor_test.h"


int main() {

  // BinOM project namespace (all magic here)
  using namespace binom;

  DynamicStorage storage("test.db", vobj{}, true);
  FileNodeVisitor root = storage;

  for(auto it = root.begin(), end = root.end();
      it != end; ++it) {
    FileNodeVisitor node = *it;
    std::clog << "name: " << *node.getName() << "; value: " << node.getVariable() << '\n';
  }

  std::clog << "===============================================\n"
               "|                  Test complete              |\n"
               "===============================================\n";



}
