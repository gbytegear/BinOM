#ifndef MATRIX_TEMPLATE_H
#define MATRIX_TEMPLATE_H

#include "../types.h"

namespace binom {

class Matrix {
  byte* ptr = nullptr;
  friend class Variable;

  void* clone() {}
  void destroy() {}

};

}

#endif
