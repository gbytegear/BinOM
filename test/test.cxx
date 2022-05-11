//#define TEST_FULL_INFO
#include "test/all_test.hxx"

int main() {
  testTypesConversions();
  testGenericValue();
  testNumber();
  testBits();
  testRecursiveSharedMutex();
}
