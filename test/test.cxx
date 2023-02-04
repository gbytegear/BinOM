// TEST FLAGS:
//#define FULL_TEST
//#define TEST_FULL_INFO

#include "test/all_test.hxx"

#include "libbinom/include/binom_impl/ram_storage_implementation/table_impl.hxx"

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
  testVariable(); // Not ended!
#endif

  testAllBugs();

  TEST_ANNOUNCE(InDev test)
  GRP_PUSH

  using namespace binom::priv;
  using namespace binom::literals;


  TableImplementation tbl (table{
    {
      {"ID", IndexType::unique_index},
      {"Name", IndexType::multi_index},
      {"Last name", IndexType::multi_index},
    }, {
      multimap{
        {"ID", 0},
        {"Name", "Maksim"},
        {"Last name", "Shemendyuk"}
      },
      multimap{
        {"ID", 1},
        {"Name", "Roma"},
        {"Last name", "Vitkovsiy"}
      }
    }
  });

  utils::printVariable(tbl.getRow("name", "Maksim"));

  GRP_POP
}
