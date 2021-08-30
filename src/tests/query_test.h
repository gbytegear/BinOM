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

void ram_structure() {
  Variable var = varr {
      vobj {}
  };
  NodeVisitor root = var;


  std::clog << "Finded:\n";
  for(std::unique_ptr<NodeVisitorBase>& node
      : root.findAll({
  {QProp::type_class, {"content"}, QOper::equal, VarTypeClass::buffer_array}
     })) {
    std::clog << node->toRAMVisitor().getVariable() << '\n';
  }

}

void query_test() {
  ram_structure();
}

#endif // QUERY_TEST_H
