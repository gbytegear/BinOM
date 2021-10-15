//#include "tests/var_test.h"
//#include "tests/heap_blocks_test.h"
//#include "tests/memory_manager_test.h"
//#include "tests/file_node_visitor_test.h"
//#include"tests/multithreading_test.h"
#include "tests/query_test.h"
#include "binom/includes/lexer.h"

int main() {
  using namespace binom;
  try {
    FileIO file("../example/test.json");
    binom::Variable data = lexer << file.read(0, file.getSize()).toStdString();
    std::clog << "JSON deserialized to BinOM:\n" << data << '\n';


    const char struct_[] =
"obj{"
"  usr: arr["
"    obj{"
"      login: \"admin\""
"      password: \"admin\""
"      access_lvl: 0xFF_ui8"
"    },"
"    obj{"
"      login: \"user\""
"      password: \"user\""
"      access_lvl: 0x7F_ui8"
"    },"
"    obj{"
"      login: \"guest\""
"      password: \"guest\""
"      access_lvl: 0x00_ui8"
"    },"
"  ],"
"  grp: arr["
"    obj{"
"      name: 'system'"
"      users: ["
"        'admin'"
"        'guest'"
"      ]"
"    }"
"  ]"
"}";

    Variable var = lexer << struct_;

    std::clog << "Builded variable: " << var;


    std::clog << "=========================================================================\n"
                 "|                    Test completed successfully                        |\n"
                 "=========================================================================\n";
    return 0;

  } catch(Exception& except) {
    std::cerr << except.full() << std::endl;
  } catch(std::exception& except) {
    std::cerr << except.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception!\n";
  }

  std::cerr << "=========================================================================\n"
               "|                   !!! Test ended abnormally !!!                       |\n"
               "=========================================================================\n";

}
