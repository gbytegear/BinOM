//#define TEST_FULL_INFO
#include "test/all_test.hxx"

#include "libbinom/include/variables/number.hxx"

int main() {
  RAIIPerfomanceTest test_perf("Test perfomance: ");
  testTypesConversions();
  std::cout.flush();
  testGenericValue();
  std::cout.flush();
  testRecursiveSharedMutex();
}
