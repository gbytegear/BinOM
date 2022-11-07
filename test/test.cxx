// TEST FLAGS:
//#define FULL_TEST
//#define TEST_FULL_INFO

#include "test/all_test.hxx"

//#include <iostream>

#include <mutex>
#include "libbinom/include/binom_impl/ram_storage_implementation/table_impl.hxx"

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

  TableImplementation table(
    {{ // Header
       {"Column 1", IndexType::unique_index},
       {"Column 2", IndexType::multi_index}
     },{ // Rows
         {
           {"Column 1", ui64arr{1,1}},
           {"Column 2", ui64arr{2,1}},
           {"Unindexed", ui64arr{3,1}}
         },
         {
           {"Column 1", ui64arr{1,2}},
           {"Column 2", ui64arr{2,2}},
           {"Unindexed", ui64arr{3,2}}
         },
         {
           {"Column 1", ui64arr{1,3}},
           {"Unindexed", ui64arr{3,2}}
         }
    }}
  );

  table.remove("Column 1", ui64arr{1,2});
  table.remove("Column 2", ui64arr{2,1});

}
