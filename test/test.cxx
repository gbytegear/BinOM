//#define TEST_FULL_INFO
#include "test/all_test.hxx"

#include "libbinom/include/variables/number.hxx"

int main() {
  RAIIPerfomanceTest test_perf("Test perfomance: ");
  testTypesConversions();
  std::cout.flush();
  testGenericValue();
  std::cout.flush();
  testNumber();
  std::cout.flush();
  testRecursiveSharedMutex();

//  Number a = 12;
//  int b = a;

//  Number a = 12, c = 16;
//  GenericValue b = a;
//  a += b;
//  a += c;
}
