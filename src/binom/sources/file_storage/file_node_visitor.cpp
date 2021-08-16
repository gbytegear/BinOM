#include "binom/includes/file_storage/file_node_visitor.h"

using namespace binom;

class ObjectElementFinder {
  ByteArray indexes;
  ObjectDescriptor descriptor;
  ByteArray name_lengths;
  ByteArray names;
  ObjectNameLength* name_length_it;
  ObjectNameLength* const name_length_end;
  byte* name_it;
  byte* const name_end;
  virtual_index* index_it;
public:
  ObjectElementFinder(ByteArray data)
    : indexes(data),
      descriptor(indexes.takeFront<ObjectDescriptor>()),
      name_lengths(indexes.takeFront(descriptor.length_element_count * sizeof (ObjectNameLength))),
      names(indexes.takeFront(descriptor.name_block_size)),
      name_length_it(name_lengths.begin<ObjectNameLength>()),
      name_length_end(name_lengths.end<ObjectNameLength>()),
      name_it(names.begin()),
      name_end(names.end()),
      index_it(indexes.begin<virtual_index>()) {}

  ObjectElementFinder& dropPosition() {
    name_length_it = name_lengths.begin<ObjectNameLength>();
    name_it = names.begin();
    index_it = indexes.begin<virtual_index>();
    return *this;
  }

  // O(length_element_count)
  ObjectElementFinder& findBlock(ValType type, block_size name_length) {
    if(name_length_it != name_lengths.begin<ObjectNameLength>())
      dropPosition();

    for(;name_length_it != name_length_end; ++name_length_it) {
      if(name_length_it->char_type != type || name_length_it->name_length != name_length) {
        index_it += name_length_it->name_count;
        name_it += toSize(name_length_it->char_type) * name_length_it->name_length * name_length_it->name_count;
      } else break;
    }
    return *this;
  }

  inline bool isNameBlockFinded() const {return name_length_it != name_length_end;}

  // O(log(name_count))
  ObjectElementFinder& findNameInBlock(void* name) {
    const ui8 char_size = toSize(name_length_it->char_type);
    const i64 name_count = name_length_it->name_count;
    const ui64 name_byte_length = name_length_it->name_length*char_size;

    i64 middle = 0;
    i64 left = 0;
    i64 right = name_length_it->name_count;

    while(true) {
      middle = (left + right) / 2;

      if(left > right || middle > name_count) {
        middle = -1;
        break;
      }

      int cmp = memcmp(name_it + middle*name_byte_length, name, name_byte_length);

      if(cmp > 0) right = middle - 1;
      elif(cmp < 0) left = middle + 1;
      else break;
    }

    if(middle == -1)
      name_it = name_end;
    else {
      name_it += middle*name_byte_length;
      index_it += middle;
    }
    return *this;
  }

  bool isNameFinded() {return name_it != name_end;}

  BufferArray getName() const {return BufferArray(name_length_it->char_type, name_it, name_length_it->name_length);}
  virtual_index getNodeIndex() const {return *index_it;}
  Object buildObject() {

    for(ObjectNameLength* it = name_lengths.begin<ObjectNameLength>(),
        * end = name_lengths.end<ObjectNameLength>();
        it != end; ++it) {
      ui64 name_count = it->name_count;
      const ui64 name_size = it->name_length;
      const ValType char_type = it->char_type;
      // TODO
    }
  }

};


Variable FileNodeVisitor::buildVariable(virtual_index node_index) const {
  RWGuard rwg = fmm.getRWGuard(node_index);
  ScopedRWGuard lk(rwg, LockType::shared_lock);
  NodeDescriptor descriptor = fmm.getNodeDescriptor(node_index);
  switch (toTypeClass(descriptor.type)) {
    case binom::VarTypeClass::primitive: return buildPrimitive(node_index, &descriptor, &lk);
    case binom::VarTypeClass::buffer_array: return buildBufferArray(node_index, &descriptor, &lk);
    case binom::VarTypeClass::array: return buildArray(node_index, &descriptor, &lk);
    case binom::VarTypeClass::object: return buildObject(node_index, &descriptor, &lk);
    case binom::VarTypeClass::invalid_type:
      throw Exception(ErrCode::binom_invalid_type);
  }
}

Primitive FileNodeVisitor::buildPrimitive(virtual_index node_index, const NodeDescriptor* _descriptor, FileNodeVisitor::ScopedRWGuard* lk) const {
  std::unique_ptr<RWGuard> rwg_ptr;
  ScopedRWGuard cur_lk(lk? *lk : *(rwg_ptr = std::make_unique<RWGuard>(fmm.getRWGuard(node_index))), LockType::shared_lock);
  NodeDescriptor descriptor = _descriptor? *_descriptor : fmm.getNodeDescriptor(node_index);
  if(toTypeClass(descriptor.type) != VarTypeClass::primitive)
    throw Exception(ErrCode::binom_invalid_type);
  return Primitive(toValueType(descriptor.type), &descriptor.index);
}

BufferArray FileNodeVisitor::buildBufferArray(virtual_index node_index, const NodeDescriptor* _descriptor, FileNodeVisitor::ScopedRWGuard* lk) const {
  std::unique_ptr<RWGuard> rwg_ptr;
  ScopedRWGuard cur_lk(lk? *lk : *(rwg_ptr = std::make_unique<RWGuard>(fmm.getRWGuard(node_index))), LockType::shared_lock);
  NodeDescriptor descriptor = _descriptor? *_descriptor : fmm.getNodeDescriptor(node_index);
  if(toTypeClass(descriptor.type) != VarTypeClass::buffer_array)
    throw Exception(ErrCode::binom_invalid_type);
  ByteArray data = fmm.getNodeData(descriptor);
  data.pushFront<ui64>(data.length());
  data.pushFront(descriptor.type);
  void* variable = data.unfree();
  return *reinterpret_cast<BufferArray*>(&variable);
}

Array FileNodeVisitor::buildArray(virtual_index node_index, const NodeDescriptor* _descriptor, FileNodeVisitor::ScopedRWGuard* lk) const {
  std::unique_ptr<RWGuard> rwg_ptr;
  ScopedRWGuard cur_lk(lk? *lk : *(rwg_ptr = std::make_unique<RWGuard>(fmm.getRWGuard(node_index))), LockType::shared_lock);
  NodeDescriptor descriptor = _descriptor? *_descriptor : fmm.getNodeDescriptor(node_index);
  if(toTypeClass(descriptor.type) != VarTypeClass::array)
    throw Exception(ErrCode::binom_invalid_type);
  ByteArray data = fmm.getNodeData(descriptor);
  for(auto it = data.begin<virtual_index>(), end = data.end<virtual_index>(); it != end; ++it)
    new(it) Variable(buildVariable(*it));
  data.pushFront<ui64>(data.length());
  data.pushFront(descriptor.type);
  void* variable = data.unfree();
  return *reinterpret_cast<Array*>(&variable);
}

Object FileNodeVisitor::buildObject(virtual_index node_index, const NodeDescriptor* _descriptor, FileNodeVisitor::ScopedRWGuard* lk) const {
  std::unique_ptr<RWGuard> rwg_ptr;
  ScopedRWGuard cur_lk(lk? *lk : *(rwg_ptr = std::make_unique<RWGuard>(fmm.getRWGuard(node_index))), LockType::shared_lock);
  NodeDescriptor descriptor = _descriptor? *_descriptor : fmm.getNodeDescriptor(node_index);
  if(toTypeClass(descriptor.type) != VarTypeClass::object)
    throw Exception(ErrCode::binom_invalid_type);
  ObjectElementFinder finder(fmm.getNodeData(descriptor));
}

FileNodeVisitor& FileNodeVisitor::stepInside(ui64 index) {
  auto lk = getScopedRWGuard(LockType::shared_lock);

  NodeDescriptor descriptor = getDescriptor();
  switch (toTypeClass(descriptor.type)) {
    case binom::VarTypeClass::primitive:
    case binom::VarTypeClass::object:
    case binom::VarTypeClass::invalid_type:
      throw Exception(ErrCode::binom_invalid_type);

    case binom::VarTypeClass::buffer_array:
      if(descriptor.size / toSize(toValueType(descriptor.type)) < index)
        throw Exception(ErrCode::binom_out_of_range);
      this->index = index;
    break;
    case binom::VarTypeClass::array:
      if(descriptor.size / sizeof (virtual_index) < index)
        throw Exception(ErrCode::binom_out_of_range);
      node_index = fmm.getNodeDataPart(descriptor, index * sizeof (virtual_index), sizeof(virtual_index)).first<virtual_index>();
    break;
  }

  current_rwg = fmm.getRWGuard(node_index);
  return *this;
}

FileNodeVisitor& FileNodeVisitor::stepInside(BufferArray name) {
  auto lk = getScopedRWGuard(LockType::shared_lock);

  // Validation
  NodeDescriptor descriptor = getDescriptor();
  if(descriptor.type != VarType::object)
    throw Exception(ErrCode::binom_invalid_type);
  elif(descriptor.size == 0)
    throw Exception(ErrCode::binom_out_of_range);

  ObjectElementFinder finder(fmm.getNodeData(descriptor));
  if(!finder.findBlock(name.getValType(), name.getMemberCount()).isNameBlockFinded())
    throw Exception(ErrCode::binom_out_of_range);
  if(!finder.findNameInBlock(name.getDataPointer()).isNameFinded())
    throw Exception(ErrCode::binom_out_of_range);
  return *this;
}

FileNodeVisitor& FileNodeVisitor::stepInside(Path path) {
  for(const Path::PathNode& path_node : path)
    switch (path_node.type()) {
      case binom::PathNodeType::index: stepInside(path_node.index()); continue;
      case binom::PathNodeType::name: stepInside(path_node.name()); continue;
    }
  return *this;
}
