#ifndef DB_NODE_VISITOR_H
#define DB_NODE_VISITOR_H

#include "file_virtual_memory_controller.h"
#include "../variables/variable.h"

namespace binom {

class DBNodeVisitor {
  FileVirtualMemoryController& fvmc;
  NodeDescriptor node_descriptor;
  f_virtual_index node_index = 0;

  bool is_value_ptr = false;
  ui64 value_index = 0;

  NodeDescriptor loadNode(f_virtual_index node_index);
  void updateNode();
  ByteArray loadData();

  f_virtual_index createVariable(Variable var);
  f_virtual_index createPrimitive(Primitive primitive);
  f_virtual_index createBufferArray(BufferArray buffer_array);
  f_virtual_index createArray(Array array);
  f_virtual_index createObject(Object object);

  void deleteNode(f_virtual_index node_index);

public:

  DBNodeVisitor(FileVirtualMemoryController& fvmc, f_virtual_index node_index = 0);
  DBNodeVisitor(DBNodeVisitor& other);
  DBNodeVisitor(DBNodeVisitor&& other);

  DBNodeVisitor& operator=(DBNodeVisitor other);
  DBNodeVisitor& operator=(f_virtual_index _node_index);

  VarType getType();
  VarTypeClass getTypeClass();

  bool isPrimitive();
  bool isBufferArray();
  bool isArray();
  bool isObject();

  DBNodeVisitor& stepInside(ui64 index);
  DBNodeVisitor& stepInside(BufferArray name);

  Variable& getVariable() const;
  Variable& getVariable(ui64 index) const;
  Variable& getVariable(BufferArray name) const;

  DBNodeVisitor getChild(ui64 index);
  DBNodeVisitor getChild(BufferArray name);

  DBNodeVisitor operator[](ui64 index);
  DBNodeVisitor operator[](BufferArray name);

  DBNodeVisitor& operator()(ui64 index);
  DBNodeVisitor& operator()(BufferArray name);
};

}

#endif // DB_NODE_VISITOR_H
