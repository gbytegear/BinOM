#include "binom/includes/binom.h"

const char SEPARATOR[] = "=========================================================================\n";

using namespace binom;

void testDB() {
  std::clog << "FUNCTION: " << __FUNCTION__ << "()\n";

  {
    FileIO file("db.binomdb");
    file.resize(0);
  }

  BinOMDataBase db("db.binomdb");

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

  std::clog << SEPARATOR;

  std::clog << "DataBase has been backuped:\n"
            << data << '\n';

  std::clog << SEPARATOR;

}

void testQuery() {
  std::clog << "FUNCTION: " << __FUNCTION__ << "()\n";
  Variable data = vobj {
    {"usr", varr{
      vobj{
        {"login", "admin"},
        {"password", "admin"},
        {"access_lvl", 0xFF_ui8},
        {"files", vobj{
          {"config.json",
                  "{\n"
                  "  \"login\":\"admin\",\n"
                  "  \"password\":\"admin\",\n"
                  "  \"access_lvl\":255\n"
                  "}"
          }
        }}
      },
      vobj{
        {"login", "any_user"},
        {"password", "any_password"},
        {"access_lvl", 0x7F_ui8},
        {"files", vobj{
          {"any.txt", "Hello, World!"}
        }}
      },
      vobj{
        {"login", "other_user"},
        {"password", "other_password"},
        {"access_lvl", 0x7F_ui8},
        {"files", vobj{}}
      },
    }},
    {"grp", varr{
      vobj{
        {"name", "any_group"},
        {"owner", "any_user"},
        {"members", varr{
          "any_user",
          "other_user"
        }}
      },
      vobj{
        {"name", "other_group"},
        {"owner", "admin"},
        {"members", varr{
          "admin",
          "any_user"
        }}
      }
    }}
  };

  BinOMDataBase db("qtest.binomdb");
  if(db.isUninitializedRoot()) {
    db.getRoot().setVariable(data);
  }

  DBNodeVisitor db_node(db.getRoot());

  std::clog << "Structure:\n" << db_node.getVariable() << '\n';

  std::clog << SEPARATOR;

  std::clog << "Files of all users:\n";
  for(DBNodeVisitor user_node : db_node["usr"].findAll({{QProp::element_count, {"files"}, QOper::highter, 0}}))
    for(DBNodeVisitor file_node : user_node["files"])
      std::clog << file_node.getVariable().toBufferArray().toString() << "\n\n";

  std::clog << SEPARATOR;

  for(DBNodeVisitor group_node : db_node["grp"]) {
    std::clog << "Group \"" << group_node["name"].getVariable().toBufferArray().toString() << "\" members:\n";
    for(DBNodeVisitor user_login_node : group_node["members"]) {
      std::clog << db_node["usr"]
          .find({
                  {QProp::value,
                   {"login"},
                   QOper::equal,
                   user_login_node
                   .getVariable()
                   .toBufferArray()}
                }).getVariable()
          << "\n\n";
    }
  }

  std::clog << SEPARATOR;






  NodeVisitor node(&data);

  std::clog << "Structure:\n" << node.getVariable() << '\n';

  std::clog << SEPARATOR;

  std::clog << "Files of all users:\n";

  node["usr"]
      .findAll({{QProp::element_count, {"files"}, QOper::highter, 0}})
      .foreach([&](NodeVisitor& node) {
    for(NodeVisitor file_node : node["files"])
      std::clog << file_node.getName().toString() << ":\n"
                << file_node.getVariable().toBufferArray().toString() << "\n\n";
  });

  std::clog << SEPARATOR;

  for(NodeVisitor group_node : node["grp"]) {
    std::clog << "Group \"" << group_node["name"].getVariable().toBufferArray().toString() << "\" members:\n";
    for(NodeVisitor user_login_node : group_node["members"]) {
      std::clog << node["usr"]
                   .find({
                           {QProp::value,
                            {"login"},
                            QOper::equal,
                            user_login_node
                              .getVariable()
                              .toBufferArray()}
                         }).getVariable()
          << "\n\n";
    }
  }

  std::clog << SEPARATOR;

}

int main() {
  try {

    testDB();
    testQuery();

  } catch(Exception& except) {
    std::cerr << except.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception!" << std::endl;
  }

  return 0;
}
