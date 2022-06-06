//#define FULL_TEST
//#define TEST_FULL_INFO
#include "test/all_test.hxx"
#include "libbinom/include/utils/print_variable.hxx"

#include <mutex>

int main() {
  testTypesConversions();
  testGenericValue();
  testNumber();
  testBits();
  testBufferArray();

#ifdef FULL_TEST // Questionable or incompletely implemented tests
  testRecursiveSharedMutex();
  testVariable(); // Not ended!
#endif

  testAllBugs();

//  Variable a = 15;
//  Number b = 256;

//  Variable::Transaction tr(a, b);
//  if(b > 50) {
//    a.toNumber() += 50;
//    b -= 50;
//  }
//  tr.unlock();

}
