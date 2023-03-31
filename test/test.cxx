// TEST FLAGS:
//#define FULL_TEST
//#define TEST_FULL_INFO
#define IN_DEV_TEST_RUN

#include "test/all_test.hxx"
#include "test/test_area.hxx"

auto main() -> int {
//  testTypesConversions();
//  testGenericValue();
//  testNumber();
//  testBits();
//  testBufferArray();
//  testAVLTree();
//  testMap();
//  testMultiMap();
//#ifdef FULL_TEST // Questionable or incompletely implemented tests
//  testRecursiveSharedMutex();
//  testVariable(); // Not ended!
//#endif
//  testAllBugs();

#ifdef IN_DEV_TEST_RUN
  TEST_ANNOUNCE(InDev tests:) GRP_PUSH
//    testTableImpl();
    runInDevTests();
  GRP_POP
#endif
}
