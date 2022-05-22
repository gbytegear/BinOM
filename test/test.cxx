//#define TEST_FULL_INFO
#include "test/all_test.hxx"
#include "libbinom/include/variables/buffer_array.hxx"

int main() {
  testTypesConversions();
  testGenericValue();
  testNumber();
  testBits();
  testRecursiveSharedMutex();
}
