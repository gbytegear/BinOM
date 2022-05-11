//#define TEST_FULL_INFO
#include "test/all_test.hxx"

#include "libbinom/include/variables/number.hxx"

#include <vector>
#include <initializer_list>
#include <array>

#include <assert.h>

int main() {
  testTypesConversions();
  std::cout.flush();
  testGenericValue();
  std::cout.flush();
  testNumber();
  std::cout.flush();
  testBits();
  std::cout.flush();
  testRecursiveSharedMutex();
}
