#ifndef FILE_NODE_STRUCTS_H
#define FILE_NODE_STRUCTS_H
#include "../types.h"


namespace binom {

#pragma pack(push, 1)

struct RootDescriptor {
  ui32 binom_version = 0;
  ui32 node_page_size = 0; ///< 512 default
  ui32 data_page_size = 0; ///< 1024 default
};




struct NodePageDescriptor {
  ui32 node_count = 0;
  ui16 next_page_index = 0;
};

struct NodeDescriptor {
  VarType type = VarType::end;
  ui16 hard_link_count = 0;
  ui64 data_index = 0;
};




struct DataPageDescriptor {
  ui32 page_size = 0;
};

struct AODescriptor {
  ui32 element_count = 0;
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
