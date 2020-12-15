#ifndef TABLE_TEMPLATE_H
#define TABLE_TEMPLATE_H

#include "../types.h"

namespace binom {


class Table {
  byte* ptr = nullptr;
  friend class Variable;
    
  void* clone() {}
  void destroy() {}
};


}

#endif
