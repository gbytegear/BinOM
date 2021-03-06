#ifndef OBJECT_H
#define OBJECT_H

#include "../types.h"
#include "variable.h"

namespace binom {

class Object {
  union types {
      void* ptr;
      VarType* type;
      byte* bytes;
      types(void* ptr) : ptr(ptr) {}
  } data;

  inline ui64& length() const {return *reinterpret_cast<ui64*>(data.bytes + 1);}
  ui64 msize() const;

  void mch(size_t new_size);
  void* madd(size_t add_size);
  void msub(size_t sub_size);
  void* maddto(void* to, size_t size);
  void msubfrom(void* from, size_t size);
  void* clone() const;
  void destroy();

  friend class Variable;
public:
  Object();
  Object(vobj object);
  Object(const Object& other) : data(other.clone()) {}
  Object(Object&& other) : data(other.data.ptr) {other.data.ptr = nullptr;}

  ~Object() {destroy ();}

  ByteArray serialize() const;
  static Object deserialize(ByteArray::iterator& it);

  inline bool isEmpty() const {return !length();}
  inline ui64 getMemberCount() const {return *reinterpret_cast<ui64*>(data.bytes + 1);}
  bool contains(BufferArray name) const;

  Variable& insert(BufferArray name, Variable var);
  void remove(BufferArray name);
  BufferArray& rename(BufferArray old_name, BufferArray new_name);

  Object& operator=(Object other);
  Object& operator+=(NamedVariable named_variable);
  Object& operator+=(Object other);

  bool operator==(Object other) const;
  bool operator!=(Object other) const;
  bool operator<(Object other) const;
  bool operator<=(Object other) const;
  bool operator>(Object other) const;
  bool operator>=(Object other) const;

  i8 getCompare(Object other) const;

  NamedVariable& getNamedVariable(BufferArray name) const;
  Variable& getVariable(BufferArray name) const;
  inline Variable& operator[](BufferArray name) const {return getVariable(std::move(name));}

  ObjectIterator begin() const {return reinterpret_cast<ObjectIterator>(data.bytes + 9);}
  ObjectIterator end() const {return reinterpret_cast<ObjectIterator>(data.bytes + msize());}

  Array getNameArray();
  Array getMemberArray();

  Variable& asVar() {return *reinterpret_cast<Variable*>(this);}
};

}

#endif

