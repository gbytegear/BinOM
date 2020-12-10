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
  void* clone();
  void destroy();

  friend class Variable;
public:
  Array(varr array);
  Array(Array&& other);
  Array(Array& other);

  inline ui64 getMemberCount() {return *reinterpret_cast<ui64*>(data.bytes + 1);}

  Variable& getVariable(ui64 index);
  inline Variable& operator[](ui64 index) {return getVariable(index);}

  ArrayIterator begin() const {return reinterpret_cast<ArrayIterator>(data.bytes + 9);}
  ArrayIterator end() const {return reinterpret_cast<ArrayIterator>(data.bytes + msize());}

};
}

#endif
