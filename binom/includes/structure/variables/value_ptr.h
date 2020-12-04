#ifndef VALUEPTR_H
#define VALUEPTR_H

#include "../types.h"
#include <iostream>
#include <iomanip>

namespace binom {

class ValuePtr {
  ValType type;
  union pointer {
    void* ptr;
    bool* boolptr;
    ui8* ui8ptr;
    ui16* ui16ptr;
    ui32* ui32ptr;
    ui64* ui64ptr;
    i8* i8ptr;
    i16* i16ptr;
    i32* i32ptr;
    i64* i64ptr;
    f32* f32ptr;
    f64* f64ptr;
    pointer(void* ptr) : ptr(ptr) {}
  } ptr;

  friend class ValueIterator;
public:
  ValuePtr(ValType type, void* pointer) : type(type), ptr(pointer) {}
  ValuePtr(VarType type, void* pointer) : type(toValueType(type)), ptr(pointer) {}
  ValuePtr(ValuePtr& other) : type(other.type), ptr(other.ptr.ptr) {}
  ValuePtr(ValuePtr&& other) : type(other.type), ptr(other.ptr.ptr) {}
  ValuePtr(ValueIterator& it);
  ValuePtr(ValueIterator&& it);

  bool asBool() const;
  ui64 asUnsigned() const;
  i64 asSigned() const;
  f64 asFloat() const;

  bool setBool(const bool value);
  ui64 setUnsigned(const ui64 value);
  i64 setSigned(const i64 value);
  f64 setFloat(const f64 value);

  inline ValueIterator& toIterator();

  inline bool operator=(const bool value) {return setBool(value);}
  inline ui64 operator=(const ui64 value) {return setUnsigned(value);}
  inline i64 operator=(const i64 value) {return setSigned(value);}
  inline f64 operator=(const f64 value) {return setFloat(value);}
  ValuePtr& operator=(const ValuePtr& other);
  ValuePtr& operator<<(const ValuePtr& other); //!< Set value from other to this
  ValuePtr& operator>>(ValuePtr& other) const; //!< Set value from this to other
};


class ValueIterator {
  ValuePtr pointer;

  inline ui8 getShift() const {
    switch (pointer.type) {
      case ValType::byte: return 1;
      case ValType::word: return 2;
      case ValType::dword: return 4;
      case ValType::qword: return 8;
    }
  }

public:
  ValueIterator(ValType type, void* pointer) : pointer(type, pointer) {}
  ValueIterator(VarType type, void* pointer) : pointer(type, pointer) {}
  ValueIterator(ValuePtr& val_ptr) : pointer(val_ptr) {}
  ValueIterator(ValuePtr&& val_ptr) : pointer(val_ptr) {}
  ValueIterator(ValueIterator& other) : pointer(other.pointer) {}
  ValueIterator(ValueIterator&& other) : pointer(other.pointer) {}

  inline ValuePtr& operator*() {return pointer;}
  inline ValuePtr* operator->() {return &pointer;}

  ValuePtr operator[](i64 index) {
    return ValuePtr(pointer.type, pointer.ptr.ui8ptr + getShift() * index);
  }

  ValueIterator& operator++() {
    pointer.ptr.ui8ptr += getShift();
    return *this;
  }

  ValueIterator& operator+=(i64 shift) {
    pointer.ptr.ui8ptr += getShift() * shift;
    return *this;
  }

  ValueIterator& operator--() {
    pointer.ptr.ui8ptr -= getShift();
    return *this;
  }

  ValueIterator& operator-=(i64 shift) {
    pointer.ptr.ui8ptr -= getShift() * shift;
    return *this;
  }

  ValueIterator& operator=(const ValueIterator& other) {
    pointer = other.pointer;
    return *this;
  }

  ValueIterator& operator=(const ValuePtr& other) {
    pointer = other;
    return *this;
  }

  inline bool operator==(const ValueIterator& other) const {return pointer.ptr.ptr == other.pointer.ptr.ptr;}
  inline bool operator!=(const ValueIterator& other) const {return pointer.ptr.ptr != other.pointer.ptr.ptr;}
};

std::ostream& operator<<(std::ostream& os, const ValuePtr val);

}

#endif // VALUEPTR_H
