#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "../types.h"
#include "value.h"

namespace binom {

class Primitive {

  union types {
      void* ptr;
      VarType* type;
      byte* bytes;
      types(void* ptr) : ptr(ptr) {}
  } data;

  ui64 msize() const {
    switch (*data.type) {
      case VarType::byte: return 2;
      case VarType::word: return 3;
      case VarType::dword: return 5;
      case VarType::qword: return 9;
      default: throw SException(ErrCode::binom_invalid_type);
    }
  }

  void destroy() {free(data.ptr);data.ptr = nullptr;}

  friend class Variable;
public:
  Primitive() : data(nullptr) {}
  // Bool init
  Primitive(bool value);

  // Primitive init
  Primitive(ui8 value);
  Primitive(ui16 value);
  Primitive(ui32 value);
  Primitive(ui64 value);

  Primitive(i8 value);
  Primitive(i16 value);
  Primitive(i32 value);
  Primitive(i64 value);
  Primitive(f32 value);
  Primitive(f64 value);

  Primitive(Primitive&& other);
  Primitive(Primitive& other);

  ~Primitive() {destroy();}

  inline Value getValue() const {return Value(*data.type, data.bytes + 1);}

  inline Value operator*() const {return getValue();}

  inline bool operator=(const bool value) {return (**this).setBool(value);}
  inline ui64 operator=(const ui64 value) {return (**this).setUnsigned(value);}
  inline i64 operator=(const i64 value) {return (**this).setSigned(value);}
  inline f64 operator=(const f64 value) {return (**this).setFloat(value);}

  Primitive& operator=(Primitive& other) {
    getValue() << other.getValue();
    return *this;
  }
};

}

std::ostream& operator<<(std::ostream& os, binom::Primitive& primitive);

#endif
