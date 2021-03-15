#ifndef QUERY_H
#define QUERY_H

#include "variables/variable.h"
#include "path.h"

namespace binom {

enum class QueryProperty : ui8 {
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

enum class QueryFieldValueType : ui8 {
  type,
  type_class,
  value_type,
  number,
  range,
  string
};

struct Range {
  i64 from;
  i64 to;
};

class QueryField {

  QueryProperty property;
  Path path;

  QueryOperator operat;

  QueryFieldValueType value_type;
  union QueryFieldValue {
    VarType type;
    VarTypeClass type_class;
    ValType value_type;
    i64 number;
    Range range;
    BufferArray string;

    QueryFieldValue(VarType type) : type(type) {}
    QueryFieldValue(VarTypeClass type_class) : type_class(type_class) {}
    QueryFieldValue(ValType value_type) : value_type(value_type) {}
    QueryFieldValue(i64 number) : number(number) {}
    QueryFieldValue(Range range) : range(range) {}
    QueryFieldValue(BufferArray string) : string(std::move(string)) {}
    QueryFieldValue(QueryFieldValueType vtype, QueryFieldValue&& other) {
      switch (vtype) {
        case QueryFieldValueType::type:
          type = other.type; return;
        case QueryFieldValueType::type_class:
          type_class = other.type_class; return;
        case QueryFieldValueType::value_type:
          value_type = other.value_type; return;
        case QueryFieldValueType::number:
          number = other.number; return;
        case QueryFieldValueType::range:
          range = other.range; return;
        case QueryFieldValueType::string:
          string = other.string; return;
      }
    }
  };

public:
//  QueryField(QueryProperty property,
//             QueryOperator operat,
//             VarType type)
//    : property(property),
//      operat(operat),
//      value_type()
//  {}
};

typedef QueryField Query;

typedef QueryProperty qprop;
typedef QueryOperator qoper;
typedef QueryNextFieldRelation qrel;

}


#endif // QUERY_H
