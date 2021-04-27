#include "binom/includes/binom.h"

const char SEPARATOR[] = "=========================================================================\n\n\n\n";

using namespace binom;

void testDB() {
  std::clog << "FUNCTION: " << __FUNCTION__ << "()\n";

  BinOMDataBase db("db.binomdb");

  f_virtual_index users_node = 0;
  f_virtual_index message_node = 0;
  DBNodeVisitor root_node(db.getRoot());
  if(db.isUninitializedRoot()) {

    root_node.setVariable(

          vobj{
            {"user", varr{}},
            {"message", varr{}}
          }

          );

    users_node = root_node["user"].getNodeIndex();
    message_node = root_node["message"].getNodeIndex();

    db.getByNodeIndex(users_node).pushBack(

          vobj{
            {"login", "admin"},
            {"password", "admin"},
            {"access_lvl", 0xFF_ui8}
          }

          );

    db.getByNodeIndex(users_node).pushBack(

          vobj{
            {"login", "guest"},
            {"password", "guest"},
            {"access_lvl", 0x00_ui8}
          }

          );

  } else {
    users_node = root_node["user"].getNodeIndex();
    message_node = root_node["message"].getNodeIndex();
  }

  for(ui16 i = 0 ; i < 5000; ++i)
    db.getByNodeIndex(message_node).pushBack(

          vobj{
            {"from", db.getByNodeIndex(users_node).find({{QProp::value, {"login"}, QOper::equal, "admin"_vbfr }}).getNodeIndex()},
            {"to", db.getByNodeIndex(users_node).find({{QProp::value, {"login"}, QOper::equal, "guest"_vbfr }}).getNodeIndex()},
            {"content", "You are entered in system with Guest account, please login with your account"}
          }

          );
  std::clog << "Message count: " << db.getByNodeIndex(message_node).getElementCount() << " objs\n";

//  std::clog << "DB:\n" << db.getRoot().getVariable() << '\n';

  BinOMFile binom_file("db.binom");
  binom_file.write(db);
  Variable data;
  binom_file.load(data);

  std::clog << SEPARATOR;

  std::clog << "DataBase has been backuped\n";
//            << data << '\n';

//  std::clog << SEPARATOR;

}

int main() {
  try {

    testDB();
//    testQuery();

  } catch(Exception& except) {
    std::cerr << except.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception!" << std::endl;
  }

  return 0;
}
