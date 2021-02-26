#ifndef DATA_BASE_CONTAINER_H
#define DATA_BASE_CONTAINER_H

#include "file_virtual_memory_controller.h"
#include "DB_node_visitor.h"

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

  DBNodeVisitor getRoot() {return fvmc;}
};

}

#endif // DATA_BASE_CONTAINER_H
