#ifndef TABLE_TEST_HXX
#define TABLE_TEST_HXX

#include "libbinom/include/variables/table.hxx"
#include "libbinom/include/variables/stdoutput.hxx"
#include "tester.hxx"

void testTable() {
  RAIIPerfomanceTest test_perf("Table test: ");
  SEPARATOR
  TEST_ANNOUNCE(Table test)
  GRP_PUSH

  using namespace binom;
  using namespace binom::conditions;
  using namespace binom::literals;

  Table users_1 = table{
    { // Table header!
      {"ID", IndexType::unique_index},
      {"Name", IndexType::multi_index},
      {"Last name", IndexType::multi_index},
      {"Email", IndexType::unique_index},
      {"Phone number", IndexType::unique_index}
    },

    { // Rows
      map{
        {"ID", 0_ui64},
        {"Name", "First"},
        {"Last name", "User"},
        {"Email", "first@example.org"},
        {"Phone number", "+0(000)00-00-00"}
      },
      map{
        {"ID", 2_ui64},
        {"Name", "Third"},
        {"Last name", "User"},
        {"Email", "third@example.org"},
        {"Phone number", "+0(000)00-00-02"}
      },
      map{
        {"ID", 3_ui64},
        {"Name", "Fourth"},
        {"Last name", "User"},
        {"Email", "fourt@example.org"},
        {"Phone number", "+0(000)00-00-03"}
      }
    },

    { // Constraits
      { // Contains check
        cexp{"ID"},
        cexp{"Name"},
        cexp{"Last name"},
        cexp_list{
          cexp{"Email", rel::OR},
          cexp{"Phone number"}
        }
      }
    }
  };

  LOG(WHITE_TXT "Print table")
  std::cout << users_1;
//  for(auto field_ref : *users_1["ID"])
//    utils::printVariable(field_ref);

  LOG(WHITE_TXT "Find \"ID\" = 2_ui64")
  auto answer = users_1.findAndMove({cexp{"ID", op::equal, 2_ui64}});
  for(auto field_ref : *answer["ID"])
    utils::printVariable(field_ref);

  Table users_2 = table{
    { // Table header!
      {"ID", IndexType::unique_index},
      {"Name", IndexType::multi_index},
      {"Last name", IndexType::multi_index},
      {"Email", IndexType::unique_index},
      {"Phone number", IndexType::unique_index}
    },

    { // Rows
      map{
        {"ID", 4_ui64},
        {"Name", "Fivth"},
        {"Last name", "User"},
        {"Email", "fivth@example.org"},
        {"Phone number", "+0(000)00-00-05"}
      },
      map{
        {"ID", 1_ui64},
        {"Name", "Second"},
        {"Last name", "User"},
        {"Email", "second@example.org"},
        {"Phone number", "+0(000)00-00-01"}
      }
    },

    { // Constraits
      { // Contains check
        cexp{"ID"},
        cexp{"Name"},
        cexp{"Last name"},
        cexp_list{
          cexp{"Email", rel::OR},
          cexp{"Phone number"}
        }
      }
    }
  };

  GRP_POP
}

#endif // TABLE_TEST_HXX
