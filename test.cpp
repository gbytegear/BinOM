#include "binom/includes/binom.h"

using namespace binom;

void testDB() {

  {
    FileIO file("db.binomdb");
    file.resize(0);
  }

  DataBaseContainer db("db.binomdb");

  if(db.isUninitializedRoot()) {
    DBNodeVisitor root_node(db.getRoot());
    f_virtual_index users_node = 0;
    f_virtual_index message_node = 0;

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

    ;


    Query q = {{QProp::value, {"login"}, QOper::equal, "admin"_vbfr }};
    for(Query::QueryEpression& exp : q) {
      std::clog << "Hello\n";
    }


    db.getByNodeIndex(message_node).pushBack(

          vobj{
            {"from", db.getByNodeIndex(users_node).find({{QProp::value, {"login"}, QOper::equal, "admin"_vbfr }}).getNodeIndex()},
            {"to", db.getByNodeIndex(users_node).find({{QProp::value, {"login"}, QOper::equal, "guest"_vbfr }}).getNodeIndex()},
            {"content", "You are entered in system with Guest account, please login with your account"}
          }

          );

  }


  std::clog << "DB:\n" << db.getRoot().getVariable() << '\n';

  BinOMFile binom_file("db.binom");

  Variable data;
  binom_file
      .write(db.getRoot().getVariable())
      .load(data);

  std::clog << "DataBase has been backuped:\n"
            << data << '\n';
}

int main() {
  try {

    testDB();

    std::clog <<
    "=========================================================================\n";

  } catch(Exception& except) {
    std::cerr << except.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception!" << std::endl;
  }

  return 0;
}
