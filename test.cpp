#include "binom/includes/binom.h"

using namespace binom;

DataBaseContainer db("test_db.binomdb");

void testVariable() {
  std::clog <<
  "========================== Variable Test ================================\n";
//  Variable local = obj{
//  {"usr", varr{
//    obj{
//      {"id", 0_ui64},
//      {"login", "admin"},
//      {"password", "admin"},
//      {"access_lvl", 0xff_ui8},
//      {"props", obj{
//        {"db_access", 0xff_ui8},
//        {"server_cli_access", 0xff_ui8}
//      }}
//    },
//    obj{
//      {"id", 0_ui64},
//      {"login", "guest"},
//      {"password", "guest"},
//      {"access_lvl", 0x00_ui8},
//      {"props", obj{}}
//    },
//  }},
//  {"grp", varr{
//    obj{
//      {"id", 0_ui64},
//      {"name", "system"},
//      {"owner", 0_ui64},
//      {"members", ui64arr{0_ui64}},
//      {"access_lvl", 0xff_ui8},
//      {"props", obj{
//        {"db_access", 0xff_ui8},
//        {"server_cli_access", 0xff_ui8}
//      }}
//    },
//    obj{
//      {"id", 0_ui64},
//      {"name", "unlogined"},
//      {"owner", 0_ui64},
//      {"members", ui64arr{1_ui64}},
//      {"access_lvl", 0x00_ui8 },
//      {"props", obj{}}
//    }
//  }}
//  };


//  ByteArray data = local.serialize();

//  std::clog << "Serialized data size: " << data.length() << " byte\n";

//  Variable local_2 = Variable::deserialize(data);

//  std::clog << local_2 << '\n';

  Variable p = 1_ui64;
  Variable ba = "Hello World"_vbfr;
  Variable arr = varr{1_ui8, 2_ui16, 3_ui32, 4_ui64};
  Variable ob = obj {
    {"first", 1_ui8},
    {"second", 2_ui16},
    {"third", 3_ui32},
    {"fourth", 4_ui64}
  };

  ob.toObject().insert("Hello", 5_ui64);

  ByteArray data = p.serialize();
  p = Variable::deserialize(data);

  std::clog << "P s/d!\n";

  ByteArray data_1 = ba.serialize();
  ba = Variable::deserialize(data_1);

  std::clog << "BA s/d!\n";

  ByteArray data_2 = arr.serialize();
  arr = Variable::deserialize(data_2);

  std::clog << "Arr s/d!\n";

  ByteArray data_3 = ob.serialize();
  std::clog << "Obj s!\n";
  ob = Variable::deserialize(data_3);

  std::clog << "Obj s/d!\n";



}


void testDB() {
  std::clog <<
  "========================= DataBase test =================================\n";

//// Windows MinGw fread bug!!!
//  FileVirtualMemoryController memory("test_db.binomdb");

//  ByteArray data(4096*8);
//  {
//    ui64 i = 0;
//    for(ui64* it = data.begin<ui64>(); it != data.end<ui64>(); (++it, ++i))
//      *it = i;
//  }
//  f_virtual_index node_index = memory.createNode(VarType::array, data);

//  for(ui64 i = 0; i < 4096; ++i) {
//    ByteArray loaded_data(memory.loadHeapDataPart(node_index, i*sizeof(ui64), sizeof(ui64)));
//    std::clog << "Loaded number: " << std::dec << loaded_data.get<ui64>(0) << '\n';
//  }

  Variable local = obj{
  {"usr", varr{
    obj{
      {"id", 0_ui64},
      {"login", "admin"},
      {"password", "admin"},
      {"access_lvl", 0xff_ui8},
      {"props", obj{
        {"db_access", 0xff_ui8},
        {"server_cli_access", 0xff_ui8}
      }}
    },
    obj{
      {"id", 0_ui64},
      {"login", "guest"},
      {"password", "guest"},
      {"access_lvl", 0x00_ui8},
      {"props", obj{}}
    },
  }},
  {"grp", varr{
    obj{
      {"id", 0_ui64},
      {"name", "system"},
      {"owner", 0_ui64},
      {"members", ui64arr{0_ui64}},
      {"access_lvl", 0xff_ui8},
      {"props", obj{
        {"db_access", 0xff_ui8},
        {"server_cli_access", 0xff_ui8}
      }}
    },
    obj{
      {"id", 0_ui64},
      {"name", "unlogined"},
      {"owner", 0_ui64},
      {"members", ui64arr{1_ui64}},
      {"access_lvl", 0x00_ui8 },
      {"props", obj{}}
    }
  }}
  };

  DBNodeVisitor db_visitor(db.getRoot());
  NodeVisitor local_visitor(&local);

  db_visitor.setVariable(local);

//  std::clog << "Loaded form database variable: \n"
//            << db_visitor[{"usr", 0, "login", 1}].getVariable() << "\n\n";

  std::clog << "DB OutPut:\n";
  for(const DBNodeVisitor &node : db_visitor[{"usr", 0, "password"}]) {
    std::clog << node.getVariable() << "\n\n";
  }

  std::clog << "DB Info:\n" << db.getDBInfo() << '\n';


  std::clog << "DB test ended!\n";

}





void testQuery() {
  std::clog <<
  "============================== Query Test ===============================\n";

  Variable local = obj{
  {"usr", varr{
    obj{
      {"id", 0_ui64},
      {"login", "admin"},
      {"password", "admin"},
      {"access_lvl", 0xff_ui8},
      {"props", obj{
        {"db_access", 0xff_ui8},
        {"server_cli_access", 0xff_ui8}
      }}
    },
    obj{
      {"id", 1_ui64},
      {"login", "guest"},
      {"password", "guest"},
      {"access_lvl", 0x00_ui8},
      {"props", obj{}}
    },
    obj{
      {"id", 2_ui64},
      {"login", "old2ev"},
      {"password", "some_strong_password"},
      {"access_lvl", 0xff_ui8},
      {"props", obj{}},
      {"file_storage", obj{
        {"index.html", "<h1>Hello world!</h1>"}
      }}
    }
  }},
  {"grp", varr{
    obj{
      {"id", 0_ui64},
      {"name", "system"},
      {"owner", 0_ui64},
      {"members", ui64arr{0_ui64}},
      {"access_lvl", 0xff_ui8},
      {"props", obj{
        {"db_access", 0xff_ui8},
        {"server_cli_access", 0xff_ui8}
      }}
    },
    obj{
      {"id", 1_ui64},
      {"name", "unlogined"},
      {"owner", 0_ui64},
      {"members", ui64arr{1_ui64}},
      {"access_lvl", 0x00_ui8 },
      {"props", obj{}}
    }
  }}
  };

  Query q{

    {
      {
        {qprop::value, {"id"}, qoper::equal, 0_i64, qrel::OR},
        {qprop::value, {"id"}, qoper::equal, 1_i64, qrel::OR},
//        {qprop::value, {"login"}, qoper::equal, "guest"_vbfr}, // qrel::AND by default
      }
      // qrel::OR by default
    },

  };

  NodeVisitor node(&local);

  node.stepInside("usr"_vbfr);

  NodeVector node_vector = node.findAll(q);

  std::clog << "Local Query result: \n";
  for(const NodeVisitor& node : node_vector) {
    std::clog << node.getVariable() << "\n\n";
  }

  DBNodeVisitor db_node(db.getRoot());

  db_node.stepInside("usr"_vbfr);

  DBNodeVector db_node_vector = db_node.findAll(q);

  std::clog << "DB Query result: \n";
  for(const DBNodeVisitor& node : db_node_vector) {
    std::clog << node.getVariable() << "\n\n";
  }


  std::clog << "Query test ended!\n";
}


int main() {
  try {

    testVariable();
//    testDB();
//    testQuery();

//    DBNodeVisitor node(db.getRoot());

//    std::clog << node.getVariable() << '\n';


    std::clog <<
    "=========================================================================\n";

  } catch(SException& except) {
    std::cerr << SException::ectos(except.code()) << except.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception!" << std::endl;
  }

  return 0;
}
