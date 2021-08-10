#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include "file_node_visitor.h"

namespace binom {

class FileStorage {
  FileMemoryManager fmm;
public:
  FileStorage(std::string_view file_name)
    : fmm(file_name) {}

  FileNodeVisitor getRoot() {return fmm;}
  FileNodeVisitor getNode(virtual_index index, real_index element_index = 0xFFFFFFFFFFFFFFFF) {
    return FileNodeVisitor(fmm, index, element_index);
  }

};

}

#endif // FILE_STORAGE_H
