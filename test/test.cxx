// TEST FLAGS:
//#define FULL_TEST
//#define TEST_FULL_INFO

#include "test/all_test.hxx"

//#include <iostream>

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

  {
    std::set<IndexedRowCell, RowCellComparator> indexed_cells;
    std::set<UnindexedRowCell, RowUnindexedCellComparator> unindexed_cells;


  }

  TableImplementation table(
        {{{"Column 1", IndexType::unique_index}, {"Column 2", IndexType::multi_index}}, {
           {
             {"Column 1", "1 1"},
             {"Column 2", "2 1"},
             {"Unindexed", "3 1"}
           },
           {
             {"Column 1", "1 2"},
             {"Column 2", "2 2"},
             {"Unindexed", "3 2"}
           }
         }});

  table.remove("Column 1", "1 2");

}
