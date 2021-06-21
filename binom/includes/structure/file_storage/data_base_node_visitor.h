#ifndef DATA_BASE_NODE_VISITOR_H
#define DATA_BASE_NODE_VISITOR_H

#include "file_virtual_memory_controller.h"
#include "../variables/variable.h"
#include "../path.h"
#include "../query.h"

#include <functional>

namespace binom {

class DBNodeVisitor;
class DBNodeIterator;

//typedef std::vector<DBNodeVisitor> DBNodeVector;
class DBNodeVector {
  ByteArray data;
public:
  typedef DBNodeVisitor* iterator;
  DBNodeVector() = default;
  DBNodeVector(DBNodeVector&& other);
  DBNodeVector(const DBNodeVector& other);
  void pushBack(DBNodeVisitor node);
  DBNodeVisitor& get(ui64 index);
  DBNodeVisitor& operator[](ui64 index);
  ui64 length();
  iterator begin();
  iterator end();

  DBNodeVector& foreach(std::function<void(DBNodeVisitor&)> callback);
  DBNodeVector& ifEmpty(std::function<void()> callback);

  Array toArray();
};

class DBNodeVisitor {

  friend class DBNodeIterator;
  friend class DBNodeIteratorEnd;

  FileVirtualMemoryController& fvmc;
  mutable NodeDescriptor node_descriptor;
  f_virtual_index node_index = 0;

  bool is_value_ptr = false;
  ui64 value_index = 0;

  NodeDescriptor loadNode(f_virtual_index node_index) const;
  void updateNode() const;
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

  bool test(Query& query, ui64 index, BufferArray name);

  DBNodeVisitor(FileVirtualMemoryController& fvmc, decltype(nullptr));

public:

  DBNodeVisitor(FileVirtualMemoryController& fvmc, f_virtual_index node_index = 0, bool is_value_ptr = false, ui64 value_index = 0);
  DBNodeVisitor(const DBNodeVisitor& other);
  DBNodeVisitor(const DBNodeVisitor&& other);

  DBNodeVisitor& operator=(DBNodeVisitor other);
  DBNodeVisitor& operator=(f_virtual_index _node_index);

  VarType getType() const;
  VarTypeClass getTypeClass() const;
  f_virtual_index getNodeIndex() const;
  ui64 getElementCount() const;

  bool isEmpty() const;
  bool isNull() const;
  bool isInvalid() const;
  bool isIterable() const;
  bool isPrimitive() const;
  bool isBufferArray() const;
  bool isArray() const;
  bool isObject() const;
  bool isValueRef() const;

  DBNodeVisitor& snapTo(f_virtual_index node_index);
  DBNodeVisitor& stepInside(ui64 index);
  DBNodeVisitor& stepInside(BufferArray name);
  DBNodeVisitor& stepInside(Path path);

  Variable getVariable() const;
  Variable getVariable(ui64 index) const;
  Variable getVariable(BufferArray name) const;
  Variable getVariable(Path path) const;

  void setVariable(Variable var);
  void pushBack(Variable var);
  void pushFront(Variable var);
  void insert(ui64 index, Variable var);
  void insert(BufferArray name, Variable var);
  void remove(ui64 index, ui64 count = 1);
  void remove(BufferArray name);
  void remove(Path path);
  void remove();

  DBNodeVisitor getChild(ui64 index) const;
  DBNodeVisitor getChild(BufferArray name) const;
  DBNodeVisitor getChild(Path path) const;

  DBNodeVisitor operator[](ui64 index) const;
  DBNodeVisitor operator[](BufferArray name) const;
  DBNodeVisitor operator[](Path path) const;

  DBNodeVisitor& operator()(ui64 index);
  DBNodeVisitor& operator()(BufferArray name);
  DBNodeVisitor& operator()(Path path);

  DBNodeVector findAll(Query query, DBNodeVector vector = DBNodeVector());
  DBNodeVisitor find(Query query);

  DBNodeIterator begin();
  inline decltype(nullptr) end() {return nullptr;}


  //functional

  DBNodeVisitor& ifNotNull(std::function<void(DBNodeVisitor&)> callback);
  DBNodeVisitor& ifNull(std::function<void()> callback);
  void foreach(std::function<void(DBNodeVisitor&)> callback);

};


class DBNodeIterator {

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

  DBNodeVisitor parent;
  ByteArray data;
  Index index;

public:
  DBNodeIterator(DBNodeVisitor& parent);
  DBNodeIterator(const DBNodeIterator& other);
  DBNodeIterator(DBNodeIterator&& other);
  DBNodeIterator& operator++();
  DBNodeVisitor operator*();
  ByteArray getName();
  bool isEnd();

  inline bool operator!=([[maybe_unused]] decltype(nullptr) null) {return !isEnd();}


};


}

#endif // DATA_BASE_NODE_VISITOR_H
