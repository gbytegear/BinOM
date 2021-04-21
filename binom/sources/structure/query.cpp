#include "binom/includes/structure/query.h"

using namespace binom;

#ifdef NEW_QUERY

Query::QValue::Data::~Data() {}
Query::QValue::Data::Data(VarType type) : type(type) {}
Query::QValue::Data::Data(VarTypeClass type_class) : type_class(type_class) {}
Query::QValue::Data::Data(ValType value_type) : value_type(value_type) {}
Query::QValue::Data::Data(i64 number) : number(number) {}
Query::QValue::Data::Data(Range range) : range(range) {}
Query::QValue::Data::Data(BufferArray string) : string(std::move(string)) {}





Query::QValue::QValue(VarType type)
  : value_type(QValType::type),
    data(type) {}

Query::QValue::QValue(VarTypeClass type_class)
  : value_type(QValType::type_class),
    data(type_class) {}

Query::QValue::QValue(ValType value_type)
  : value_type(QValType::value_type),
    data(value_type) {}

Query::QValue::QValue(i64 number)
  : value_type(QValType::number),
    data(number) {}

Query::QValue::QValue(Range range)
  : value_type(QValType::range),
    data(range) {}

Query::QValue::QValue(BufferArray string)
  : value_type(QValType::string),
    data(std::move(string)) {}


Query::QValue::~QValue() {
  if(value_type == QValType::string)
    if(data.number != 0)
      data.string.~BufferArray();
}

ByteArray Query::buildSubexpression(Query::QExprInitList& exprs) {
  ByteArray data;
  for(const QueryLiteral& literal : exprs) {
    data.pushBack<Flag>(literal.flag);
    if(literal.flag.has_path) {
      ByteArray path_data = literal.path.toByteArray();
      data.pushBack<ui64>(path_data.length());
      data.pushBack(path_data);
    }
    switch (literal.flag.val_type) {
      case binom::QValType::type:
        data.pushBack<VarType>(literal.value.data.type);
      break;
      case binom::QValType::type_class:
        data.pushBack<VarTypeClass>(literal.value.data.type_class);
      break;
      case binom::QValType::value_type:
        data.pushBack<ValType>(literal.value.data.value_type);
      break;
      case binom::QValType::number:
        data.pushBack<ui64>(literal.value.data.number);
      break;
      case binom::QValType::range:
        data.pushBack<Range>(literal.value.data.range);
      break;
      case binom::QValType::string: {
        data.pushBack(literal.value.data.string.serialize());
      break;}
    }
  }
  return data;
}

Query::Query(Query::QExprInitList exprs)
  : data(buildSubexpression(exprs)) {}

Query::iterator Query::begin() {return data.begin();}
Query::iterator Query::end() {return data.end();}




Query::QueryLiteral::QueryLiteral(QProp prop, Path path, QOper oper, QValue val, QRel next_rel)
  : flag{prop, oper, !path.isEmpty(), val.value_type, next_rel},
    path(path),
    value(std::move(val)) {}

Query::QueryLiteral::QueryLiteral(QProp prop, QOper oper, QValue val, QRel next_rel)
  : flag{prop, oper, false, val.value_type, next_rel},
    path(),
    value(std::move(val)) {}

Query::QueryLiteral::QueryLiteral(QExprInitList subexprs, QRel next_rel)
  : QueryLiteral(QProp::sub_exp, {}, QOper::equal, BufferArray(buildSubexpression(subexprs)), next_rel) {}






Query::iterator::iterator(ByteArray::iterator it) : it(it) {}
Query::iterator::iterator(binom::Query::iterator& other) : it(other.it) {}
Query::iterator::iterator(binom::Query::iterator&& other) : it(other.it) {}

Query::QueryEpression& Query::iterator::operator*() {return *reinterpret_cast<QueryEpression*>(this);}
Query::QueryEpression* Query::iterator::operator->() {return reinterpret_cast<QueryEpression*>(this);}

binom::Query::iterator& Query::iterator::operator++() {
  Flag f = *reinterpret_cast<Flag*>(it);
  it += sizeof (Flag);
  if(f.has_path)
    it += *reinterpret_cast<ui64*>(it) + sizeof(ui64);
  switch (f.val_type) {
    case binom::QValType::type:
      it += sizeof (VarType);
    break;
    case binom::QValType::type_class:
      it += sizeof (VarTypeClass);
    break;
    case binom::QValType::value_type:
      it += sizeof (ValType);
    break;
    case binom::QValType::number:
      it += sizeof (ui64);
    break;
    case binom::QValType::range:
      it += sizeof (Range);
    break;
    case binom::QValType::string:
      it += BufferArray::serializedSize(it);
    break;
  }
  return *this;
}

binom::Query::iterator Query::iterator::operator++(int) {
  iterator last = *this;
  ++(*this);
  return last;
}

bool Query::iterator::operator==(binom::Query::iterator other) const {return it == other.it;}
bool Query::iterator::operator!=(binom::Query::iterator other) const {return it != other.it;}
bool Query::iterator::operator>(binom::Query::iterator other) const {return it > other.it;}
bool Query::iterator::operator>=(binom::Query::iterator other) const {return it >= other.it;}
bool Query::iterator::operator<(binom::Query::iterator other) const {return it < other.it;}
bool Query::iterator::operator<=(binom::Query::iterator other) const {return it <= other.it;}







Query::Flag& Query::QueryEpression::getFlag() {return *reinterpret_cast<Flag*>(it);}

byte* Query::QueryEpression::getDataPointer() {
  return reinterpret_cast<byte*>(it + sizeof(Flag) + (
                                   hasPath()
                                   ? (*reinterpret_cast<ui64*>(it + 1) + sizeof(ui64))
                                   : 0
                                     )
                                 );
}

byte* Query::QueryEpression::getPathPointer() {return reinterpret_cast<byte*>(it + sizeof(Flag));}
QProp Query::QueryEpression::getProp() {return getFlag().prop;}
QOper Query::QueryEpression::getOper() {return getFlag().oper;}
bool Query::QueryEpression::hasPath() {return getFlag().has_path;}
QValType Query::QueryEpression::getQValType() {return getFlag().val_type;}
QRel Query::QueryEpression::getNextRel() {return getFlag().next_rel;}

Path Query::QueryEpression::getPath() {
  ByteArray::iterator _it = getPathPointer();
  return Path::fromByteArray(ByteArray(_it + sizeof(ui64), *reinterpret_cast<ui64*>(_it)));
}

VarType Query::QueryEpression::getVarType() {return *reinterpret_cast<VarType*>(getDataPointer());}
VarTypeClass Query::QueryEpression::getVarTypeClass() {return *reinterpret_cast<VarTypeClass*>(getDataPointer());}
ValType Query::QueryEpression::getValType() {return *reinterpret_cast<ValType*>(getDataPointer());}
i64 Query::QueryEpression::getNumber() {return *reinterpret_cast<i64*>(getDataPointer());}
Range Query::QueryEpression::getRange() {return *reinterpret_cast<Range*>(getDataPointer());}
BufferArray Query::QueryEpression::getString() {ByteArray::iterator it = getDataPointer();return BufferArray::deserialize(it);}

Query::iterator Query::QueryEpression::begin() {
  ByteArray::iterator it = getDataPointer();
  for(; *it > 127; ++it);
  return ++it;
}

Query::iterator Query::QueryEpression::end() {
  ByteArray::iterator it = getDataPointer();
  return it + BufferArray::serializedSize(it);
}

#else

//  QueryFieldValue

QueryFieldValue::QueryFieldValue(VarType type)
  : value_type(QueryFieldValueType::type),
    data(type) {}

QueryFieldValue::QueryFieldValue(VarTypeClass type_class)
  : value_type(QueryFieldValueType::type_class),
    data(type_class) {}

QueryFieldValue::QueryFieldValue(ValType value_type)
  : value_type(QueryFieldValueType::value_type),
    data(value_type) {}

QueryFieldValue::QueryFieldValue(i64 number)
  : value_type(QueryFieldValueType::number),
    data(number) {}

QueryFieldValue::QueryFieldValue(Range range)
  : value_type(QueryFieldValueType::range),
    data(range) {}

QueryFieldValue::QueryFieldValue(BufferArray string)
  : value_type(QueryFieldValueType::string),
    data(std::move(string)) {}

QueryFieldValue::QueryFieldValue(const QueryFieldValue& other)
  : value_type(other.value_type),
    data(other.value_type, other.data) {}

QueryFieldValue::QueryFieldValue(QueryFieldValue&& other)
  : value_type(other.value_type),
    data(other.value_type, other.data) {}

QueryFieldValue::~QueryFieldValue() {
  if(value_type == QueryFieldValueType::string)
    if(data.number != 0)
      data.string.~BufferArray();
}




///////////////////////////////////////////////////////////////////////// QueryFieldValue::Data

QueryFieldValue::Data::~Data() {}

QueryFieldValue::Data::Data(VarType type) : type(type) {}

QueryFieldValue::Data::Data(VarTypeClass type_class) : type_class(type_class) {}

QueryFieldValue::Data::Data(ValType value_type) : value_type(value_type) {}

QueryFieldValue::Data::Data(i64 number) : number(number) {}

QueryFieldValue::Data::Data(Range range) : range(range) {}

QueryFieldValue::Data::Data(BufferArray string) : string(std::move(string)) {}

QueryFieldValue::Data::Data(QueryFieldValueType vtype, const QueryFieldValue::Data& other) {
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
      new(&string) BufferArray(other.string); return;
  }
}

QueryFieldValue::Data::Data(QueryFieldValueType vtype, QueryFieldValue::Data&& other)
  : Data(vtype, const_cast<const Data&>(other)) {}









// QueryField

bool QueryField::test() {
  switch (property) {
    case QueryProperty::type:
      if(value.value_type != QueryFieldValueType::type)
        return false;
      switch (operat) {
        case QueryOperator::equal: case QueryOperator::not_equal:
        return true;
        default: return false;
      }
    case QueryProperty::type_class:
      if(value.value_type != QueryFieldValueType::type_class)
        return false;
      switch (operat) {
        case QueryOperator::equal: case QueryOperator::not_equal:
        return true;
        default: return false;
      }
    case QueryProperty::value_type:
      if(value.value_type != QueryFieldValueType::value_type)
        return false;
      switch (operat) {
        case QueryOperator::equal: case QueryOperator::not_equal:
        return true;
        default: return false;
      }
    case QueryProperty::element_count:
      if(value.value_type != QueryFieldValueType::number &&
         value.value_type != QueryFieldValueType::range)
        return false;
      switch (operat) {
        case QueryOperator::in_range: case QueryOperator::out_range:
        return value.value_type == QueryFieldValueType::range;
        case QueryOperator::equal: case QueryOperator::not_equal:
        case QueryOperator::lower: case QueryOperator::lower_equal:
        case QueryOperator::highter: case QueryOperator::highte_equal:
        return true;
        default: return false;
      }
    case QueryProperty::index:
      if(value.value_type != QueryFieldValueType::number &&
         value.value_type != QueryFieldValueType::range)
        return false;
      switch (operat) {
        case QueryOperator::in_range: case QueryOperator::out_range:
        return value.value_type == QueryFieldValueType::range;
        case QueryOperator::equal: case QueryOperator::not_equal:
        case QueryOperator::lower: case QueryOperator::lower_equal:
        case QueryOperator::highter: case QueryOperator::highte_equal:
        return true;
        default: return false;
      }
    case QueryProperty::node_index:
      if(value.value_type != QueryFieldValueType::number &&
         value.value_type != QueryFieldValueType::range)
        return false;
      switch (operat) {
        case QueryOperator::in_range: case QueryOperator::out_range:
        return value.value_type == QueryFieldValueType::range;
        case QueryOperator::equal: case QueryOperator::not_equal:
        case QueryOperator::lower: case QueryOperator::lower_equal:
        case QueryOperator::highter: case QueryOperator::highte_equal:
        return true;
        default: return false;
      }
    case QueryProperty::name:
      if(value.value_type != QueryFieldValueType::string)
        return false;
      switch (operat) {
        case QueryOperator::equal: case QueryOperator::not_equal:
        case QueryOperator::reg_test:
        return true;
        default: return false;
      }
    case QueryProperty::value:
      if(value.value_type != QueryFieldValueType::number &&
         value.value_type != QueryFieldValueType::string)
        return false;
      switch (operat) {
        case QueryOperator::equal: case QueryOperator::not_equal:
        return true;
        case QueryOperator::lower: case QueryOperator::lower_equal:
        case QueryOperator::highter: case QueryOperator::highte_equal:
        case QueryOperator::in_range: case QueryOperator::out_range:
          return value.value_type != QueryFieldValueType::number;
        case QueryOperator::reg_test:
          return value.value_type != QueryFieldValueType::string;
        default: return false;
      }
  }
  return false;
}

QueryField::QueryField(QueryProperty property, QueryOperator operat, QueryFieldValue value, QueryNextFieldRelation next_rel)
  : value(std::move(value)),
    property(property),
    operat(operat),
    next_rel(next_rel) {if(!test()) throw Exception(ErrCode::binom_query_invalid_field);}

QueryField::QueryField(QueryProperty property, Path path, QueryOperator operat, QueryFieldValue value, QueryNextFieldRelation next_rel)
  : value(std::move(value)),
    path(new Path(std::move(path))),
    property(property),
    operat(operat),
    next_rel(next_rel) {if(!test()) throw Exception(ErrCode::binom_query_invalid_field);}

QueryField::QueryField(const QueryField& other)
  : value(other.value),
    path((other.path)? new Path(*other.path) : nullptr),
    next(other.next? new QueryField(*other.next) : nullptr),
    property(other.property),
    operat(other.operat),
    next_rel(other.next_rel) {}

QueryField::QueryField(QueryField&& other)
  : value(std::move(other.value)),
    path(other.path),
    property(other.property),
    operat(other.operat),
    next_rel(other.next_rel) {
  other.value.data.number = 0;
  other.path = nullptr;
}

QueryField::~QueryField() {
  if(path) delete path;
  if(value.value_type == QueryFieldValueType::string)
    value.data.string.~BufferArray();
  if(next) delete next;
}

QueryProperty QueryField::getPropertyType() const {return property;}

bool QueryField::isCurrentNodeTest() const {return path == nullptr;}
Path& QueryField::getPath() const {return *path;}
QueryOperator QueryField::getOperatorType() const {return operat;}
QueryNextFieldRelation QueryField::getNextRealation() const {return next_rel;}
QueryFieldValueType QueryField::getValueType() const {return value.value_type;}

VarType QueryField::getVarType() const {return value.data.type;}
VarTypeClass QueryField::getVarTypeClass() const {return value.data.type_class;}
ValType QueryField::getValType() const {return value.data.value_type;}
i64 QueryField::getNumber() const {return value.data.number;}
ui64 QueryField::getUNumber() const {return static_cast<ui64>(value.data.number);}
Range QueryField::getRange() const {return value.data.range;}
BufferArray QueryField::getString() const {return value.data.string;}

QueryField::iterator QueryField::begin() {return this;}
QueryField::iterator QueryField::end() {return nullptr;}






// QueryField::iterator

QueryField::iterator::iterator(QueryField* field) : current(field) {}
QueryField::iterator::iterator(const QueryField::iterator& other) : current(other.current) {}
QueryField::iterator::iterator(QueryField::iterator&& other) : current(other.current) {}
QueryField& QueryField::iterator::operator*() const {return *current;}
QueryField* QueryField::iterator::operator ->() const {return current;}
QueryField::iterator& QueryField::iterator::operator++() {current = current->next; return *this;}
QueryField::iterator QueryField::iterator::operator++(int) {QueryField* last = current; current = current->next; return last;}
bool QueryField::iterator::operator==(QueryField::iterator other) const {return current == other.current;}
bool QueryField::iterator::operator!=(QueryField::iterator other) const {return current != other.current;}

// QueryFieldGroup

QueryFieldGroup::QueryFieldGroup(std::initializer_list<QueryField> field_list,
                                 QueryNextFieldRelation next_relation)
  : fields(*field_list.begin()),
    next_relation(next_relation) {
  QueryField::iterator it = fields.begin();
  for(const QueryField& field : field_list) {
    if(&field == field_list.begin()) continue;
    it->next = new QueryField(field);
    ++it;
  }
}

QueryFieldGroup::QueryFieldGroup(std::initializer_list<QueryFieldGroup> field_group_list)
  : QueryFieldGroup(*field_group_list.begin()) {
  iterator it = begin();
  for(const QueryFieldGroup& field_group : field_group_list) {
    if(&field_group == field_group_list.begin()) continue;
    it->next = new QueryFieldGroup(field_group);
    ++it;
  }
}

QueryFieldGroup::QueryFieldGroup(const QueryFieldGroup& other)
  : fields(other.fields),
    next (other.next? new QueryFieldGroup(*other.next) : nullptr ),
    next_relation(other.next_relation) {}

QueryFieldGroup::QueryFieldGroup(QueryFieldGroup&& other)
  : fields(other.fields),
    next (other.next),
    next_relation(other.next_relation) {other.next = nullptr;}

QueryFieldGroup::~QueryFieldGroup() {if(next) delete next;}

QueryField& QueryFieldGroup::getFields() {return fields;}

QueryNextFieldRelation QueryFieldGroup::getNextRealation() {return next_relation;}

QueryFieldGroup::iterator QueryFieldGroup::begin() {return this;}

QueryFieldGroup::iterator QueryFieldGroup::end() {return nullptr;}

//  QueryFieldGroup::iterator

QueryFieldGroup::iterator::iterator(QueryFieldGroup* field_group) : current(field_group) {}
QueryFieldGroup::iterator::iterator(const QueryFieldGroup::iterator& other) : current(other.current) {}
QueryFieldGroup::iterator::iterator(QueryFieldGroup::iterator&& other) : current(other.current) {}
QueryFieldGroup& QueryFieldGroup::iterator::operator*() const {return *current;}
QueryFieldGroup* QueryFieldGroup::iterator::operator->() const {return current;}
QueryFieldGroup::iterator& QueryFieldGroup::iterator::operator++() {current = current->next; return *this;}
QueryFieldGroup::iterator QueryFieldGroup::iterator::operator++(int) {QueryFieldGroup* last = current; current = current->next; return last;}
bool QueryFieldGroup::iterator::operator==(QueryFieldGroup::iterator other) const {return current == other.current;}
bool QueryFieldGroup::iterator::operator!=(QueryFieldGroup::iterator other) const {return current != other.current;}


#endif
