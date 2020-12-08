#include "binom/includes/structure/variables/value.h"

using namespace binom;

Value::Value(ValueIterator& it) : type(it->type), ptr(it->ptr.ptr) {}
Value::Value(ValueIterator&& it) : type(it->type), ptr(it->ptr.ptr) {}

bool Value::asBool() const {
  switch (type) {
    case ValType::byte: return *ptr.boolptr;
    case ValType::word: return *ptr.ui16ptr;
    case ValType::dword: return *ptr.ui32ptr;
    case ValType::qword: return *ptr.ui64ptr;
  }
}

ui64 Value::asUnsigned() const {
  switch (type) {
    case ValType::byte: return *ptr.ui8ptr;
    case ValType::word: return *ptr.ui16ptr;
    case ValType::dword: return *ptr.ui32ptr;
    case ValType::qword: return *ptr.ui64ptr;
  }
}

i64 Value::asSigned() const {
  switch (type) {
    case ValType::byte: return *ptr.i8ptr;
    case ValType::word: return *ptr.i16ptr;
    case ValType::dword: return *ptr.i32ptr;
    case ValType::qword: return *ptr.i64ptr;
  }
}

f64 Value::asFloat() const {
  switch (type) {
    case ValType::byte: return *ptr.ui8ptr;
    case ValType::word: return *ptr.ui16ptr;
    case ValType::dword: return *ptr.f32ptr;
    case ValType::qword: return *ptr.f64ptr;
  }
}

bool Value::setBool(const bool value) {
  switch (type) {
    case ValType::byte: return *ptr.boolptr = value;
    case ValType::word: return *ptr.ui16ptr = value;
    case ValType::dword: return *ptr.ui32ptr = value;
    case ValType::qword: return *ptr.ui64ptr = value;
  }
}

ui64 Value::setUnsigned(const ui64 value) {
  switch (type) {
    case ValType::byte: return *ptr.ui8ptr = value;
    case ValType::word: return *ptr.ui16ptr = value;
    case ValType::dword: return *ptr.ui32ptr = value;
    case ValType::qword: return *ptr.ui64ptr = value;
  }
}

i64 Value::setSigned(const i64 value) {
  switch (type) {
    case ValType::byte: return *ptr.i8ptr = value;
    case ValType::word: return *ptr.i16ptr = value;
    case ValType::dword: return *ptr.i32ptr = value;
    case ValType::qword: return *ptr.i64ptr = value;
  }
}

f64 Value::setFloat(const f64 value) {
  switch (type) {
    case ValType::byte: return *ptr.i8ptr = value;
    case ValType::word: return *ptr.i16ptr = value;
    case ValType::dword: return *ptr.i32ptr = value;
    case ValType::qword: return *ptr.i64ptr = value;
  }
}

ValueIterator& Value::toIterator() {return *reinterpret_cast<ValueIterator*>(this);}

Value& Value::operator=(const Value& other) {
  type = other.type;
  ptr.ptr = other.ptr.ptr;
  return *this;
}

Value& Value::operator<<(const Value& other) {
  setUnsigned(other.asUnsigned());
  return *this;
}

Value& Value::operator>>(Value& other) const {
  other.setUnsigned(asUnsigned());
  return *const_cast<Value*>(this);
}

std::ostream& operator<<(std::ostream& os, const binom::Value val) {

  ui8 sym_count = (val.getType() == ValType::byte) ? 2
                 :(val.getType() == ValType::word) ? 4
                 :(val.getType() == ValType::dword) ? 8
                 : 16;

  return os << std::right << std::setw(sym_count) << std::setfill('0') << std::hex << std::uppercase << val.asUnsigned();
}
