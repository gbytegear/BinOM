//#include "binom/includes/structure/variables/variable.h"
#include "binom/includes/binom.h"
#include <cassert>

using namespace binom;

void testVariable() {
  Variable p(56_ui64);
  Variable bf(ui8arr{1_ui8, 2_ui8, 3_ui8});
  Variable a(varr{1,2,3,4,5});
  Variable o(obj{{"Hello", "World"}, {"Goodby", "World"}});
  std::clog << p << "\n\n"
            << bf << "\n\n"
            << a << "\n\n"
            << o << "\n\n";
}

void testDB() {
  DBFile db_file("test.binomdb");

  std::clog << std::dec
            << "File size: " << db_file.getFileSize() << " bytes\n"
            << "|Node segments count: " << db_file.getNodeSegmentsCount() << "\n"
            << "|Node segments size: " << db_file.getNodeSegmentsSize() << " bytes\n"
            << "|Node count: " << db_file.getNodeCount() << "\n"
            << "|Primitive segment count: " << db_file.getPrimitiveSegmentsCount() << "\n"
            << "|Primitive segment size: " << db_file.getPrimitiveSegmentsSize() << " bytes\n"
            << "|Data segment count: " << db_file.getDataSegmentsCount() << "\n"
            << "|Data segment size: " << db_file.getDataSegmentsSize() << " bytes\n";
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
