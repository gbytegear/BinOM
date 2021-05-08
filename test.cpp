#include "binom/includes/binom.h"

#include <ctime>

const char SEPARATOR[] = "=========================================================================\n\n";

using namespace binom;

void testDB() {
  BinOMDataBase db("test.binomdb",
                   vobj {
                     {"usr", varr{
                        vobj{ // id: 20
                          {"login", "admin"},
                          {"password_digest", "c7ad44cbad762a5da0a452f9e854fdc1e0e7a52a38015f23f3eab1d80b931dd472634dfac71cd34ebc35d16ab7fb8a90c81f975113d6c7538dc69dd8de9077ec"},
                          {"access_lvl", 0xFF_ui8},
                          {"files", vobj{
                             {"config.json",
                              "{"
                              "\"port\":8080,"
                              "\"host\":\"127.0.0.1\","
                              "\"database\":{"
                              "\"port\":8081,"
                              "\"host\":\"127.0.0.1\""
                              "}"
                              "}"}
                           }}
                        },
                        vobj{ // id: 25
                          {"login", "user"},
                          {"password_digest", "b14361404c078ffd549c03db443c3fede2f3e534d73f78f77301ed97d4a436a9fd9db05ee8b325c0ad36438b43fec8510c204fc1c1edb21d0941c00e9e2c1ce2"},
                          {"access_lvl", 0x7F_ui8},
                          {"files", vobj{}}
                        },

                        vobj{ // id: 30
                          {"login", "other_user"},
                          {"password_digest", "8a33b2a626b76a156565802257ad6656a56927d42254c96e5b01bca312e4612f3c95399e7aef22162fdb1b23ca5f043a546666f5bad2fbc694cfa21665aa69f6"},
                          {"access_lvl", 0x7F_ui8},
                          {"files", vobj{}}
                        }
                      }},

                     {"msg", varr{
                        vobj {
                          {"from", 25_ui64},
                          {"to", 30_ui64},
                          {"content", "Hello, friend!"}
                        },

                        vobj {
                          {"from", 30_ui64},
                          {"to", 25_ui64},
                          {"content", "Hi"}
                        },
                      }},

                     {"grp", varr{
                        vobj {
                          {"name", "User Group"},
                          {"owner", 25_ui64},
                          {"members", ui64arr{25, 30}}
                        }
                      }}
                   }
  );

  DBNodeVisitor root_node = db.getRoot();

  std::clog << "Structure in database:\n" << root_node.getVariable() << '\n' << SEPARATOR
            << std::dec
            << "admin id: " << root_node[{"usr",0}].getNodeIndex() << '\n'
            << "user id: " << root_node[{"usr",1}].getNodeIndex() << '\n'
            << "other_user id: " << root_node[{"usr",2}].getNodeIndex() << '\n'
            << SEPARATOR;


  Query q = {
    {QProp::value, {"login"}, QOper::equal, "other_user"_vbfr}
  };

  root_node["usr"].find(q).ifNotNull([](DBNodeVisitor& node){
    std::clog << "Login of finded by query: "
              << node["login"].getVariable().toBufferArray().toString() << '\n';
  }).ifNull([](){
    std::cerr << "No results were found for your search\n";
  });

  std::clog << SEPARATOR;

  q = {
    {QProp::value, {"from"}, QOper::equal, root_node[{"usr",2}].getNodeIndex(), QRel::OR},
    {QProp::value, {"to"}, QOper::equal, root_node[{"usr",2}].getNodeIndex()},
//    {QProp::value, {"content"}, QOper::equal, "Nothing"_vbfr}
  };

  root_node["msg"].findAll(q).foreach([&](DBNodeVisitor& node){
    std::clog << "Message\nfrom: "
    << db.getByNodeIndex(node["from"].getVariable().getValue().asUi64())["login"].getVariable().toBufferArray().toString()
    << "\nto: "
    << db.getByNodeIndex(node["to"].getVariable().getValue().asUi64())["login"].getVariable().toBufferArray().toString()
    << "\ncontent: " << node["content"].getVariable().toBufferArray().toString() << "\n\n";
  }).ifEmpty([](){
    std::cerr << "No results were found for your search\n";
  });


}

int main() {
  try {

    testDB();

    std::clog << "=========================================================================\n"
              << "|                    Test completed successfully                        |\n"
              << "=========================================================================\n";

  } catch(Exception& except) {
    std::cerr << except.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception!" << std::endl;
  }

  return 0;
}
