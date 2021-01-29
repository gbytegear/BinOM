#ifndef VARIABLE_H
#define VARIABLE_H

#include "../types.h"

#include "primitive.h"
#include "buffer_array.h"
#include "array.h"
#include "object.h"

namespace binom {

class Variable {

  union types {
    void* ptr;
    VarType* type;
    byte* bytes;

    Primitive primitive;
    BufferArray buffer_array;
    Array array;
    Object object;

    types(void* ptr) : ptr(ptr) {}
    ~types() {}
  } data;

  void* clone() const;
  void destroy();

  friend class Primitive;
  friend class BufferArray;
  friend class Array;
  friend class Object;
  friend class Matrix;
  friend class Table;

  friend struct NamedVariable;

public:

  Variable() : data(nullptr) {}

  // Bool init
  Variable(bool value);

  // Primitive init

  Variable(ui8 value);
  Variable(ui16 value);
  Variable(ui32 value);
  Variable(ui64 value);

  Variable(i8 value);
  Variable(i16 value);
  Variable(i32 value);
  Variable(i64 value);
  Variable(f32 value);
  Variable(f64 value);

  // Buffer_array init
  Variable(const char* str);
  Variable(const std::string str);

  Variable(size_t size, ui8 value);
  Variable(size_t size, ui16 value);
  Variable(size_t size, ui32 value);
  Variable(size_t size, ui64 value);
  Variable(size_t size, i8 value);
  Variable(size_t size, i16 value);
  Variable(size_t size, i32 value);
  Variable(size_t size, i64 value);

  Variable(ui8* values, size_t size);
  Variable(ui16* values, size_t size);
  Variable(ui32* values, size_t size);
  Variable(ui64* values, size_t size);

  Variable(i8* values, size_t size);
  Variable(i16* values, size_t size);
  Variable(i32* values, size_t size);
  Variable(i64* values, size_t size);

  Variable(ui8arr array);
  Variable(ui16arr array);
  Variable(ui32arr array);
  Variable(ui64arr array);

  Variable(i8arr array);
  Variable(i16arr array);
  Variable(i32arr array);
  Variable(i64arr array);

  // Array
  Variable(varr array);

  // Object
  Variable(obj object);

  Variable(Primitive primitive);
  Variable(BufferArray buffer_array);
  Variable(Array array);
  Variable(Object object);

  Variable(Variable&& other);
  Variable(Variable& other);

  ~Variable() {destroy();}

  ByteArray serialize() const;
  static Variable deserialize(ByteArray::iterator& it);
  static inline Variable deserialize(ByteArray serialized) {ByteArray::iterator it = serialized.begin(); return deserialize(it);}

  inline void* getDataPointer() const {return data.ptr;}

  inline VarType type() const noexcept             {return (data.type == nullptr)? VarType::invlid_type :*data.type;}
  inline VarTypeClass typeClass() const noexcept   {return (data.type == nullptr)? VarTypeClass::invalid_type :toTypeClass(*data.type);}
  inline bool isNull() const noexcept              {return data.type == nullptr;}

  inline bool isPrimitive() const noexcept         {return typeClass() == VarTypeClass::primitive;}
  inline bool isBufferArray() const noexcept       {return typeClass() == VarTypeClass::buffer_array;}
  inline bool isArray() const noexcept             {return typeClass() == VarTypeClass::array;}
  inline bool isObject() const noexcept            {return typeClass() == VarTypeClass::object;}

  inline Primitive& toPrimitive() const noexcept         {return const_cast<Primitive&>(data.primitive);}
  inline BufferArray& toBufferArray() const noexcept     {return const_cast<BufferArray&>(data.buffer_array);}
  inline Array& toArray() const noexcept                 {return const_cast<Array&>(data.array);}
  inline Object& toObject() const noexcept               {return const_cast<Object&>(data.object);}

  Variable& operator=(Variable other);

  inline ui64 length() {
    return
          (isNull())
        ? 0
        : (isBufferArray() || isArray() || isObject())
        ? *reinterpret_cast<ui64*>(data.bytes + 1)
        : 1;
  }


};








// Initers or other

struct NamedVariable {
  BufferArray name;
  Variable variable;

  NamedVariable& operator=(NamedVariable& other) {
    name.data.ptr = other.name.clone();
    variable.data.ptr = other.variable.clone();
    return *this;
  }
};

}


std::ostream& operator<<(std::ostream& os, const binom::Variable& var);

#endif
