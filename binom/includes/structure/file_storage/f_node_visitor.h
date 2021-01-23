#ifndef FILE_NODE_VISITOR_H
#define FILE_NODE_VISITOR_H

#include "vfmem.h"

namespace binom {

class FileNodeVisitor {
  VFMemoryController* vmemory;
  IndexedNodeDescriptor descriptor;

  void load(ui64 index);

public:
  FileNodeVisitor(VFMemoryController& vmemory) : vmemory(&vmemory), descriptor(vmemory.getRootNodeDescriptor()) {}
  FileNodeVisitor(VFMemoryController& vmemory, IndexedNodeDescriptor descriptor) : vmemory(&vmemory), descriptor(descriptor) {}
  FileNodeVisitor(FileNodeVisitor& other) : vmemory(other.vmemory), descriptor(other.descriptor) {}
  FileNodeVisitor(FileNodeVisitor&& other) : vmemory(other.vmemory), descriptor(other.descriptor) {}

  FileNodeVisitor& operator=(IndexedNodeDescriptor descriptor) {this->descriptor = descriptor; return *this;}
  FileNodeVisitor& operator=(FileNodeVisitor& other) {vmemory = other.vmemory; descriptor = other.descriptor; return *this;}
  FileNodeVisitor& operator=(FileNodeVisitor&& other) {vmemory = other.vmemory; descriptor = other.descriptor; return *this;}

  inline VarType getType() {return descriptor.descriptor.type;}
  inline VarTypeClass getTypeClass() {return toTypeClass(getType());}

  FileNodeVisitor& stepInside(ui64 index);
  FileNodeVisitor& stepInside(BufferArray name);
  FileNodeVisitor getChilde(ui64 index);
  FileNodeVisitor getChilde(BufferArray name);

  Variable getVariable();

  FileNodeVisitor operator[](ui64 index) {return FileNodeVisitor(*this).stepInside(index);}
  FileNodeVisitor operator[](BufferArray name) {return FileNodeVisitor(*this).stepInside(std::move(name));}

  FileNodeVisitor& operator()(ui64 index) {return stepInside(index);}
  FileNodeVisitor& operator()(BufferArray name) {return stepInside(std::move(name));}


};

}

#endif
