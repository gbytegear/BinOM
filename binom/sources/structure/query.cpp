#include "binom/includes/structure/query.h"

using namespace binom;




///////////////////////////////////////////////////////////////////////// QueryFieldValue

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
      string = other.string; return;
  }
}

QueryFieldValue::Data::Data(QueryFieldValueType vtype, QueryFieldValue::Data&& other)
  : Data(vtype, const_cast<const Data&>(other)) {}









///////////////////////////////////////////////////////////////////////// QueryField

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
        case QueryOperator::equal: case QueryOperator::not_equal:
        case QueryOperator::lower: case QueryOperator::lower_equal:
        case QueryOperator::highter: case QueryOperator::highte_equal:
        case QueryOperator::in_range: case QueryOperator::out_range:
        return true;
        default: return false;
      }
    case QueryProperty::index:
      if(value.value_type != QueryFieldValueType::number &&
         value.value_type != QueryFieldValueType::range)
        return false;
      switch (operat) {
        case QueryOperator::equal: case QueryOperator::not_equal:
        case QueryOperator::lower: case QueryOperator::lower_equal:
        case QueryOperator::highter: case QueryOperator::highte_equal:
        case QueryOperator::in_range: case QueryOperator::out_range:
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
}

QueryField::QueryField(QueryProperty property, QueryOperator operat, QueryFieldValue value, QueryNextFieldRelation next_rel)
  : value(std::move(value)),
    property(property),
    operat(operat),
    next_rel(next_rel) {if(!test()) throw SException(ErrCode::binom_query_invalid_field);}

QueryField::QueryField(QueryProperty property, Path path, QueryOperator operat, QueryFieldValue value, QueryNextFieldRelation next_rel)
  : value(std::move(value)),
    path(new Path(std::move(path))),
    property(property),
    operat(operat),
    next_rel(next_rel) {if(!test()) throw SException(ErrCode::binom_query_invalid_field);}

QueryField::QueryField(const QueryField& other)
  : value(other.value),
    path(other.path),
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

QueryField::QueryField(std::initializer_list<QueryField> field_list)
  : QueryField(*field_list.begin()) {
  iterator it = begin();
  for(const QueryField& field : field_list)
    if(&field != field_list.begin()) {
      it->next = new QueryField(field);
      ++it;
    }
}

QueryField::~QueryField() {
  if(path) delete path;
  if(value.value_type == QueryFieldValueType::string)
    value.data.string.~BufferArray();
  if(next) delete next;
}

QueryProperty QueryField::getPropertyType() const {return property;}
Path& QueryField::getPath() const {return *path;}
QueryOperator QueryField::getOperatorType() const {return operat;}
QueryNextFieldRelation QueryField::getNextRealation() const {return next_rel;}
QueryFieldValueType QueryField::getValueType() const {return value.value_type;}
VarType QueryField::getVarType() const {return value.data.type;}
VarTypeClass QueryField::getVarTypeClass() const {return value.data.type_class;}
ValType QueryField::getValType() const {return value.data.value_type;}
i64 QueryField::getNumber() const {return value.data.number;}
Range QueryField::getRange() const {return value.data.range;}
BufferArray QueryField::getString() const {return value.data.string;}

QueryField::iterator QueryField::begin() {return this;}
QueryField::iterator QueryField::end() {return nullptr;}






/////////////////////////////////////////////////////////////////////////// QueryField::iterator

QueryField::iterator::iterator(QueryField* field)
  : current(field) {}

QueryField::iterator::iterator(const QueryField::iterator& other)
  : current(other.current) {}

QueryField::iterator::iterator(QueryField::iterator&& other)
  : current(other.current) {}

QueryField& QueryField::iterator::operator*() const {return *current;}
QueryField* QueryField::iterator::operator ->() const {return current;}

QueryField::iterator& QueryField::iterator::operator++() {current = current->next; return *this;}
QueryField::iterator QueryField::iterator::operator++(int) {QueryField* last = current; current = current->next; return last;}

bool QueryField::iterator::operator==(QueryField::iterator other) const {return current == other.current;}
bool QueryField::iterator::operator!=(QueryField::iterator other) const {return current != other.current;}
