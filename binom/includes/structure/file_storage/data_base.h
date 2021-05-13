#ifndef DATA_BASE_H
#define DATA_BASE_H

#include "file_virtual_memory_controller.h"
#include "data_base_node_visitor.h"

namespace binom {

class BinOMDataBase {
  FileVirtualMemoryController fvmc;
public:
  BinOMDataBase(std::string file_name, Variable init_struct = nullptr)
    : fvmc(std::move(file_name)) {
    if(isUninitializedRoot() && !init_struct.isNull())
      getRoot().setVariable(init_struct);
  }
  BinOMDataBase(const BinOMDataBase& other) = delete;
  BinOMDataBase(BinOMDataBase&& other) : fvmc(std::move(other.fvmc)) {}

  inline BinOMDataBase& operator=(const BinOMDataBase& other) {
    this->~BinOMDataBase();
    return *new(this) BinOMDataBase(std::move(const_cast<BinOMDataBase&>(other)));
  }

  inline BinOMDataBase& operator=(BinOMDataBase&& other) {
    this->~BinOMDataBase();
    return *new(this) BinOMDataBase(std::move(other));
  }

  static bool isValid(std::string file_name) {
    FileIO file(file_name);
    DBVersion version;
    file.read(0, version);
    return !memcmp(&version, &current, sizeof (DBVersion));
  }

  // DB Info
  inline f_size  getFileSize()      {return fvmc.getFileSize();}
  inline ui64    getNodePageCount() {return fvmc.getNodePageCount();}
  inline ui64    getHeapPageCount() {return fvmc.getHeapPageCount();}
  inline ui64    getBytePageCount() {return fvmc.getBytePageCount();}

  bool isUninitializedRoot() {return fvmc.loadNodeDescriptor(0).type == VarType::end;}

  inline Variable getDBInfo() {
    return vobj {
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

#endif // DATA_BASE_H
