#ifndef DB_NODE_VISITOR_H
#define DB_NODE_VISITOR_H

#include "file_virtual_memory_controller.h"
#include "../variables/variable.h"
#include "../path.h"
#include "../query.h"

namespace binom {

class DBNodeVisitor {
  FileVirtualMemoryController& fvmc;
  NodeDescriptor node_descriptor;
  f_virtual_index node_index = 0;

  bool is_value_ptr = false;
  ui64 value_index = 0;

  NodeDescriptor loadNode(f_virtual_index node_index) const;
  void updateNode();
  ByteArray loadData();

  f_virtual_index createVariable(Variable var);
  f_virtual_index createPrimitive(Primitive primitive);
  f_virtual_index createBufferArray(BufferArray buffer_array);
  f_virtual_index createArray(Array array);
  f_virtual_index createObject(Object object);

  void setPrimitive(f_virtual_index node_index, Primitive primitive);
  void setBufferArray(f_virtual_index node_index, BufferArray buffer_array);
  void setArray(f_virtual_index node_index, Array array);
  void setObject(f_virtual_index node_index, Object object);

  Variable buildVariable(f_virtual_index node_index) const;
  Primitive buildPrimitive(NodeDescriptor node_des) const;
  BufferArray buildBufferArray(NodeDescriptor node_des) const;
  Array buildArray(NodeDescriptor node_des) const;
  Object buildObject(NodeDescriptor node_des) const;

  void clearNode(f_virtual_index node_index);
  void deleteNode(f_virtual_index node_index);

public:

  DBNodeVisitor(FileVirtualMemoryController& fvmc, f_virtual_index node_index = 0);
  DBNodeVisitor(const DBNodeVisitor& other);
  DBNodeVisitor(const DBNodeVisitor&& other);

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
  DBNodeVisitor& stepInside(PathNode path);

  Variable getVariable() const;
  Variable getVariable(ui64 index) const;
  Variable getVariable(BufferArray name) const;
  Variable getVariable(PathNode path) const;

  void setVariable(Variable var);
  void pushBack(Variable var);
  void pushFront(Variable var);
  void insert(ui64 index, Variable var);
  void insert(BufferArray name, Variable var);
  void remove(ui64 index, ui64 count = 1);
  void remove(BufferArray name); // TODO Current

  DBNodeVisitor getChild(ui64 index) const;
  DBNodeVisitor getChild(BufferArray name) const;
  DBNodeVisitor getChild(PathNode path) const;

  DBNodeVisitor operator[](ui64 index) const;
  DBNodeVisitor operator[](BufferArray name) const;
  DBNodeVisitor operator[](PathNode name) const;

  DBNodeVisitor& operator()(ui64 index);
  DBNodeVisitor& operator()(BufferArray name);
  DBNodeVisitor& operator()(PathNode path);
};

}

#endif // DB_NODE_VISITOR_H
