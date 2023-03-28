// TEST FLAGS:
//#define FULL_TEST
//#define TEST_FULL_INFO

#include "test/all_test.hxx"

#include "libbinom/include/binom_impl/ram_storage_implementation/table_impl.hxx"
#include "libbinom/include/binom_impl/query.hxx"

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

  TEST_ANNOUNCE(InDev test) GRP_PUSH
    TEST_ANNOUNCE(Table impl test) GRP_PUSH
      using namespace binom::priv;
      using namespace binom::literals;


      TableImplementation tbl (
            table{
              {
                {"ID", IndexType::unique_index},
                {"Name", IndexType::multi_index},
                {"Last name", IndexType::multi_index},
                {"Organization", IndexType::multi_index},
                {"Is admin", IndexType::multi_index}
              }, {
                multimap{
                  {"ID", 0},
                  {"Name", "Maksim"},
                  {"Last name", "Shemendyuk"},
                  {"Organization", "Grebci i galeri"},
                  {"Is admin", true}
                },
                multimap{
                  {"ID", 1},
                  {"Name", "Someone"},
                  {"Last name", "Unknown"},
                  {"Organization", "Grebci i galeri"},
                  {"Is admin", false}
                },
                multimap{
                  {"ID", 2},
                  {"Name", "Ivan"},
                  {"Last name", "Ivanov"},
                  {"Organization", "Grebci i galeri"},
                  {"Is admin", true}
                },
                multimap{
                  {"ID", 3},
                  {"Name", "Dmitry"},
                  {"Last name", "Igorevich"},
                  {"Organization", "Roga i copita"},
                  {"Is admin", true}
                },
                multimap{
                  {"ID", 4},
                  {"Name", "Alexey"},
                  {"Last name", "Petrovich"},
                  {"Organization", "Roga i copita"},
                  {"Is admin", false}
                },
                multimap{
                  {"ID", 5},
                  {"Name", "Vladimir"},
                  {"Last name", "Vladimirovich"},
                  {"Organization", "Roga i copita"},
                  {"Is admin", false}
                },
                multimap{
                  {"ID", 6},
                  {"Name", "Bill"},
                  {"Last name", "Heits"},
                  {"Organization", "Roga i copita"},
                  {"Is admin", false}
                },
                multimap{
                  {"ID", 7},
                  {"Name", "Linus"},
                  {"Last name", "Torvalds"},
                  {"Organization", "Roga i copita"},
                  {"Is admin", false}
                },
                multimap{
                  {"ID", 8},
                  {"Name", "Ivan"},
                  {"Last name", "Ivanov"},
                  {"Organization", "Roga i copita"},
                  {"Is admin", false}
                }
              }
            });

      utils::printVariable(tbl.getRow("Name", "Maksim"));

      tbl["ID"].ifNoError([](TableImplementation::Column& column) {
        LOG("Column finded")
        utils::printVariable(column.getFirstMapByKey(1));
      }).ifError([](Error err) {
        LOG("Column doesn't finded:" << err.what())
      });

      {
        using namespace binom::conditions;
        TEST_ANNOUNCE(Search by query - Organization = Grebci i galeri && Is admin = true || Organization = Roga i copita && Is admin = true)
        GRP_PUSH
        auto new_tbl = tbl.find(
              ConditionQuery(cexp_list{
                cexp{"Organization", op::equal, "Grebci i galeri"},
                cexp{"Is admin", op::equal, true, rel::OR},
                cexp{"Organization", op::equal, "Roga i copita"},
                cexp{"Is admin", op::equal, false, rel::OR},
              }));

        for(auto field_ref : *new_tbl["ID"])
          utils::printVariable(field_ref.getOwner());
        GRP_POP
      }

      {
        using namespace binom::conditions;
        TEST_ANNOUNCE(Remove by query)
        TableImplementation tbl_copy = tbl;
        LOG("Table before remove")
        for(auto field_ref : *tbl_copy["ID"])
          utils::printVariable(field_ref.getOwner());
        tbl_copy.remove(ConditionQuery{cexp{"Is admin", op::equal, true}});
        LOG("Table after remov by query - Is admin = true")
        for(auto field_ref : *tbl_copy["ID"])
          utils::printVariable(field_ref.getOwner());
      }

    GRP_POP

    TEST_ANNOUNCE(Iterator template) GRP_PUSH

    GRP_POP

  GRP_POP
}
