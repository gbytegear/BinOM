#ifndef MATRIX_TEMPLATE_H
#define MATRIX_TEMPLATE_H

#include "../types.h"

namespace binom {

class Matrix {
  union types {
      void* ptr;
      VarType* type;
      byte* bytes;
      types(void* ptr) : ptr(ptr) {}
  } data;

  friend class Variable;

  inline ui64& length() const {return *reinterpret_cast<ui64*>(data.bytes + 1);}
  inline ui64& descriptor_length() const {return *reinterpret_cast<ui64*>(data.bytes + 9);}
  inline ui64 msize() const {return 17 + descriptor_length() + length()*descriptor_length()*8;}

  void mch(size_t new_size);
  void* madd(size_t add_size);
  void msub(size_t sub_size);
  void* maddto(void* to, size_t size);
  void msubfrom(void* from, size_t size);

  void* clone() {}
  void destroy() {}

};

}

#endif
