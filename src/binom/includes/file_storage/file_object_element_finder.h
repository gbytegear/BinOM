#ifndef FILE_OBJECT_ELEMENT_FINDER_H
#define FILE_OBJECT_ELEMENT_FINDER_H

#include "file_node_visitor.h"

namespace binom {

class FileNodeVisitor::ObjectElementFinder{
  // Node Control
  FileNodeVisitor& node_visitor;
  FileMemoryManager& fmm;
  virtual_index node_index;
  RWGuard& rw_gurad;

  ObjectDescriptor descriptor;
  ObjectNameLength name_block_descriptor;

  ObjectDescriptor getObjectDescriptor();
  ObjectNameLength getNameBlockDescriptor(ui64 index);

  real_index name_block_index = 0;
  real_index name_index = 0;
  real_index index = 0;
public:

  struct ObjectElement{
    ValType type;
    ui64 name_size;
    const void* name;
    virtual_index index;
  };

  ObjectElementFinder(FileNodeVisitor& node_visitor);

  ObjectElementFinder& dropPosition();

  ObjectElementFinder& findBlock(ValType type, block_size name_length);
  inline bool isNameBlockFinded() const {return name_block_index != descriptor.length_element_count;}

  ObjectElementFinder& findNameInBlock(void* name);
  inline bool isNameFinded() {return name_index != descriptor.name_block_size;}

  bool findElement(BufferArray name);

  FileNodeVisitor::NamePosition getNamePosition();
  BufferArray getName();
  inline ui64 getElementCount() const {return descriptor.index_count;}
  virtual_index getNodeIndex() const;

  void insert(BufferArray name, virtual_index node_index);
  bool remove(BufferArray name);

  void foreach(std::function<void(ObjectElement)> handler);

};


}

#endif // FILE_OBJECT_ELEMENT_FINDER_H
