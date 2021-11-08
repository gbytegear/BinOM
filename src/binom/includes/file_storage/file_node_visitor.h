#ifndef FILE_NODE_VISITOR_H
#define FILE_NODE_VISITOR_H

#include "file_memory_manager.h"
#include "../utils/node_visitor_base.h"

namespace binom {


class FileNodeVisitor : public NodeVisitorBase {
public:
  class NodeIterator;

  struct ObjectElementPosition {
    ObjectDescriptor descriptor = {};
    ObjectNameLength name_block_descriptor = {};
    real_index name_block_index = 0;
    real_index name_index = 0;
    real_index name_shift = 0;
    real_index index = 0;
  };

private:
  typedef RWSyncMap::ScopedRWGuard ScopedRWGuard;
  typedef RWSyncMap::RWGuard RWGuard;
  typedef RWSyncMap::LockType LockType;
  class ObjectElementFinder;
  static constexpr virtual_index null_index = 0xFFFFFFFFFFFFFFFF;

  struct NamePosition {
    virtual_index parent_node_index = 0;
    ValType char_type = ValType::invalid_type;
    element_cnt length = 0;
    real_index name_pos = 0;
    void setNull() {char_type = ValType::invalid_type;}
    bool isNull() {return char_type == ValType::invalid_type;}
  };


  FileMemoryManager& fmm;
  virtual_index node_index = 0;
  real_index index = null_index;
  NamePosition name_pos;
  mutable RWGuard current_rwg;

  friend class FileNodeVisitor::ObjectElementFinder;
  friend class DynamicStorage;
  friend class FileNodeVisitor::NodeIterator;

  Variable buildVariable(virtual_index node_index) const;
  Primitive buildPrimitive(virtual_index node_index, const NodeDescriptor* descriptor = nullptr, ScopedRWGuard* lk = nullptr) const;
  BufferArray buildBufferArray(virtual_index node_index, const NodeDescriptor* descriptor = nullptr, ScopedRWGuard* lk = nullptr) const;
  Array buildArray(virtual_index node_index, const NodeDescriptor* descriptor = nullptr, ScopedRWGuard* lk = nullptr) const;
  Object buildObject(virtual_index node_index, const NodeDescriptor* descriptor = nullptr, ScopedRWGuard* lk = nullptr) const;

  virtual_index createVariable(Variable variable);
  virtual_index createPrimitive(Primitive primitive);
  virtual_index createBufferArray(BufferArray buffer_array);
  virtual_index createArray(Array array);
  virtual_index createObject(Object object);

  ByteArray getContainedNodeIndexes(virtual_index node_index);

  NodeDescriptor getDescriptor() const {
    if(node_index == null_index) return NodeDescriptor::null();
    auto lk = getScopedRWGuard(LockType::shared_lock);
    return fmm.getNodeDescriptor(node_index);
  }

  NodeFullInfo getFullNodeInfo() {
    if(node_index == null_index) return {null_index, NodeDescriptor::null(), ByteArray()};
    auto lk = getScopedRWGuard(LockType::shared_lock);
    return fmm.getFullNodeInfo(node_index);
  }

  FileNodeVisitor& setNull() {
    node_index = null_index;
    index = null_index;
    return *this;
  }

  bool test(Query query, ui64 index) noexcept;

  FileNodeVisitor(FileMemoryManager& fmm, decltype (nullptr))
    : fmm(fmm), node_index(null_index), index(null_index) {}

  FileNodeVisitor(FileMemoryManager& fmm,
           virtual_index node_index = 0,
           real_index index = null_index)
    : fmm(fmm),
      node_index(node_index),
      index(index),
      current_rwg(fmm.getRWGuard(node_index))
  {}

  FileNodeVisitor(FileMemoryManager& fmm,
           virtual_index node_index,
           NamePosition name_pos)
    : fmm(fmm),
      node_index(node_index),
      index(null_index),
      name_pos(name_pos),
      current_rwg(fmm.getRWGuard(node_index))
  {}

public:

  FileNodeVisitor(const FileNodeVisitor& other)
    : fmm(other.fmm),
      node_index(other.node_index),
      index(other.index),
      name_pos(other.name_pos),
      current_rwg(fmm.getRWGuard(node_index))
  {}

  ScopedRWGuard getScopedRWGuard(LockType lock_type = LockType::unlocked) const {return ScopedRWGuard(current_rwg, lock_type);}

  inline FileNodeVisitor& operator=(FileNodeVisitor& other) {this->~FileNodeVisitor(); return *new(this) FileNodeVisitor(other);}
  inline FileNodeVisitor& operator=(virtual_index node_index) {this->~FileNodeVisitor(); return *new(this) FileNodeVisitor(fmm, node_index);}

  VarType getType() const override;
  VisitorType getVisitorType() const override {return VisitorType::file_storage_visitor;}
  inline virtual_index getNodeIndex() const {return node_index;}

  inline bool isNull() const override {return node_index == null_index;}
  inline bool isValueRef() const override {return index != null_index;}
  ui64 getElementCount() const override;
  std::optional<BufferArray> getName();

  FileNodeVisitor& stepInside(ui64 index) override;
  FileNodeVisitor& stepInside(BufferArray name) override;
  FileNodeVisitor& stepInside(Path path) override;

  Variable getVariable() const;
  inline Variable getVariable(ui64 index) const {return getChild(index).getVariable();}
  inline Variable getVariable(BufferArray name) const {return getChild(name).getVariable();}
  inline Variable getVariable(Path path) const {return getChild(path).getVariable();}

  virtual bool contains(ui64 index) override;
  virtual bool contains(BufferArray name) override;

  void setVariable(Variable var) override;
  void pushBack(Variable var) override;
  void pushFront(Variable var) override;
  void insert(ui64 index, Variable var) override;
  void insert(BufferArray name, Variable var) override;
  void remove(ui64 index, ui64 count = 1) override;
  void remove(BufferArray name) override;
  void remove(Path path) override;

  inline FileNodeVisitor getChild(ui64 index) const {return FileNodeVisitor(*this)(index);}
  inline FileNodeVisitor getChild(BufferArray name) const {return FileNodeVisitor(*this)(name);}
  inline FileNodeVisitor getChild(Path path) const {return FileNodeVisitor(*this)(path);}

  NodeVector findAll(Query query, NodeVector node_vector = NodeVector());
  FileNodeVisitor find(Query query);

  FileNodeVisitor& operator()(ui64 index) override {return stepInside(index);}
  FileNodeVisitor& operator()(BufferArray name) override {return stepInside(name);}
  FileNodeVisitor& operator()(Path path) override {return stepInside(path);}

  inline FileNodeVisitor operator[](ui64 index) const {return getChild(index);}
  inline FileNodeVisitor operator[](BufferArray name) const {return getChild(name);}
  inline FileNodeVisitor operator[](Path path) const {return getChild(path);}

  NodeIterator beginFrom(ui64 index);
  NodeIterator beginFrom(BufferArray name);
  NodeIterator begin();
  NodeIterator end();

  NodeVisitor& toRAMVisitor() = delete;
  FileNodeVisitor& toFileVisitor() = delete;
  Variable getVariableClone() = delete;
};

class FileNodeVisitor::NodeIterator {
  ui64 index = 0;

  ObjectDescriptor obj_descriptor = {};
  real_index name_block_index = 0;
  real_index name_shift = 0;
  real_index name_index = 0;
  ObjectNameLength name_block_descriptor = {};

  VarType container_type;
  FileNodeVisitor& node_visitor;

  friend class FileNodeVisitor;
  NodeIterator(FileNodeVisitor& node_visitor, bool is_end = false)
    : container_type(node_visitor.getType()),
      node_visitor(node_visitor) {
    switch (toTypeClass(container_type)) {
      case binom::VarTypeClass::primitive:
        index = is_end? 1 : 0;
      break;
      case binom::VarTypeClass::buffer_array:
      case binom::VarTypeClass::array:
        index = is_end? node_visitor.getElementCount() : 0;
      break;
      case binom::VarTypeClass::object:
        obj_descriptor = node_visitor.fmm
            .getNodeDataPart( node_visitor.node_index, 0, sizeof(ObjectDescriptor) )
            .get<ObjectDescriptor>(0);
        if(is_end) {
          index = obj_descriptor.index_count;
          name_index = 0;
          name_shift = obj_descriptor.name_block_size;
          name_block_descriptor = ObjectNameLength{};
        } else {
          name_block_descriptor = node_visitor.fmm
              .getNodeDataPart( node_visitor.node_index, sizeof(ObjectDescriptor), sizeof(ObjectNameLength) )
              .get<ObjectNameLength>(0);
        }
      break;
      case binom::VarTypeClass::invalid_type:
        throw Exception(ErrCode::binom_invalid_type);
    }
  }

  NodeIterator(FileNodeVisitor& node_visitor, ObjectElementPosition pos)
    : container_type(node_visitor.getType()),
      node_visitor(node_visitor) {
    switch (toTypeClass(container_type)) {
      case binom::VarTypeClass::object:
        obj_descriptor = pos.descriptor;
        index = pos.index;
        name_index = pos.name_index;
        name_shift = pos.name_shift;
        name_block_index = pos.name_block_index;
        name_block_descriptor = pos.name_block_descriptor;
      break;
      default:
        throw Exception(ErrCode::binom_invalid_type);
    }
  }

  NodeIterator(FileNodeVisitor& node_visitor, ui64 index)
    : index(index),
      container_type(node_visitor.getType()),
      node_visitor(node_visitor) {
    switch (toTypeClass(container_type)) {
      case binom::VarTypeClass::buffer_array:
      case binom::VarTypeClass::array: break;
      case binom::VarTypeClass::primitive:
        if(index == 0 || index == 1) break;
      default:
        throw Exception(ErrCode::binom_invalid_type);
    }
  }

public:
  NodeIterator(const NodeIterator& other)
    : index(other.index),
      obj_descriptor(other.obj_descriptor),
      name_block_index(other.name_block_index),
      name_shift(other.name_shift),
      name_index(other.name_index),
      name_block_descriptor(other.name_block_descriptor),
      container_type(other.container_type),
      node_visitor(other.node_visitor) {}

  NodeIterator(const NodeIterator&& other)
    : index(other.index),
      obj_descriptor(other.obj_descriptor),
      name_block_index(other.name_block_index),
      name_shift(other.name_shift),
      name_index(other.name_index),
      name_block_descriptor(other.name_block_descriptor),
      container_type(other.container_type),
      node_visitor(other.node_visitor) {}


  bool isBegin() { return !index; }

  bool isEnd() {
    switch (toTypeClass(container_type)) {
      case binom::VarTypeClass::primitive: return index == 1;
      case binom::VarTypeClass::buffer_array:
      case binom::VarTypeClass::array: return index == node_visitor.getElementCount();
      case binom::VarTypeClass::object: return index == obj_descriptor.index_count;
      case binom::VarTypeClass::invalid_type: return true;
    }
  }


  NodeIterator& operator--() {
    if(isBegin()) return *this;
    switch (toTypeClass(container_type)) {
      case VarTypeClass::primitive:
        index = 0;
      break;
      case VarTypeClass::array:
      case VarTypeClass::buffer_array:
        --index;
      break;
      case VarTypeClass::object: {
        --index;
        if(name_index) {
          --name_index;
          name_shift -= name_block_descriptor.name_length*toSize(name_block_descriptor.char_type);
        } else {
          --name_block_index;
          name_block_descriptor = node_visitor.fmm.getNodeDataPart(
                node_visitor.node_index,
                sizeof (ObjectDescriptor) +
                sizeof (ObjectNameLength)*name_block_index,
                sizeof (ObjectNameLength))
              .get<ObjectNameLength>(0);
          name_index = name_block_descriptor.name_count - 1;
          name_shift -= name_block_descriptor.name_length*toSize(name_block_descriptor.char_type);
        }
      } break;
      default: break;
    }
    return *this;
  }

  NodeIterator& operator++() {
    if(isEnd()) return *this;
    switch (toTypeClass(container_type)) {
      case VarTypeClass::primitive:
        index = 1;
      break;
      case VarTypeClass::array:
      case VarTypeClass::buffer_array:
        ++index;
      break;
      case VarTypeClass::object: {
        ++index;
        ++name_index;
        name_shift += name_block_descriptor.name_length*toSize(name_block_descriptor.char_type);
        if(name_block_descriptor.name_count == name_index) {
          if(obj_descriptor.index_count == index) {
            name_index = 0;
            name_block_descriptor = ObjectNameLength{};
          } else {
            name_index = 0;
            ++name_block_index;
            name_block_descriptor = node_visitor.fmm.getNodeDataPart(
                  node_visitor.node_index,
                  sizeof (ObjectDescriptor) +
                  sizeof (ObjectNameLength)*name_block_index,
                  sizeof (ObjectNameLength))
                .get<ObjectNameLength>(0);
          }
        }
      } break;
      default: break;
    }
    return *this;
  }

  inline NodeIterator& operator++(int) {return ++*this;}
  std::optional<BufferArray> getName();
  bool operator==(NodeIterator iterator) const {
    return node_visitor.node_index == iterator.node_visitor.node_index &&
           index == iterator.index;
  }
  bool operator!=(NodeIterator iterator) const {
    return node_visitor.node_index != iterator.node_visitor.node_index ||
           index != iterator.index;
  }
  FileNodeVisitor getParent() {return node_visitor;}
  FileNodeVisitor operator*() {
    if(isEnd()) return FileNodeVisitor(node_visitor.fmm, nullptr);
    switch (toTypeClass(container_type)) {
      case binom::VarTypeClass::primitive: return node_visitor;
      case binom::VarTypeClass::buffer_array:
      return FileNodeVisitor(node_visitor.fmm,
                             node_visitor.node_index,
                             index);
      case binom::VarTypeClass::array:
      return FileNodeVisitor(
            node_visitor.fmm,
            node_visitor.fmm.getNodeDataPart(
              node_visitor.node_index,
              sizeof (virtual_index)*index,
              sizeof (virtual_index))
            .get<virtual_index>(0) );
      case binom::VarTypeClass::object:
      return FileNodeVisitor(
            node_visitor.fmm,
            node_visitor.fmm.getNodeDataPart(
                    node_visitor.node_index,
                    sizeof (ObjectDescriptor) +
                    sizeof (ObjectNameLength)*obj_descriptor.length_element_count +
                    obj_descriptor.name_block_size +
                    sizeof (virtual_index)*index,
                    sizeof (virtual_index))
                  .get<virtual_index>(0),
            NamePosition{
              node_visitor.node_index,
              name_block_descriptor.char_type,
              name_block_descriptor.name_length,
              sizeof (ObjectDescriptor) +
              sizeof (ObjectNameLength)*obj_descriptor.length_element_count +
              name_shift
            });
      case binom::VarTypeClass::invalid_type:
      return FileNodeVisitor(node_visitor.fmm, nullptr);
    }



  }


};

}

#include "file_object_element_finder.h"

namespace binom {



//class FileNodeVisitor::NodeIterator {
//  union Data {
//    struct ObjectData {
//      ByteArray indexes;
//      ObjectDescriptor descriptor;
//      ByteArray name_lengths;
//      ByteArray names;
//      ObjectNameLength* name_length_it;
//      byte* name_it;
//      virtual_index* index_it;
//      ui64 name_count;

//      bool isEmpty() {return indexes.isEmpty();}

//      ObjectData() = default;
//      ObjectData(ByteArray data)
//        : indexes(std::move(data)),
//          descriptor(indexes.takeFront<ObjectDescriptor>()),
//          name_lengths(indexes.takeFront(descriptor.length_element_count * sizeof (ObjectNameLength))),
//          names(indexes.takeFront(descriptor.name_block_size)),
//          name_length_it(name_lengths.begin<ObjectNameLength>()),
//          name_it(names.begin()),
//          index_it(indexes.begin<virtual_index>()),
//          name_count(name_length_it->name_count) {}

//    } object_data;

//    struct ArrayData {
//      ByteArray indexes;
//      virtual_index* index_it = nullptr;

//      bool isEmpty() {return indexes.isEmpty();}

//      ArrayData() = default;
//      ArrayData(ByteArray indexes)
//        : indexes(std::move(indexes)), index_it(this->indexes.begin<virtual_index>()) {}
//    } array_data;

//    struct BufferArrayData {
//      const virtual_index node_index = 0;
//      const size_t size = 0;
//      real_index index = 0;
//    } buffer_array_data;

//    Data(VarTypeClass type_class, ByteArray data) {
//      switch (type_class) {
//        default: throw Exception(ErrCode::binom_invalid_type);
//        case binom::VarTypeClass::array:
//          if(data.isEmpty())new(&array_data) ArrayData();
//          else
//            new(&array_data) ArrayData(std::move(data));
//        return;
//        case binom::VarTypeClass::object:
//          if(data.isEmpty())
//            new(&object_data) ObjectData();
//          else
//            new(&object_data) ObjectData(std::move(data));
//        return;
//      }
//    }

//    Data(virtual_index node_index, size_t count, real_index index = 0)
//      : buffer_array_data({node_index, count, index}) {}
//    Data(Data&& other) {
//      std::memcpy(this, &other, sizeof (Data));
//      std::memset(&other, 0, sizeof (Data));
//    }
//    Data(const Data&) = delete;
//    ~Data(){}
//  };

//  FileMemoryManager& fmm;
//  virtual_index parent_node_index;
//  VarType container_type;
//  Data data;

//  friend class FileNodeVisitor;

//  NodeIterator(FileMemoryManager& fmm, virtual_index parent_node_index, VarType type, ByteArray data);
//  NodeIterator(FileMemoryManager& fmm,
//               virtual_index parent_node_index,
//               VarType type,
//               virtual_index node_index,
//               size_t count,
//               real_index index = 0);

//public:
//  NodeIterator(const NodeIterator& other) = delete;
//  NodeIterator(NodeIterator&& other);
//  ~NodeIterator();
//  NodeIterator& operator++();
//  inline NodeIterator& operator++(int) {return operator++();}
//  std::optional<BufferArray> getName();
//  bool isEnd();
//  inline bool operator==(decltype (nullptr)) {return isEnd();}
//  inline bool operator!=(decltype (nullptr)) {return !isEnd();}
//  FileNodeVisitor getParent() {return FileNodeVisitor(fmm, parent_node_index);}
//  FileNodeVisitor operator*();

//};


}


#endif // FILE_NODE_VISITOR_H
