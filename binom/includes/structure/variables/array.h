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

  ui64 msize() const {return 9 + length() * PTR_SZ;}

  void mch(size_t new_size) {
    data.ptr = tryRealloc(data.ptr, new_size);
  }

  void* madd(size_t add_size) {
    size_t shift = msize();
    mch(shift + add_size);
    return data.bytes + shift;
  }

  void msub(size_t sub_size) {
    mch(msize() - sub_size);
  }

  void* maddto(void* to, size_t size) {
    size_t pos = reinterpret_cast<byte*>(to) - data.bytes;
    size_t old_size = msize();
    madd(size);
    memmove(data.bytes + pos + size, data.bytes + pos, old_size - pos);
    return data.bytes + pos;
  }

  void destroy();

  friend class Variable;
public:
  Array(varr array);


  ArrayIterator begin() const {return reinterpret_cast<ArrayIterator>(data.bytes + 9);}
  ArrayIterator end() const {return reinterpret_cast<ArrayIterator>(data.bytes + msize());}

};
}

#endif
