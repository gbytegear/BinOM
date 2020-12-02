#ifndef BOOLEAN_H
#define BOOLEAN_H

#include "../types.h"

namespace binom {

class Boolean {
  byte* ptr = nullptr;
  friend class Variable;
};

}

#endif
