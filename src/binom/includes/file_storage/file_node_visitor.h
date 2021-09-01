#ifndef FILE_NODE_VISITOR_H
#define FILE_NODE_VISITOR_H

#include "file_memory_manager.h"
#include "../utils/node_visitor_base.h"

namespace binom {


class FileNodeVisitor : public NodeVisitorBase {
  typedef RWSyncMap::ScopedRWGuard ScopedRWGuard;
  typedef RWSyncMap::RWGuard RWGuard;
  typedef RWSyncMap::LockType LockType;

  static constexpr virtual_index null_index = 0xFFFFFFFFFFFFFFFF;

  FileMemoryManager& fmm;
  virtual_index node_index = 0;
  real_index index = null_index;
  mutable RWGuard current_rwg;

  FileNodeVisitor(FileMemoryManager& fmm,
           virtual_index node_index = 0,
           real_index index = null_index)
    : fmm(fmm),
      node_index(node_index),
      index(index),
      current_rwg(fmm.getRWGuard(node_index))
  {}

  friend class FileStorage;

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

  void setNull() {
    node_index = null_index;
    index = null_index;
  }

public:

  class NodeIterator;

  FileNodeVisitor(const FileNodeVisitor& other)
    : fmm(other.fmm),
      node_index(other.node_index),
      index(other.index),
      current_rwg(fmm.getRWGuard(node_index))
  {}

  ScopedRWGuard getScopedRWGuard(LockType lock_type = LockType::unlocked) const {return ScopedRWGuard(current_rwg, lock_type);}

  inline FileNodeVisitor& operator=(FileNodeVisitor& other) {this->~FileNodeVisitor(); return *new(this) FileNodeVisitor(other);}
  inline FileNodeVisitor& operator=(virtual_index node_index) {this->~FileNodeVisitor(); return *new(this) FileNodeVisitor(fmm, node_index);}

  VarType getType() const override {auto lk = getScopedRWGuard(LockType::shared_lock); return getDescriptor().type;}
  VisitorType getVisitorType() const override {return VisitorType::file_storage_visitor;}
  inline virtual_index getNodeIndex() const {return node_index;}

  inline bool isNull() const override {return node_index == null_index;}
  inline bool isValueRef() const override {return index != null_index;}
  ui64 getElementCount() const override;

  FileNodeVisitor& stepInside(ui64 index) override;
  FileNodeVisitor& stepInside(BufferArray name) override;
  FileNodeVisitor& stepInside(Path path) override;

  Variable getVariable() const;
  inline Variable getVariable(ui64 index) const {return getChild(index).getVariable();}
  inline Variable getVariable(BufferArray name) const {return getChild(name).getVariable();}
  inline Variable getVariable(Path path) const {return getChild(path).getVariable();}

  void setVariable(Variable var) override;
  void pushBack(Variable var) override;
  void pushFront(Variable var) override;
  void insert(ui64 index, Variable var) override;
  void insert(BufferArray name, Variable var) override;
  void remove(ui64 index, ui64 count = 0) override;
  void remove(BufferArray name) override;
  void remove(Path path) override;

  inline FileNodeVisitor getChild(ui64 index) const {return FileNodeVisitor(*this)(index);}
  inline FileNodeVisitor getChild(BufferArray name) const {return FileNodeVisitor(*this)(name);}
  inline FileNodeVisitor getChild(Path path) const {return FileNodeVisitor(*this)(path);}

  NodeVector findAll(Query query, NodeVector node_vector = NodeVector()); /* Not_Impl */
  FileNodeVisitor find(Query query); /* Not_Impl */

  FileNodeVisitor& operator()(ui64 index) override {return stepInside(index);}
  FileNodeVisitor& operator()(BufferArray name) override {return stepInside(name);}
  FileNodeVisitor& operator()(Path path) override {return stepInside(path);}

  inline FileNodeVisitor operator[](ui64 index) const {return getChild(index);}
  inline FileNodeVisitor operator[](BufferArray name) const {return getChild(name);}
  inline FileNodeVisitor operator[](Path path) const {return getChild(path);}

  NodeIterator begin();
  static inline decltype(nullptr) end() {return nullptr;}

  NodeVisitor& toRAMVisitor() = delete;
  FileNodeVisitor& toFileVisitor() = delete;
};


class FileNodeVisitor::NodeIterator {
  FileMemoryManager& fmm;
  VarType container_type;

  union Data {
    struct ObjectData {
      ByteArray indexes;
      ObjectDescriptor descriptor;
      ByteArray name_lengths;
      ByteArray names;
      ObjectNameLength* name_length_it;
      ObjectNameLength* name_length_end;
      byte* name_it;
      byte* name_end;
      virtual_index* index_it;

      ObjectData() = default;
      ObjectData(ByteArray data)
        : indexes(std::move(data)),
          descriptor(indexes.takeFront<ObjectDescriptor>()),
          name_lengths(indexes.takeFront(descriptor.length_element_count * sizeof (ObjectNameLength))),
          names(indexes.takeFront(descriptor.name_block_size)),
          name_length_it(name_lengths.begin<ObjectNameLength>()),
          name_length_end(name_lengths.end<ObjectNameLength>()),
          name_it(names.begin()),
          name_end(names.end()),
          index_it(indexes.begin<virtual_index>()) {}

    } object_data;

    struct ArrayData {
      ByteArray indexes;
      virtual_index* index_it;
      ArrayData(ByteArray indexes)
        : indexes(std::move(indexes)), index_it(indexes.begin<virtual_index>()) {}
    } array_data;

    struct BufferArrayData {
      const virtual_index node_index = 0;
      const size_t size = 0;
      real_index index = 0;
    } buffer_array_data;

    Data(VarTypeClass type_class, ByteArray data) {
      switch (type_class) {
        default: throw Exception(ErrCode::binom_invalid_type);
        case binom::VarTypeClass::array:
          new(&array_data) ArrayData(std::move(data));
        return;
        case binom::VarTypeClass::object:
          new(&object_data) ObjectData(std::move(object_data));
        return;
      }
    }

    Data(virtual_index node_index, size_t count, real_index index = 0)
      : buffer_array_data({node_index, count, index}) {}
    Data(Data&& other) {
      std::memcpy(this, &other, sizeof (Data));
      std::memset(&other, 0, sizeof (Data));
    }
    Data(const Data&) = delete;
    ~Data(){}
  } data;

  friend class FileNodeVisitor;

  NodeIterator(FileMemoryManager& fmm, VarType type, ByteArray data)
    : fmm(fmm), container_type(type), data(toTypeClass(type), std::move(data)) {}
  NodeIterator(FileMemoryManager& fmm, VarType type, virtual_index node_index, size_t count, real_index index = 0)
    : fmm(fmm), container_type(type), data(node_index, count, index) {}

public:
  NodeIterator(NodeIterator&& other)
    : fmm(other.fmm), container_type(other.container_type), data(std::move(other.data)) {}

  NodeIterator& operator++() {
    switch (toTypeClass(container_type)) {
      default: throw Exception(ErrCode::binom_invalid_type);
      case binom::VarTypeClass::buffer_array:
        ++data.buffer_array_data.index;
      break;
      case binom::VarTypeClass::array:
        ++data.array_data.index_it;
      break;
      case binom::VarTypeClass::object:
        /*
    byte* name_it = names.begin();
    virtual_index* index_it = indexes.begin<virtual_index>();
    for(ObjectNameLength* it = name_lengths.begin<ObjectNameLength>(),
        * end = name_lengths.end<ObjectNameLength>();
        it != end; ++it) {
      ui64 name_count = it->name_count;
      while (name_count) {
        handler({it->char_type, it->name_length, name_it, *index_it});
        --name_count;
        name_it += it->name_length * toSize(it->char_type);
        ++index_it;
      }
    }
*/
// TODO
      break;
    }
    return *this;
  }
  NodeIterator& operator++(int);

  bool isEnd();
  inline bool operator==(decltype (nullptr)) {return isEnd();}

//  FileNodeVisitor operator*() {return FileNodeVisitor(fmm, *index_it);}

};


}


#endif // FILE_NODE_VISITOR_H
