#ifndef FILE_VIRTUAL_MEMORY_CONTROLLER_H
#define FILE_VIRTUAL_MEMORY_CONTROLLER_H

#include "fvmc_utils.h"

namespace binom {

typedef RWSyncMap::RWGuard RWGuard;

class FileVirtualMemoryController {
  FileIO file;
  NodePageVector node_page_list;
  BytePageVector byte_page_list;
  HeapPageVector heap_page_list;
  MemoryBlockList heap_block_list;
  RWSyncMap rw_sync_map;

  DBHeader header;

  void init();

  // Page creation
  void createNodePage();
  void createHeapPage();
  void createBytePage();

  // Virtual index translation
  f_real_index getRealNodePos(f_virtual_index v_index);
  f_real_index getRealHeapPos(f_virtual_index v_index);
  f_real_index getRealBytePos(f_virtual_index v_index);

  struct VMemoryBlock {f_virtual_index v_index;f_size size;};
  struct RealBlock {f_real_index r_index; f_size size;};

  // Node management
  f_virtual_index allocNode(NodeDescriptor descriptor);
  void setNode(f_virtual_index v_index, NodeDescriptor descriptor);
  void freeNode(f_virtual_index v_index);

  // Heap data management
  ByteArray getRealHeapBlocks(f_virtual_index index, f_size size);
  VMemoryBlock allocHeapData(const ByteArray data);
  ByteArray loadHeapData(f_virtual_index data_index);
  void freeHeapData(f_virtual_index index) {heap_block_list.freeBlock(index);}

  // Primitive data management
  f_virtual_index allocByteBlock(ValType type);
  f_virtual_index allocByteData(ValType type, ByteArray data);
  void setByteData(f_virtual_index index, ValType type, ByteArray data);
  ByteArray loadByteData(f_virtual_index index, ValType type);
  void freeByteData(f_virtual_index index, ValType type);

  friend class MemoryBlockList;
public:
  FileVirtualMemoryController(const char* filename) : file(filename) {init();}
  FileVirtualMemoryController(std::string filename) : file(filename) {init();}
  FileVirtualMemoryController(const FileVirtualMemoryController& other) = delete;
  FileVirtualMemoryController(FileVirtualMemoryController&& other)
    : file(std::move(other.file)),
      node_page_list(std::move(other.node_page_list)),
      byte_page_list(std::move(other.byte_page_list)),
      heap_page_list(std::move(other.heap_page_list)),
      heap_block_list(std::move(other.heap_block_list)),
      header(other.header) {}

  // DB Info
  f_size  getFileSize() {return file.size();}
  ui64    getNodePageCount() {return node_page_list.getPageCount();}
  ui64    getHeapPageCount() {return heap_page_list.getPageCount();}
  ui64    getBytePageCount() {return byte_page_list.getPageCount();}

  // DB Back-end IO interface
  f_virtual_index createNode(VarType type, ByteArray data);
  void            updateNode(f_virtual_index node_index, VarType type, ByteArray data);
  void            freeNodeData(f_virtual_index node_index);
  void            free(f_virtual_index node_index);
  void            markNodeAsBusy(f_virtual_index node_index);
  bool            isBusyNode(f_virtual_index node_index);
  void            setHeapDataPartByNode(f_virtual_index node_index, f_real_index shift, ByteArray data);

  ByteArray       loadDataByNode(f_virtual_index node_index);
  ByteArray       loadHeapDataByIndex(f_virtual_index heap_index) { return loadHeapData(heap_index); }
  ByteArray       loadHeapDataPartByIndex(f_virtual_index heap_index, f_real_index shift, f_size size);
  ByteArray       loadHeapDataPartByNode(f_virtual_index node_index, f_real_index shift, f_size size) { return loadHeapDataPartByIndex(loadNodeDescriptor(node_index).index, shift, size); }
  ByteArray       loadByteDataByIndex(f_virtual_index byte_index, ValType type) { return loadByteData( byte_index, type); }
  NodeDescriptor  loadNodeDescriptor(f_virtual_index v_index);

  // Synchroniztion
  inline RWGuard getRWGuard(f_virtual_index node_index) {return rw_sync_map.get(node_index);}

  void clear();

};

}

#endif // FILE_VIRTUAL_MEMORY_CONTROLLER_H
