#include "binom/includes/variables/value.h"

using namespace binom;

ValueRef::ValueRef(const ValueIterator& it) : type(it->type), ptr(it->ptr.ptr) {}
ValueRef::ValueRef(const ValueIterator&& it) : type(it->type), ptr(it->ptr.ptr) {}

bool ValueRef::asBool() const {
  switch (type) {
    case ValType::byte: return *ptr.boolptr;
    case ValType::word: return *ptr.ui16ptr;
    case ValType::dword: return *ptr.ui32ptr;
    case ValType::qword: return *ptr.ui64ptr;
    default: throw Exception(ErrCode::binom_invalid_type);
  }
}

ui64 ValueRef::asUnsigned() const {
  switch (type) {
    case ValType::byte: return *ptr.ui8ptr;
    case ValType::word: return *ptr.ui16ptr;
    case ValType::dword: return *ptr.ui32ptr;
    case ValType::qword: return *ptr.ui64ptr;
    default: throw Exception(ErrCode::binom_invalid_type);
  }
}

i64 ValueRef::asSigned() const {
  switch (type) {
    case ValType::byte: return *ptr.i8ptr;
    case ValType::word: return *ptr.i16ptr;
    case ValType::dword: return *ptr.i32ptr;
    case ValType::qword: return *ptr.i64ptr;
    default: throw Exception(ErrCode::binom_invalid_type);
  }
}

f64 ValueRef::asFloat() const {
  switch (type) {
    case ValType::byte: return *ptr.ui8ptr;
    case ValType::word: return *ptr.ui16ptr;
    case ValType::dword: return *ptr.f32ptr;
    case ValType::qword: return *ptr.f64ptr;
    default: throw Exception(ErrCode::binom_invalid_type);
  }
}

bool ValueRef::setBool(const bool value) {
  switch (type) {
    case ValType::byte: return *ptr.boolptr = value;
    case ValType::word: return *ptr.ui16ptr = static_cast<ui16>(value);
    case ValType::dword: return *ptr.ui32ptr = static_cast<ui32>(value);
    case ValType::qword: return *ptr.ui64ptr = static_cast<ui64>(value);
    default: throw Exception(ErrCode::binom_invalid_type);
  }
}

ui64 ValueRef::setUnsigned(const ui64 value) {
  switch (type) {
    case ValType::byte: return *ptr.ui8ptr = value;
    case ValType::word: return *ptr.ui16ptr = value;
    case ValType::dword: return *ptr.ui32ptr = value;
    case ValType::qword: return *ptr.ui64ptr = value;
    default: throw Exception(ErrCode::binom_invalid_type);
  }
}

i64 ValueRef::setSigned(const i64 value) {
  switch (type) {
    case ValType::byte: return *ptr.i8ptr = value;
    case ValType::word: return *ptr.i16ptr = value;
    case ValType::dword: return *ptr.i32ptr = value;
    case ValType::qword: return *ptr.i64ptr = value;
    default: throw Exception(ErrCode::binom_invalid_type);
  }
}

f64 ValueRef::setFloat(const f64 value) {
  switch (type) {
    case ValType::byte: return *ptr.i8ptr = value;
    case ValType::word: return *ptr.i16ptr = value;
    case ValType::dword: return *ptr.i32ptr = value;
    case ValType::qword: return *ptr.i64ptr = value;
    default: throw Exception(ErrCode::binom_invalid_type);
  }
}

ValueIterator& ValueRef::toIterator() {return *reinterpret_cast<ValueIterator*>(this);}

ValueRef& ValueRef::operator=(const ValueRef& other) {
  type = other.type;
  ptr.ptr = other.ptr.ptr;
  return *this;
}

ValueRef& ValueRef::operator<<(const ValueRef& other) {
  setUnsigned(other.asUnsigned());
  return *this;
}
