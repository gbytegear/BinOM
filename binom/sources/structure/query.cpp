#include "binom/includes/structure/query.h"

using namespace binom;

Query::Flag::Flag(QProp prop, QOper oper, bool has_path, QValType val_type, QRel next_rel)
  : prop(prop),
    oper(oper),
    has_path(has_path),
    val_type(val_type),
    next_rel(next_rel) {}




Query::QValue::Data::~Data() {}
Query::QValue::Data::Data(VarType type) : type(type) {}
Query::QValue::Data::Data(VarTypeClass type_class) : type_class(type_class) {}
Query::QValue::Data::Data(ValType value_type) : value_type(value_type) {}
Query::QValue::Data::Data(i64 number) : number(number) {}
Query::QValue::Data::Data(Range range) : range(range) {}
Query::QValue::Data::Data(BufferArray string) : string(std::move(string)) {}

Query::QValue::Data::Data(QValType vtype, const Query::QValue::Data& other) {
    switch (vtype) {
        case QValType::type:
            type = other.type; return;
        case QValType::type_class:
            type_class = other.type_class; return;
        case QValType::value_type:
            value_type = other.value_type; return;
        case QValType::number:
            number = other.number; return;
        case QValType::range:
            range = other.range; return;
        case QValType::string:
            new(&string) BufferArray(other.string); return;
    }
}






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

Query::QValue::QValue(binom::Query::QValue&& other)
    : value_type(other.value_type),
      data(other.value_type, other.data) {}


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

Query::Query(ByteArray data) : data(std::move(data)) {}

Query::Query(Query::QExprInitList exprs)
  : data(buildSubexpression(exprs)) {}

Query::Query(const Query& other) : data(other.data) {}
Query::Query(Query&& other) : data(std::move(other.data)) {}

Query& Query::operator=(Query&& other) {this->~Query(); return *new(this) Query(std::move(other));}
Query& Query::operator=(const Query& other) {this->~Query();return *new(this) Query(other);}

bool Query::isEmpty() const {return data.isEmpty();}

Query::iterator Query::begin() {return data.begin();}
Query::iterator Query::end() {return data.end();}

ByteArray Query::toByteArray() const {return data;}
Query Query::fromByteArray(ByteArray data) {return data;}




Query::QueryLiteral::QueryLiteral(QProp prop, Path path, QOper oper, QValue val, QRel next_rel)
  : flag(prop, oper, !path.isEmpty(), val.value_type, next_rel),
    path(path),
    value(std::move(val)) {}

Query::QueryLiteral::QueryLiteral(QProp prop, QOper oper, QValue val, QRel next_rel)
  : flag(prop, oper, false, val.value_type, next_rel),
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
                                   ? (*reinterpret_cast<ui64*>(it + sizeof(Flag)) + sizeof(ui64))
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

ui64 Query::QueryEpression::getUNumber() {return *reinterpret_cast<ui64*>(getDataPointer());}
i64 Query::QueryEpression::getNumber() {return *reinterpret_cast<i64*>(getDataPointer());}
Range Query::QueryEpression::getRange() {return *reinterpret_cast<Range*>(getDataPointer());}
BufferArray Query::QueryEpression::getString() {
  ByteArray::iterator it = getDataPointer();
  return BufferArray::deserialize(it);
}

Query::iterator Query::QueryEpression::begin() {
  ByteArray::iterator it = getDataPointer();
  for(; *it > 127; ++it);
  return ++it;
}

Query::iterator Query::QueryEpression::end() {
  ByteArray::iterator it = getDataPointer();
  return it + BufferArray::serializedSize(it);
}
