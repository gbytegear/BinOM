//#include "binom/includes/structure/variables/variable.h"
#include "binom/includes/binom.h"
#include "binom/includes/structure/file_storage/file_virtual_memory_controller.h"
#include <cassert>
#include <cmath>

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
  memory.createNodePage();
//  memory.createHeapPage(); // Test heap page autoallocation
  memory.createBytePage();
//  memory.createHeapPage();


  ByteArray data(7680);
  for(ui8& byte : data)
    byte = (ui8)(33+rand()%93);

  f_virtual_index pos = memory.allocHeapData(data).v_index;

  ByteArray data_loaded = memory.loadHeapData(pos);

  std::clog << "Data compare: " << data.isEqual(data_loaded) << '\n';
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
