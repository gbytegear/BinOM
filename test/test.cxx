// TEST FLAGS:
//#define FULL_TEST
//#define TEST_FULL_INFO

#include "test/all_test.hxx"

#include "libbinom/include/binom_impl/ram_storage_implementation/table_impl.hxx"

int main() {
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

  Variable var = "Hello world";
  KeyValue key_val = "Hello world";

  TableImplementation table;

  table.initTable({{1_ui64, IndexType::unique_index}, {2_i64, IndexType::multi_index}});

}
