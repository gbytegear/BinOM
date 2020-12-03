#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "../types.h"
#include "value_ptr.h"

namespace binom {

class Primitive {

  union types {
      void* ptr;
      VarType* type;
      byte* bytes;
      types(void* ptr) : ptr(ptr) {}
  } data;

//  byte* ptr = nullptr;
  friend class Variable;
public:
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

  ValuePtr getValue() const {return ValuePtr(*data.type, data.bytes + 1);}
};

}

#endif
