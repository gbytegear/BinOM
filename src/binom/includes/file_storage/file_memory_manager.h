#ifndef FILE_MEMORY_MANAGER_H
#define FILE_MEMORY_MANAGER_H

#define DEBUG

#include "binom/includes/utils/file.h"
#include "fmm_containers.h"
#include "../utils/rwguard.h"

namespace binom {

// NOTE: Use the virtual index of the node descriptor as a store for primitive values

class FileMemoryManager {
  FileIO file;
  NodePageVector node_page_vector;
  HeapPageVector heap_page_vector;
  HeapMap heap_map;
  DBHeader db_header;
  std::shared_mutex node_page_mtx;
  std::shared_mutex heap_page_mtx;
  RWSyncMap sync_map;

  void init();

  void allocNodePage();
  void allocHeapPage();

  VMemoryBlock allocHeapBlock(block_size size);


  virtual_index allocNode();
  inline void freeHeapBlock(virtual_index v_index) {return heap_map.freeBlock(v_index);}

  real_index translateVNodeIndex(virtual_index v_index);
  real_index translateVHeapIndex(virtual_index v_index);
  RMemoryBlockVector translateVMemoryBlock(VMemoryBlock v_mem_block);

  void writeToVBlock(VMemoryBlock block, ByteArray data);

public:
  FileMemoryManager(std::string_view file_path);

  NodeDescriptor getNodeDescriptor(virtual_index node_index);
  ByteArray getNodeData(virtual_index node_index);
  ByteArray getNodeDataPart(virtual_index node_index, real_index shift, block_size size);

  virtual_index createNode(VarType type, ByteArray data);
  void updateNode(virtual_index node_index, VarType type, ByteArray data);
  inline void updateNode(virtual_index node_index, ByteArray data) {return updateNode(node_index, VarType::end, std::move(data));}
  void removeNode(virtual_index node_index);

  inline RWSyncMap::RWGuard getRWGuard(f_virtual_index node_index) {return sync_map.get(node_index);}

IF_DEBUG(
  void check();
)

};

}

#endif // FILE_MEMORY_MANAGER_H
