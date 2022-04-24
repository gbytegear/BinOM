#define TEST_FULL_INFO
#include "test/all_test.hxx"

int main() {
  testTypesConversions();
  std::cout.flush();
  testGenericValue();
  std::cout.flush();
  testRecursiveSharedMutex();
}
