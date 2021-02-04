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

//  FileIO file("test_db.binomdb");

//  ui64 index;

//  std::clog << std::hex << 0x1064f << ':' << std::dec << 0x1064f << '\n';

//  file.read(0x1064f, index);

//  std::clog << std::hex << index << ':' << std::dec << index << '\n';

//  file.read(index, index);

//  std::clog << std::hex << index << ':' << std::dec << index << '\n';

//  file.read(index, index);

//  std::clog << std::hex << index << ':' << std::dec << index << '\n';

  FileVirtualMemoryController memory("test_db.binomdb");
  // TODO: Complete & test public interface of FVMC

  ByteArray hdata(sizeof (f_virtual_index)*256);
  for(ui8& val : hdata)
    val = 33+rand()%98;
  ByteArray bdata(8);
  for(ui8& val : bdata)
    val = 33+rand()%98;

  for(ui8 i = 0; i < 80; ++i) {
    memory.createNode(VarType::array, hdata);
    memory.createNode(VarType::qword, bdata);
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
