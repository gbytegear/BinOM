#ifndef DATA_BASE_NODE_VISITOR_H
#define DATA_BASE_NODE_VISITOR_H

#include "file_virtual_memory_controller.h"
#include "../variables/variable.h"
#include "../path.h"
#include "../query.h"

namespace binom {

class DBNodeVisitor;
class DBNodeIterator;

typedef std::vector<DBNodeVisitor> DBNodeVector;

class DBNodeVisitor {

  friend class DBNodeIterator;
  friend class DBNodeIteratorEnd;

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

  bool test(Query& query, ui64 index, BufferArray name = "");

public:

  DBNodeVisitor(FileVirtualMemoryController& fvmc, f_virtual_index node_index = 0, bool is_value_ptr = false, ui64 value_index = 0);
  DBNodeVisitor(const DBNodeVisitor& other);
  DBNodeVisitor(const DBNodeVisitor&& other);

  DBNodeVisitor& operator=(DBNodeVisitor other);
  DBNodeVisitor& operator=(f_virtual_index _node_index);

  VarType getType() const;
  VarTypeClass getTypeClass() const;

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
  void remove(BufferArray name);

  DBNodeVisitor getChild(ui64 index) const;
  DBNodeVisitor getChild(BufferArray name) const;
  DBNodeVisitor getChild(PathNode path) const;

  DBNodeVisitor operator[](ui64 index) const;
  DBNodeVisitor operator[](BufferArray name) const;
  DBNodeVisitor operator[](PathNode name) const;

  DBNodeVisitor& operator()(ui64 index);
  DBNodeVisitor& operator()(BufferArray name);
  DBNodeVisitor& operator()(PathNode path);

  DBNodeVector findAll(Query query);
  DBNodeVisitor find(Query query);

  DBNodeIterator begin();
  inline decltype(nullptr) end() {return nullptr;}

};


namespace {

typedef ui64 ArrayIndex;
struct ObjectIndex {
  ui64 length_block;
  ui64 name_counter;
  ui64 name;
  ui64 index;

  bool operator==(ObjectIndex other) {
    return length_block == other.length_block &&
           name == other.name &&
           index == other.index;
  }

  bool operator!=(ObjectIndex other) {
    return length_block != other.length_block ||
           name != other.name ||
           index != other.index;
  }

};

union Index {
  ArrayIndex array_index;
  ObjectIndex object_index;
  Index(const Index& other)
    : object_index(other.object_index) {}
  Index()
    : object_index{0, 0, 0, 0}
  {}
};

}


class DBNodeIterator {

  DBNodeVisitor parent;
  ByteArray data;
  Index index;

public:
  DBNodeIterator(DBNodeVisitor& parent)
    : parent(parent),
      data(parent.loadData()) {

    switch (parent.getTypeClass()) {
      default:
      case binom::VarTypeClass::invalid_type:
      case binom::VarTypeClass::primitive:
        throw SException(ErrCode::binom_invalid_type);
      case binom::VarTypeClass::buffer_array:
      case binom::VarTypeClass::array:
        index.array_index = 0;
      break;
      case binom::VarTypeClass::object:
        ObjectDescriptor& obj_des = data.get<ObjectDescriptor>(0);

        index.object_index.length_block = sizeof (ObjectDescriptor);

        index.object_index.name_counter =
            data.get<ObjectNameLength>(0, index.object_index.length_block).name_count;

        index.object_index.name =
            sizeof (ObjectDescriptor) +
            obj_des.length_element_count*sizeof(ObjectNameLength);

        index.object_index.index =
            sizeof (ObjectDescriptor) +
            obj_des.length_element_count*sizeof(ObjectNameLength) +
            obj_des.name_block_size;

      break;
    }
  }

  DBNodeIterator(const DBNodeIterator& other)
    : parent(other.parent),
      data(std::move(other.data)),
      index(other.index) {}

  DBNodeIterator(DBNodeIterator&& other)
    : parent(other.parent),
      data(std::move(other.data)),
      index(other.index) {}

  DBNodeIterator& operator++() {
    switch (parent.getTypeClass()) {
      default:
      case binom::VarTypeClass::invalid_type:
      case binom::VarTypeClass::primitive:
        throw SException(ErrCode::binom_invalid_type);
      case binom::VarTypeClass::buffer_array:
        index.array_index += toSize(toValueType(parent.getType()));
      break;
      case binom::VarTypeClass::array:
        index.array_index += sizeof(f_virtual_index);
      break;
      case binom::VarTypeClass::object:
        index.object_index.index += sizeof(f_virtual_index);
        index.object_index.name += data.get<ObjectNameLength>(0, index.object_index.length_block).name_length;
        --index.object_index.name_counter;
        if(!index.object_index.name_counter)
          index.object_index.length_block += sizeof (ObjectNameLength);
      break;
    }
    return *this;
  }

  DBNodeVisitor operator*() {
    switch (parent.getTypeClass()) {
      default:
      case binom::VarTypeClass::invalid_type:
      case binom::VarTypeClass::primitive:
        throw SException(ErrCode::binom_invalid_type);
      case binom::VarTypeClass::buffer_array:
      return DBNodeVisitor(parent.fvmc, parent.node_index, true, index.array_index / toSize(toValueType(parent.getType())));
      case binom::VarTypeClass::array:
      return DBNodeVisitor(parent.fvmc, data.get<f_virtual_index>(0, index.array_index));
      case binom::VarTypeClass::object:
      return DBNodeVisitor(parent.fvmc, data.get<f_virtual_index>(0, index.object_index.index));
    }
  }


  bool isEnd() {
    switch (parent.getTypeClass()) {
      default:
      case binom::VarTypeClass::invalid_type:
      case binom::VarTypeClass::primitive:
        throw SException(ErrCode::binom_invalid_type);
      case binom::VarTypeClass::buffer_array:
      case binom::VarTypeClass::array:
      return (data.begin() + index.array_index) == data.end();
      case binom::VarTypeClass::object:
      return (data.begin() + index.object_index.index) == data.end();
    }
  }

  inline bool operator!=([[maybe_unused]] decltype(nullptr) null) {return !isEnd();}


};


}

#endif // DATA_BASE_NODE_VISITOR_H
