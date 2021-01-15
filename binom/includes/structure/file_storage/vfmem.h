#ifndef VFMEM_H
#define VFMEM_H

#include "file_io.h"
#include "file_structs.h"

namespace binom {

class VFMemory {

  // Constants
  static constexpr ui64 node_segment_size = 8*8*sizeof(NodeDescriptor);
  static constexpr ui64 primitive_segment_size = 8*8;

  struct NodeSegmentList {
    NodeSegmentList* next = nullptr;
    ui64 header_pos;
    NodeSegmentDescriptor descriptor;
  };

  struct PrimitiveSegmentList {
    PrimitiveSegmentList* next = nullptr;
    ui64 header_pos;
    PrimitiveSegementDescriptor descriptor;
  };

  struct MemoryBlock {
    ui64 pos = 0;
    ui64 size = 0;

    MemoryBlock cut(ui64 block_size) {
      MemoryBlock new_block{pos, block_size};
      if(block_size > size) throw SException(ErrCode::binomdb_memory_management_error);
      pos += block_size;
      size -= block_size;
      return new_block;
    }
  };

  struct MemoryBlockChain {
    MemoryBlockChain* next = nullptr;
    MemoryBlock mem_block;
  };

  struct NodeBuffer {
    NodeSegmentDescriptor segment_descriptor;
    NodeDescriptor node_descriptors[16];
  };

  // VFM Data
  DBHeader header;
  NodeSegmentList* last_node_segment = &node_segments;
  PrimitiveSegmentList* last_primitive_segment = &primitive_segments;

  NodeSegmentList node_segments;
  PrimitiveSegmentList primitive_segments;

  MemoryBlockChain free_mem_chain;

  FileIO file;


  void init();
  void findFree();

public:
  VFMemory(std::string filename) : file(std::move(filename)) {init();}
  VFMemory(const char* filename) : file(filename) {init();}

  ui64 getFileSize() {return file.size();}

};

}
#endif
