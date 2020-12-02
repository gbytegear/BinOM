#ifndef TABLE_TEMPLATE_H
#define TABLE_TEMPLATE_H

#include "../types.h"

namespace binom {

struct tbl {
  struct VarTypeInfo {
    const char* name;
    VarTemplateType type;
    ui64 size = 0;
  };
  std::initializer_list<VarTypeInfo> column_type_list;
  std::initializer_list<Variable> vars;
};


class Table {
  byte* ptr = nullptr;
  friend class Variable;
    
};
}

#endif
