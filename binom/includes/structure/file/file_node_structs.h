#ifndef FILE_NODE_STRUCTS_H
#define FILE_NODE_STRUCTS_H
#include "../types.h"


namespace binom {

#pragma pack(push, 1)

struct NodeDescriptor {
  VarType type = VarType::end;
  ui16 hard_link_count = 0;
  ui64 pos = 0;
};

struct ArrayElement {
  ui64 node_index = 0;
};

struct ObjectElemeny {
  ui64 name_index = 0;
  ui64 node_index = 0;
};

struct BufferDescriptor {
  VarType type = VarType::end;
  ui32 size = 0;
};


#pragma pack(pop)

}


#endif
