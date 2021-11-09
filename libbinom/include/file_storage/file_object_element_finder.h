#ifndef FILE_OBJECT_ELEMENT_FINDER_H
#define FILE_OBJECT_ELEMENT_FINDER_H

#include "file_node_visitor.h"

namespace binom {

class FileNodeVisitor::ObjectElementFinder{
public:
  struct ObjectElement{
    ValType type;
    ui64 name_size;
    const void* name;
    virtual_index index;
  };

private:
  // Node Control
  FileMemoryManager& fmm;
  virtual_index node_index;
  RWGuard& rw_gurad;

  ObjectElementPosition pos;

  ObjectDescriptor getObjectDescriptor();
  ObjectNameLength getNameBlockDescriptor(ui64 index);

  ObjectElementFinder& findBlock(ValType type, block_size name_length);
  inline bool isNameBlockFinded() const {return pos.name_block_index != pos.descriptor.length_element_count;}

  ObjectElementFinder& findNameInBlock(void* name);
  inline bool isNameFinded() {return pos.name_shift != pos.descriptor.name_block_size;}

public:

  ObjectElementFinder(FileNodeVisitor& node_visitor);
  ObjectElementFinder(FileMemoryManager& fmm, virtual_index node_index, RWGuard& rw_gurad);

  ObjectElementFinder& dropPosition();

  bool findElement(BufferArray name);

  NamePosition getNamePosition();
  BufferArray getName();
  inline ui64 getElementCount() const {return pos.descriptor.index_count;}
  virtual_index getNodeIndex() const;

  inline ObjectElementPosition getObjectElementPosition() const {return pos;}

  void insert(BufferArray name, virtual_index node_index);
  bool remove(BufferArray name);

  void foreach(std::function<void(ObjectElement)> handler);

};


}

#endif // FILE_OBJECT_ELEMENT_FINDER_H
