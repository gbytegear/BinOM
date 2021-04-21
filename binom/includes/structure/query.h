//#define NEW_QUERY
#ifdef NEW_QUERY

#ifndef QUERY_H
#define QUERY_H

#include "variables/variable.h"
#include "path.h"

namespace binom {

enum class QProp : ui8 {
  type,
  type_class,
  value_type,
  element_count,
  index,
  name,
  value,
  sub_exp,
};

enum class QOper : ui8 {
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

enum class QRel : ui8 {
  AND,
  OR,
  XOR
};

enum class QValType : ui8 {
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

  bool in(i64 number) const {return number >= from && number <= to;}
  bool out(i64 number) const {return number < from || number > to;}
};

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

class Query {
  struct Flag;
  struct QValue;
public:
  class QueryLiteral;
  class QueryEpression;
  class iterator;

private:
  typedef std::initializer_list<QueryLiteral> QExprInitList;
  ByteArray data;

  static ByteArray buildSubexpression(QExprInitList& exprs);

public:
  Query(QExprInitList exprs);
  iterator begin();
  iterator end();
};

struct Query::Flag {
  // Octet #1
  QProp prop : 3;
  QOper oper : 4;
  bool has_path : 1;

  // Octet #2
  QValType val_type : 3;
  QRel next_rel : 2;
};

struct Query::QValue {
  QValType value_type;
  union Data {
    VarType type;
    VarTypeClass type_class;
    ValType value_type;
    i64 number;
    Range range;
    BufferArray string; // or subexpression

    ~Data();
    Data(VarType type);
    Data(VarTypeClass type_class);
    Data(ValType value_type);
    Data(i64 number);
    Data(Range range);
    Data(BufferArray string);
  } data;

  QValue(VarType type);
  QValue(VarTypeClass type_class);
  QValue(ValType value_type);
  QValue(i64 number);
  QValue(Range range);
  QValue(BufferArray string);
  QValue(QValue&& other);
  ~QValue();
};

class Query::QueryLiteral {
  Flag flag;
  Path path;
  QValue value;
  friend ByteArray Query::buildSubexpression(Query::QExprInitList& exprs);
public:
  QueryLiteral(QProp prop, Path path, QOper oper, QValue val, QRel next_rel = QRel::AND);
  QueryLiteral(QProp prop, QOper oper, QValue val, QRel next_rel = QRel::AND);
  QueryLiteral(QExprInitList subexprs, QRel next_rel = QRel::AND);
  QueryLiteral(const QueryLiteral&) = delete;
  QueryLiteral(QueryLiteral&&) = delete;
};

class Query::iterator {
  ByteArray::iterator it;
public:
  iterator(ByteArray::iterator it);
  iterator(iterator& other);
  iterator(iterator&& other);

  QueryEpression& operator*();
  QueryEpression* operator->();

  iterator& operator++();

  iterator operator++(int);

  bool operator==(iterator other) const;
  bool operator!=(iterator other) const;
  bool operator>(iterator other) const;
  bool operator>=(iterator other) const;
  bool operator<(iterator other) const;
  bool operator<=(iterator other) const;
};

class Query::QueryEpression {
  ByteArray::iterator it;

  Flag& getFlag();
  byte* getDataPointer();
  byte* getPathPointer();

public:

  // Flag getters
  QProp getProp();
  QOper getOper();
  bool hasPath();
  QValType getQValType();
  QRel getNextRel();

  Path getPath();

  // Value getters
  VarType getVarType();
  VarTypeClass getVarTypeClass();
  ValType getValType();
  i64 getNumber();
  Range getRange();
  BufferArray getString();

  iterator begin();

  iterator end();
};

}

#endif
#else


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
  node_index,
  name,
  value,
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

  bool in(i64 number) const {return number >= from && number <= to;}
  bool out(i64 number) const {return number < from || number > to;}
};









struct QueryFieldValue {
  QueryFieldValueType value_type;
  union Data {
    VarType type;
    VarTypeClass type_class;
    ValType value_type;
    i64 number;
    Range range;
    BufferArray string;

    ~Data();
    Data(VarType type);
    Data(VarTypeClass type_class);
    Data(ValType value_type);
    Data(i64 number);
    Data(Range range);
    Data(BufferArray string);
    Data(QueryFieldValueType vtype, const Data& other);
    Data(QueryFieldValueType vtype, Data&& other);

  } data;

  QueryFieldValue(VarType type);
  QueryFieldValue(VarTypeClass type_class);
  QueryFieldValue(ValType value_type);
  QueryFieldValue(i64 number);
  QueryFieldValue(Range range);
  QueryFieldValue(BufferArray string);
  QueryFieldValue(const QueryFieldValue& other);
  QueryFieldValue(QueryFieldValue&& other);
  ~QueryFieldValue();
};










class QueryField {

  QueryFieldValue value;
  Path* path = nullptr;
  QueryField* next = nullptr;

  QueryProperty property;
  QueryOperator operat;
  QueryNextFieldRelation next_rel = QueryNextFieldRelation::AND;

  bool test();

  friend class QueryFieldGroup;

public:

  class iterator {
    QueryField* current;
  public:
    iterator(QueryField* field);
    iterator(const iterator& other);
    iterator(iterator&& other);

    QueryField& operator*() const;
    QueryField* operator->() const;

    iterator& operator++();
    iterator operator++(int);

    bool operator==(iterator other) const;
    bool operator!=(iterator other) const;
  };

  QueryField(
      QueryProperty property,
      QueryOperator operat,
      QueryFieldValue value,
      QueryNextFieldRelation next_rel = QueryNextFieldRelation::AND
                                        );

  QueryField(
      QueryProperty property,
      Path path,
      QueryOperator operat,
      QueryFieldValue value,
      QueryNextFieldRelation next_rel = QueryNextFieldRelation::AND
                                        );

  QueryField(const QueryField& other);
  QueryField(QueryField&& other);
  //  QueryField(std::initializer_list<QueryField> field_list);
  ~QueryField();

  //   Getters

  // Get operation properties
  QueryProperty getPropertyType() const;
  bool isCurrentNodeTest() const;
  Path& getPath() const;
  QueryOperator getOperatorType() const;
  QueryNextFieldRelation getNextRealation() const;
  QueryFieldValueType getValueType() const;

  // Get Value
  VarType getVarType() const;
  VarTypeClass getVarTypeClass() const;
  ValType getValType() const;
  i64 getNumber() const;
  ui64 getUNumber() const;
  Range getRange() const;
  BufferArray getString() const;

  iterator begin();
  iterator end();
};


class QueryFieldGroup {
  QueryField fields;
  QueryFieldGroup* next = nullptr;
  QueryNextFieldRelation next_relation;

public:
  QueryFieldGroup(std::initializer_list<QueryField> field_list,
                  QueryNextFieldRelation next_relation = QueryNextFieldRelation::OR);

  QueryFieldGroup(std::initializer_list<QueryFieldGroup> field_group_list);

  QueryFieldGroup(const QueryFieldGroup& other);

  QueryFieldGroup(QueryFieldGroup&& other);

  ~QueryFieldGroup();

  QueryField& getFields();
  QueryNextFieldRelation getNextRealation();

  class iterator {
    QueryFieldGroup* current;
  public:
    iterator(QueryFieldGroup* field_group);
    iterator(const iterator& other);
    iterator(iterator&& other);

    QueryFieldGroup& operator*() const;
    QueryFieldGroup* operator->() const;

    iterator& operator++();
    iterator operator++(int);

    bool operator==(iterator other) const;
    bool operator!=(iterator other) const;
  };

  iterator begin();
  iterator end();

};












/////////////////////////////////////////////////////////////////////////

typedef QueryFieldGroup Query;

typedef QueryField qfield;
typedef QueryProperty qprop;
typedef QueryOperator qoper;
typedef QueryNextFieldRelation qrel;
typedef QueryFieldValue qval;

}


#endif // QUERY_H
#endif
