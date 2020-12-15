#ifndef NODE_VISITOR_H
#define NODE_VISITOR_H

#include "variable.h"

namespace binom {

class NodeVisitor {
  union Data {
    Variable* variable;
    NamedVariable* named_variable;
    Value value;
//    MatrixRow matrix_row;
//    TableRow table_row;
  } data;
};

}

#endif
