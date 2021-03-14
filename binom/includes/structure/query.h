#ifndef QUERY_H
#define QUERY_H

#include "variables/variable.h"
#include "path.h"

namespace binom {

enum class QueryProp : ui8 {
  type,
  type_class,
  value_type,
  element_count,
  index,
  name,
  value,

  path
};

enum class QueryOperator : ui8 {
  equal,
  not_equal,
  highter,
  highte_equal,
  lower,
  lower_equal,
  in_range,
  out_range,
  reg_test,

  contains
};

enum class QueryNextFieldRelation : ui8 {
  AND,
  OR,
  XOR
};


typedef QueryProp qprop;
typedef QueryOperator qoper;
typedef QueryNextFieldRelation qrel;

struct Range {
  i64 from;
  i64 to;
};

union QueryValue {
  VarType type;
  VarTypeClass type_class;
  ValType value_type;
  i64 number;
  Range range;
  BufferArray str;
};

class QueryField {

  QueryValue value;
  PathNode* path = nullptr;
  QueryOperator oper;
  QueryProp prop;
  QueryNextFieldRelation next_relation = qrel::AND;

public:
  QueryField();
};

}


#endif // QUERY_H
