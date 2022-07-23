// TEST FLAGS:
#define FULL_TEST
#define TEST_FULL_INFO

#include "test/all_test.hxx"
#include "libbinom/include/binom_impl/avl_tree.hxx"

int main() {
  testTypesConversions();
  testGenericValue();
  testNumber();
  testBits();
  testBufferArray();
  testAVLTree();

#ifdef FULL_TEST // Questionable or incompletely implemented tests
  testRecursiveSharedMutex();
  testVariable(); // Not ended!
#endif

  testAllBugs();

}
