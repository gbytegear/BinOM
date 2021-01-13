#ifndef ARRAY_H
#define ARRAY_H

#include "../types.h"

namespace binom {
class Array {
  union types {
      void* ptr;
      VarType* type;
      byte* bytes;
      types(void* ptr) : ptr(ptr) {}
  } data;

  inline ui64& length() const {return *reinterpret_cast<ui64*>(data.bytes + 1);}
  inline ui64 msize() const {return 9 + length() * PTR_SZ;}

  void mch(size_t new_size);
  void* madd(size_t add_size);
  void msub(size_t sub_size);
  void* maddto(void* to, size_t size);
  void msubfrom(void* from, size_t size);
  void* clone();
  void destroy();

  friend class Variable;
public:
  Array();
  Array(varr array);
  Array(Array&& other);
  Array(Array& other);
  ~Array() {destroy ();}

  ByteArray serialize() const;
  static Array deserialize(ByteArray::iterator& it);

  inline bool isEmpty() const {return !length();}
  inline ui64 getMemberCount() const {return *reinterpret_cast<ui64*>(data.bytes + 1);}

  Variable& getVariable(ui64 index) const;
  inline Variable& operator[](ui64 index) const {return getVariable(index);}

  Variable& insert(ui64 index, Variable var);
  Variable& pushBack(Variable var);
  Variable& pushFront(Variable var);

  void remove(ui64 index, ui64 n = 1);
  void popBack(ui64 n = 1);
  void popFront(ui64 n = 1);

  void clear();

  Variable& operator+=(Variable var);

  ArrayIterator begin() const {return reinterpret_cast<ArrayIterator>(data.bytes + 9);}
  ArrayIterator end() const {return reinterpret_cast<ArrayIterator>(data.bytes + msize());}

  Variable& asVar() {return *reinterpret_cast<Variable*>(this);}
};
}

std::ostream& operator<<(std::ostream& os, const binom::Array& array);

#endif
