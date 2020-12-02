#ifndef OBJECT_H
#define OBJECT_H

#include "../types.h"
#include "variable.h"

namespace binom {

class Object {
  byte* ptr = nullptr;
  friend class Variable;
    
};
}

#endif

