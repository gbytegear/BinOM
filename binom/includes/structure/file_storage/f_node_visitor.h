#ifndef FILE_NODE_VISITOR_H
#define FILE_NODE_VISITOR_H

#include "vfmem.h"
#include "../variables/variable.h"

namespace binom {

class FileNodeVisitor {
  VFMemoryController* vmemory;
  IndexedNodeDescriptor descriptor;

  void loadNodeVisitor(ui64 index);
  void updateNodeVisitor();

  ByteArray loadData();

  ByteArray pack(Object& object);

  void set(Variable& variable);
  void set(Primitive& primitive);
  void set(BufferArray& buffer);
  void set(Array& array);
  void set(Object& object);

  ui64 create(Variable& variable);
  ui64 create(Primitive& primitive);
  ui64 create(BufferArray& buffer);
  ui64 create(Array& array);
  ui64 create(Object& object);

  void free();

public:
  FileNodeVisitor(VFMemoryController& vmemory) : vmemory(&vmemory), descriptor(vmemory.getRootNodeDescriptor()) {}
  FileNodeVisitor(VFMemoryController& vmemory, IndexedNodeDescriptor descriptor) : vmemory(&vmemory), descriptor(descriptor) {}
  FileNodeVisitor(FileNodeVisitor& other) : vmemory(other.vmemory), descriptor(other.descriptor) {}
  FileNodeVisitor(FileNodeVisitor&& other) : vmemory(other.vmemory), descriptor(other.descriptor) {}

  FileNodeVisitor& operator=(IndexedNodeDescriptor descriptor) {this->descriptor = descriptor; return *this;}
  FileNodeVisitor& operator=(FileNodeVisitor& other) {vmemory = other.vmemory; descriptor = other.descriptor; return *this;}
  FileNodeVisitor& operator=(FileNodeVisitor&& other) {vmemory = other.vmemory; descriptor = other.descriptor; return *this;}
  FileNodeVisitor& operator=(Variable var);

  inline VarType getType() {return descriptor.descriptor.type;}
  inline VarTypeClass getTypeClass() {return toTypeClass(getType());}

  FileNodeVisitor& stepInside(ui64 index);
  FileNodeVisitor& stepInside(BufferArray name);
  FileNodeVisitor getChild(ui64 index);
  FileNodeVisitor getChild(BufferArray name);

  Variable getVariable();

  FileNodeVisitor operator[](ui64 index) {return FileNodeVisitor(*this).stepInside(index);}
  FileNodeVisitor operator[](BufferArray name) {return FileNodeVisitor(*this).stepInside(std::move(name));}

  FileNodeVisitor& operator()(ui64 index) {return stepInside(index);}
  FileNodeVisitor& operator()(BufferArray name) {return stepInside(std::move(name));}

};

}

#endif
