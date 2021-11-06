#include "binom/includes/binom.h"
#include "binom/includes/lexer.h"
#include <fstream>

#include "tests/file_node_visitor_test.h"


int main() {

  // BinOM project namespace (all magic here)
  using namespace binom;
//  bufferArrayInsertTest();
  setTest();

//  DynamicStorage storage("test.db", varr{}, true);
//  FileNodeVisitor root = storage;
//  root.pushBack("1");
//  root.pushBack("3");
//  root.pushFront("0");
//  root.insert(2, "2");
//  std::clog << root.getVariable() << '\n';
//  root.remove(0, 2);
//  std::clog << root.getVariable() << '\n';
//  root.remove(0, 2);
//  std::clog << root.getVariable() << '\n';




//  DynamicStorage storage("test.db", vobj{}, true);
//  FileNodeVisitor root = storage;
//  std::clog << root.getVariable() << '\n';
//  root.insert("Hello", "a");
//  std::clog << root.getVariable() << '\n';
//  root.insert("World", "b");
//  std::clog << root.getVariable() << '\n';
//  root.insert("Hello, world", "c");
//  std::clog << root.getVariable() << '\n';
//  root.insert("Hello, world!", "c");
//  std::clog << root.getVariable() << '\n';

//  FileNodeVisitor node = root["Hello"];
//  std::clog << "Name: " << *node.getName() << '\n';

//  root.remove("World");
//  std::clog << root.getVariable() << '\n';
//  root.remove("Hello, world");
//  std::clog << root.getVariable() << '\n';
//  root.remove("Hello, world!");
//  std::clog << root.getVariable() << '\n';
//  root.remove("Hello");
//  std::clog << root.getVariable() << '\n';

//  FileMemoryManager fmm("test.data", true);
//  virtual_index node_index = fmm.createNode(VarType::byte_array, BufferArray(ui8arr{0,1,4,5,8}).toByteArray());
//  ByteArray data = fmm.getNodeData(node_index);
//  std::clog << BufferArray(data) << '\n';
//  fmm.insertNodeDataPart(node_index, {
//                           {2, BufferArray(ui8arr{2,3}).toByteArray()},
//                           {4, BufferArray(ui8arr{6,7}).toByteArray()},
//                           {5, BufferArray(ui8arr{9,10,11}).toByteArray()},
//                           {5, BufferArray(ui8arr{12,13,14}).toByteArray()},
//                           {5, BufferArray(ui8arr{15,16}).toByteArray()},
//                         });
//  data = fmm.getNodeData(node_index);
//  std::clog << BufferArray(data) << '\n';
//  fmm.removeNodeDataParts(node_index, {{3,2}, {0,2}});


}
