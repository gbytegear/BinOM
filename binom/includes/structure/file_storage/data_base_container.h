#ifndef DATA_BASE_CONTAINER_H
#define DATA_BASE_CONTAINER_H

#include "file_virtual_memory_controller.h"
#include "data_base_node_visitor.h"

namespace binom {

class DataBaseContainer {
  FileVirtualMemoryController fvmc;
public:
  DataBaseContainer(std::string file_name) : fvmc(std::move(file_name)) {}
  DataBaseContainer(const char* file_name) : fvmc(file_name) {}

  // DB Info
  inline f_size  getFileSize()      {return fvmc.getFileSize();}
  inline ui64    getNodePageCount() {return fvmc.getNodePageCount();}
  inline ui64    getHeapPageCount() {return fvmc.getHeapPageCount();}
  inline ui64    getBytePageCount() {return fvmc.getBytePageCount();}

  bool isUninitializedRoot() {return fvmc.loadNodeDescriptor(0).type == VarType::end;}

  inline Variable getDBInfo() {
    return obj {
      {"file_size", ui64(getFileSize())},
      {"node_page_count", getNodePageCount()},
      {"heap_page_count", getHeapPageCount()},
      {"byte_page_count", getBytePageCount()}
    };
  }

  DBNodeVisitor getRoot() {return fvmc;}
  DBNodeVisitor getByNodeIndex(f_virtual_index node_index) {return DBNodeVisitor(fvmc, node_index);}
};

}

#endif // DATA_BASE_CONTAINER_H
