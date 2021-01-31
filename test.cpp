//#include "binom/includes/structure/variables/variable.h"
#include "binom/includes/binom.h"
#include "binom/includes/structure/file_storage/file_virtual_memory_controller.h"
#include <cassert>

using namespace binom;

void testVariable() {
  Variable p(56_ui64);
  Variable bf(ui8arr{1_ui8, 2_ui8, 3_ui8});
  Variable a(varr{1,2,3,4,5});
  Variable o(obj{{"Hello", "World"}, {"Goodby", "World"}});

  std::clog << p << "\n"
            << bf << "\n"
            << a << "\n"
            << o << "\n\n";

  a = varr{p,bf,a,o};
  p = 10_ui32;
  bf = "Hello World";
  o = obj{{"Hello","Goodby"}, {"World", "World"}};

  std::clog << p << "\n"
            << bf << "\n"
            << a << "\n"
            << o << "\n\n";
}

void testDB() {
  FileVirtualMemoryController memory("test_db.binomdb");

  memory.allocNode(NodeDescriptor{VarType::array, 0, 16});
  memory.allocNode(NodeDescriptor{VarType::byte, 0, 1});
  memory.freeNode(1);
  memory.allocNode(NodeDescriptor{VarType::qword, 1, 8});
  NodeDescriptor des;
  memory.loadNode(1, des);

  std::clog << std::dec
            << des.index << '\n'
            << des.size << '\n';
}


int main() {
  try {
    testVariable();
    std::clog << "===================================================================\n";
    testDB();


  } catch(binom::SException except) {
    std::cerr << binom::SException::ectos(except.code()) << except.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception!" << std::endl;
  }

  return 0;
}
