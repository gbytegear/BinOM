// TEST FLAGS:
//#define FULL_TEST
//#define TEST_FULL_INFO

#include "test/all_test.hxx"

#include "libbinom/include/binom_impl/ram_storage_implementation/multi_map_impl.hxx"

int main() {
  testTypesConversions();
  testGenericValue();
  testNumber();
  testBits();
  testBufferArray();
  testAVLTree();
  testMap();

#ifdef FULL_TEST // Questionable or incompletely implemented tests
  testRecursiveSharedMutex();
  testVariable(); // Not ended!
#endif

  enum class A {
    a = 1,
    b = 1,
    c = 3
  };

  constexpr bool test = A::a == A::b;
  UNUSED(test);

  testAllBugs();

}
