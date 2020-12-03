#ifndef VALUEPTR_H
#define VALUEPTR_H

#include "../types.h"

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
public:
  ValuePtr(ValType type, void* pointer) : type(type), ptr(pointer) {}
  ValuePtr(VarType type, void* pointer) : type(toValueType(type)), ptr(pointer) {}
  ValuePtr(ValuePtr& other) : type(other.type), ptr(other.ptr.ptr) {}
  ValuePtr(ValuePtr&& other) : type(other.type), ptr(other.ptr.ptr) {}

  bool asBool() const;
  ui64 asUnsigned() const;
  i64 asSigned() const;
  f64 asFloat() const;

  bool setBool(const bool value);
  ui64 setUnsigned(const ui64 value);
  i64 setSigned(const i64 value);
  f64 setFloat(const f64 value);

  inline bool operator=(const bool value) {return setBool(value);}
  inline ui64 operator=(const ui64 value) {return setUnsigned(value);}
  inline i64 operator=(const i64 value) {return setSigned(value);}
  inline f64 operator=(const f64 value) {return setFloat(value);}
};

}

#endif // VALUEPTR_H
