//#define FULL_TEST
//#define TEST_FULL_INFO
#include "test/all_test.hxx"
#include "libbinom/include/utils/print_variable.hxx"

int main() {
  testTypesConversions();
  testGenericValue();
  testNumber();
  testBits();
#ifdef FULL_TEST
  testRecursiveSharedMutex();
#endif
  testVariable();
}
