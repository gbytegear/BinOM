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
            {"Name", "Someone"},
            {"Last name", "Unknown"}
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

    GRP_POP

    TEST_ANNOUNCE(Iterator template) GRP_PUSH

    GRP_POP

    TEST_ANNOUNCE(Query test) GRP_PUSH
    using namespace binom::conditions;
    ConditionQuery q;
    q,
        cexp("field1") == 10,
        cexp("field2") != 10, rel::OR,
        cexp("field1") == 20,
        cexp("field2") != 20;
    for(auto& express : q) {
      std::cout << std::string(express.getColumnName())
                << ([express]{
        switch(express.getOperator()) {
          case cexp::Operator::equal: return " == ";
          case cexp::Operator::not_equal: return " != ";
          case cexp::Operator::higher: return " > ";
          case cexp::Operator::higher_or_equal: return " >= ";
          case cexp::Operator::lower: return " < ";
          case cexp::Operator::lower_or_equal: return " <= ";
          case cexp::Operator::subexpression: return " () ";
        }
      })()      << std::string(express.getValue())
                << ([express]{
        switch(express.getNextRelation()) {
          case Relation::AND: return " && ";
          case Relation::OR: return " || ";
        }
      })();
    }
    std::cout << std::endl;
    GRP_POP
  GRP_POP
}
