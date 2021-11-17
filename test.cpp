#include "libbinom/include/binom.h"
#include "libbinom/include/lexer.h"
#include <fstream>

#include "tests/file_node_visitor_test.h"


int main() {

  // BinOM project namespace (all magic here)
  using namespace binom;

  BufferArray a = "Hello, World";
  BufferArray b = ui32arr{/*0x6c6c6548, 0x57202c6f, */0x646c726f};
  std::cout << "a: " << a << "\nb: " << b << '\n';
  auto it = a.findValue(b);
  std::cout << "find value: ";
  for(auto end = a.end();it != end;++it)
    std::cout << it->asI8();
  std::cout << "\nfind memory: ";
  it = a.findMemory(b);
  for(auto end = a.end();it != end;++it)
    std::cout << it->asI8();
  std::cout << '\n';

  a = ui32arr{0x6c6c6548, 0x57202c6f, 0x646c726f};
  b = "World";
  std::cout << "a: " << a << "\nb: " << b << '\n';
  it = a.findValue(b);
  std::cout << "find value: ";
  for(auto end = a.end();it != end;++it)
    std::cout << it->asI8();
  std::cout << "\nfind memory: ";
  it = a.findMemory(b);
  for(auto end = a.end();it != end;++it)
    std::cout << it->asI8();
  std::cout << '\n';

  Variable test = varr{
      "file.txt",
      "file.png",
      "file.cpp",
      "file.hpp",
      "file.binom",
      "file.binomdb"
  };

  {
  NodeVisitor node = test;

  // Query - O(n)
  std::clog << "-=- RAM storage\n";
  std::clog << "Find in:\n" << test << "\nby value regexp: ([a-z]+)\\.binom([a-z]*)\nResult:\n";
  for(auto& node : node.findSet({{QProp::value, QOper::reg_test, "([a-z]+)\\.binom([a-z]*)"}}))
    std::clog << node->toRAMVisitor().getVariable() << '\n';
  std::clog << "Find in:\n" << test << "\nby check contains in value: \"pp\"\nResult:\n";
  for(auto& node : node.findSet({{QProp::value, QOper::contains_values, "pp"}}))
    std::clog << node->toRAMVisitor().getVariable() << '\n';
  }

  DynamicStorage storage("test.db", test, true);
  {
    FileNodeVisitor node = storage;

    std::clog << "\n\n-=- File storage\n";
    std::clog << "Find in:\n" << test << "\nby value regexp: ([a-z]+)\\.binom([a-z]*)\nResult:\n";
    for(auto& node : node.findSet({{QProp::value, QOper::reg_test, "([a-z]+)\\.binom([a-z]*)"}}))
      std::clog << node->toFileVisitor().getVariable() << '\n';
    std::clog << "Find in:\n" << test << "\nby check contains in value: \"pp\"\nResult:\n";
    for(auto& node : node.findSet({{QProp::value, QOper::contains_values, "pp"}}))
      std::clog << node->toFileVisitor().getVariable() << '\n';
  }


  std::clog << "===============================================\n"
               "|                  Test complete              |\n"
               "===============================================\n";



}
