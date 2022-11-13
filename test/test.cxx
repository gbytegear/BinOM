// TEST FLAGS:
//#define FULL_TEST
//#define TEST_FULL_INFO

#include "test/all_test.hxx"

//#include <iostream>

#include <mutex>
#include "libbinom/include/binom_impl/ram_storage_implementation/index_impl.hxx"
#include "libbinom/include/variables/key_value.hxx"

auto main() -> int {
  testTypesConversions();
  testGenericValue();
  testNumber();
  testBits();
  testBufferArray();
  testAVLTree();
  testMap();
  testMultiMap();

#ifdef FULL_TEST // Questionable or incompletely implemented tests
  testRecursiveSharedMutex();
#endif
  testVariable(); // Not ended!

  testAllBugs();

  using namespace binom::priv;
  using namespace binom::literals;


  KeyValue test_a = 10, test_b = 10_ui64;
  bool value = test_a == test_b;
  std::clog << "test_a == test_b" << (test_a == test_b);

  std::set<index::Field, index::MapComparator> test /*{
    {WeakLink(), 1, "value"},
    {WeakLink(), 2, "value"},
    {WeakLink(), 3, "value"}
  }*/;

  test.emplace(WeakLink(), 1, "value 1");
  test.emplace(WeakLink(), 2, "value 2");
  test.emplace(WeakLink(), 3, "value 3");

  for(auto& entry : test) {
    std::clog << "key: " << i32(entry.getKey().toNumber()) << '\n';
  }



}
