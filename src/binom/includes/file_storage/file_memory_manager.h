#ifndef FILE_MEMORY_MANAGER_H
#define FILE_MEMORY_MANAGER_H

#include "binom/includes/utils/file.h"
#include "fmm_containers.h"

namespace binom {

// NOTE: Use the virtual index of the node descriptor as a store for primitive values

class FMemoryManager {
  FileIO file;
  NodePageVector node_page_vector;
  HeapPageVector heap_page_vector;
  HeapMap heap_map;
  DBHeader db_header;
  std::mutex mtx;

  void allocNodePage();
  void allocHeapPage();
  inline VMemoryBlock allocHeapBlock(block_size size) {return heap_map.allocBlock(size);}
  inline void freeHeapBlock(virtual_index v_index) {return heap_map.freeBlock(v_index);}

  real_index translateVNodeIndex(virtual_index v_index);
  real_index translateVHeapIndex(virtual_index v_index);
  RMemoryBlockVector translateVMemoryBlock(VMemoryBlock v_mem_block);

  void init();

public:
  FMemoryManager(std::string_view file_path);

  NodeDescriptor getNodeDescriptor(virtual_index node_index);
  ByteArray getNodeData(virtual_index node_index);
  ByteArray getNodeDataPart(virtual_index node_index, real_index shift, block_size size);

  virtual_index create(VarType type, ByteArray data);
  void rewrite(virtual_index node_index, VarType type, ByteArray data);
  void rewrite(virtual_index node_index, ByteArray data);
  void remove(virtual_index node_index);

};

}

#endif // FILE_MEMORY_MANAGER_H
