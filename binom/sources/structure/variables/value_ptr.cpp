#include "binom/includes/structure/variables/value_ptr.h"

using namespace binom;

ValuePtr::ValuePtr(ValueIterator& it) : type(it->type), ptr(it->ptr.ptr) {}
ValuePtr::ValuePtr(ValueIterator&& it) : type(it->type), ptr(it->ptr.ptr) {}

bool ValuePtr::asBool() const {
  switch (type) {
    case ValType::byte: return *ptr.boolptr;
    case ValType::word: return *ptr.ui16ptr;
    case ValType::dword: return *ptr.ui32ptr;
    case ValType::qword: return *ptr.ui64ptr;
  }
}

ui64 ValuePtr::asUnsigned() const {
  switch (type) {
    case ValType::byte: return *ptr.ui8ptr;
    case ValType::word: return *ptr.ui16ptr;
    case ValType::dword: return *ptr.ui32ptr;
    case ValType::qword: return *ptr.ui64ptr;
  }
}

i64 ValuePtr::asSigned() const {
  switch (type) {
    case ValType::byte: return *ptr.i8ptr;
    case ValType::word: return *ptr.i16ptr;
    case ValType::dword: return *ptr.i32ptr;
    case ValType::qword: return *ptr.i64ptr;
  }
}

f64 ValuePtr::asFloat() const {
  switch (type) {
    case ValType::byte: return *ptr.ui8ptr;
    case ValType::word: return *ptr.ui16ptr;
    case ValType::dword: return *ptr.f32ptr;
    case ValType::qword: return *ptr.f64ptr;
  }
}

bool ValuePtr::setBool(const bool value) {
  switch (type) {
    case ValType::byte: return *ptr.boolptr = value;
    case ValType::word: return *ptr.ui16ptr = value;
    case ValType::dword: return *ptr.ui32ptr = value;
    case ValType::qword: return *ptr.ui64ptr = value;
  }
}

ui64 ValuePtr::setUnsigned(const ui64 value) {
  switch (type) {
    case ValType::byte: return *ptr.ui8ptr = value;
    case ValType::word: return *ptr.ui16ptr = value;
    case ValType::dword: return *ptr.ui32ptr = value;
    case ValType::qword: return *ptr.ui64ptr = value;
  }
}

i64 ValuePtr::setSigned(const i64 value) {
  switch (type) {
    case ValType::byte: return *ptr.i8ptr = value;
    case ValType::word: return *ptr.i16ptr = value;
    case ValType::dword: return *ptr.i32ptr = value;
    case ValType::qword: return *ptr.i64ptr = value;
  }
}

f64 ValuePtr::setFloat(const f64 value) {
  switch (type) {
    case ValType::byte: return *ptr.i8ptr = value;
    case ValType::word: return *ptr.i16ptr = value;
    case ValType::dword: return *ptr.i32ptr = value;
    case ValType::qword: return *ptr.i64ptr = value;
  }
}

ValueIterator& ValuePtr::toIterator() {return *reinterpret_cast<ValueIterator*>(this);}

ValuePtr& ValuePtr::operator=(const ValuePtr& other) {
  type = other.type;
  ptr.ptr = other.ptr.ptr;
  return *this;
}

ValuePtr& ValuePtr::operator<<(const ValuePtr& other) {
  setUnsigned(other.asUnsigned());
  return *this;
}

ValuePtr& ValuePtr::operator>>(ValuePtr& other) const {
  other.setUnsigned(asUnsigned());
  return *const_cast<ValuePtr*>(this);
}

std::ostream& operator<<(std::ostream& os, const binom::ValuePtr val) {

  ui8 sym_count = (val.getType() == ValType::byte) ? 2
                 :(val.getType() == ValType::word) ? 4
                 :(val.getType() == ValType::dword) ? 8
                 : 16;

  return os << std::right << std::setw(sym_count) << std::setfill('0') << std::hex << std::uppercase << val.asUnsigned();
}
