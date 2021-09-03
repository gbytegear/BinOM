#ifndef QUERY_TEST_H
#define QUERY_TEST_H

#include "binom/includes/binom.h"

using namespace binom;

/*
{ // Query Initer

  {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // Expression lvl#0

  {{ // Expression lvl#0
    {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // SubExpression lvl#1
    {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // SubExpression lvl#1
    {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // SubExpression lvl#1

    {{ // SubExpression lvl#1
      {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // SubExpression lvl#2
      {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // SubExpression lvl#2
      {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // SubExpression lvl#2
    }, QRel::...}

  }, QRel::...}
}

// Data model
[flag][path][value]
[flag][value:
  [size]
  [flag][path][value]
  [flag][path][value]
  [flag][path][value]
  [flag][value:
    [size]
    [flag][path][value]
    [flag][path][value]
    [flag][path][value]
  ]
]

*/

void query_test() {
  Variable var = vobj {
    {"usr", varr{
      vobj{
        {"login", "root"},
        {"password", "root"},
        {"access_lvl", 0xFF_ui8},
        {"user_data", vobj{}},
        {"attributes", varr{"full_access", "unchangeable"}}
      },
      vobj{
        {"login", "guest"},
        {"password", "guest"},
        {"access_lvl", 0x00_ui8},
        {"user_data", vobj{}},
        {"attributes", varr{"ro_access", "unchangeable"}}
      },
      vobj{
        {"login", "admin"},
        {"password", "admin"},
        {"access_lvl", 0x7f_ui8},
        {"user_data", vobj{}},
        {"attributes", varr{"unchangeable"}}
      }
    }},
    {"grp", varr{
      vobj{
        {"name", "system"},
        {"attributes", varr{"unremovable"}},
        {"users", varr{"root", "guest", "admin"}}
      }
    }}
  };

  FileStorage storage("query_test.binomdb", var, true);

  std::clog << "Structure RAM:\n" << var << '\n';

  NodeVisitor root = var;
  FileNodeVisitor froot = storage;

  std::clog << "Structure File:\n" << froot.getVariable() << '\n';

  Query query = {
    {QProp::value, {"access_lvl"}, QOper::highter_equal, 0x00}
  };

  std::clog << "Finded in RAM container:\n";
  for(std::unique_ptr<NodeVisitorBase>& node : root["usr"].findAll(query)) {
    std::clog << node->toRAMVisitor().getVariable() << "\n\n";
  }

  std::clog << "\nFinded in file container:\n";
  for(std::unique_ptr<NodeVisitorBase>& node : froot["usr"].findAll(query)) {
    std::clog << node->toFileVisitor().getVariable() << "\n\n";
  }

}

#endif // QUERY_TEST_H
