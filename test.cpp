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
  // TODO: Complete & test public interface of FVMC

  ByteArray data(4096*8);
  {
    ui64 i = 0;
    for(ui64* it = data.begin<ui64>(); it != data.end<ui64>(); (++it, ++i))
      *it = i;
  }
  f_virtual_index node_index = memory.createNode(VarType::array, data);

  for(ui64 i = 0; i < 4096; ++i) {
    ByteArray loaded_data(memory.loadHeapDataPart(node_index, i*sizeof(ui64), sizeof(ui64)));
    std::clog << "Loaded number: " << std::dec << loaded_data.get<ui64>(0) << '\n';
  }

}


int main() {
  try {
    testVariable();
    std::clog << "===================================================================\n";
    testDB();

    std::clog << "Test ended!\n";

  } catch(binom::SException except) {
    std::cerr << binom::SException::ectos(except.code()) << except.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception!" << std::endl;
  }

  return 0;
}
