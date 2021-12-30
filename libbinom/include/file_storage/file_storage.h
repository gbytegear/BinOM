#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include "file_node_visitor.h"

namespace binom {

class DynamicStorage {
  FileMemoryManager fmm;
public:

  DynamicStorage(std::string_view file_name)
    : fmm(file_name) {}

  DynamicStorage(std::string_view file_name, Variable init_var, bool force_init = false)
    : fmm(file_name, force_init) {
    if(force_init || fmm.isEmpty())
      getRoot().setVariable(init_var);
  }

  inline FileNodeVisitor getRoot() {return fmm;}
  inline FileNodeVisitor getNodeByNodeIndex(virtual_index node_index, real_index element_index = 0xFFFFFFFFFFFFFFFF) {return FileNodeVisitor(fmm, node_index, element_index);}
  inline FileNodeVisitor getNodeByPath(Path path) {return getRoot()(std::move(path));}

  inline operator FileNodeVisitor () {return fmm;}
  inline operator Variable() {return getRoot();}
  inline FileNodeVisitor operator*() {return fmm;}
  inline FileNodeVisitor operator[](Path path) {return getNodeByPath(std::move(path));}
  inline FileNodeVisitor operator[](ui64 index) {return getRoot()(index);}
  inline FileNodeVisitor operator[](BufferArray name) {return getRoot()(std::move(name));}
};

}

#endif // FILE_STORAGE_H
