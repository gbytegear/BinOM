//#define FULL_TEST
//#define TEST_FULL_INFO
#include "test/all_test.hxx"
#include "libbinom/include/utils/print_variable.hxx"

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
}
