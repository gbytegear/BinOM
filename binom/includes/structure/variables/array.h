#ifndef ARRAY_H
#define ARRAY_H

#include "../types.h"

namespace binom {
class Array {
  byte* ptr = nullptr;
  friend class Variable;
};
}

#endif
