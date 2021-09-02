#include "binom/includes/file_storage/file_node_visitor.h"

#include <functional>

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

  struct ObjectElement{
    ValType type;
    ui64 name_size;
    const void* name;
    virtual_index index;
  };

  ObjectElementFinder(ByteArray data)
    : indexes(std::move(data)),
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
      if(name_length_it->char_type < type || name_length_it->name_length < name_length) {
        index_it += name_length_it->name_count;
        name_it += toSize(name_length_it->char_type) * name_length_it->name_length * name_length_it->name_count;
      } elif(name_length_it->char_type == type && name_length_it->name_length == name_length) break;
      else {
        name_length_it = name_length_end;
        break;
      }
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

  ui64 getElementCount() const {return descriptor.index_count;}

  void foreach(std::function<void(ObjectElement)> handler) {
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
  }

  void insert(BufferArray name, virtual_index node_index) {
    if(name_length_it != name_lengths.begin<ObjectNameLength>())
      dropPosition();

    const ValType type = name.getValType();
    const ui64 name_length = name.getMemberCount();
    for(;name_length_it != name_length_end; ++name_length_it) {
      if(name_length_it->char_type < type || name_length_it->name_length < name_length) {
        index_it += name_length_it->name_count;
        name_it += toSize(name_length_it->char_type) * name_length_it->name_length * name_length_it->name_count;
      } elif(name_length_it->char_type == type && name_length_it->name_length == name_length) break;
      else { // Insertion between name blocks
        name_lengths.insert<ObjectNameLength>(name_length_it - name_lengths.begin<ObjectNameLength>(), 0, {type, name_length, 1});
        ++descriptor.length_element_count;
        names.insert(name_it - names.begin(), name.toByteArray());
        descriptor.name_block_size += name.getDataSize();
        indexes.insert<virtual_index>(index_it - indexes.begin<virtual_index>(), 0, node_index);
        ++descriptor.index_count;

        return;
      }
    }

    if (name_length_it == name_length_end) { // Insertion at the end of Object
      name_lengths.pushBack<ObjectNameLength>({type, name_length, 1});
      ++descriptor.length_element_count;
      names.pushBack(name.toByteArray());
      descriptor.name_block_size = names.length();
      indexes.pushBack<virtual_index>(node_index);
      ++descriptor.index_count;
      return;
    }

    const i64 name_count = name_length_it->name_count;
    const i64 name_byte_length = name_length * toSize(type);
    i64 middle = 0;
    i64 left = 0;
    i64 right = name_length_it->name_count;

    while(true) {
      middle = (left + right) / 2;

      if(left > right || middle > name_count) break;

      int cmp = memcmp(name_it + middle*name_byte_length, name.getDataPointer(), name_byte_length);

      if(cmp > 0) right = middle - 1;
      elif(cmp < 0) left = middle + 1;
      else {
        dropPosition();
        throw Exception(ErrCode::binom_object_key_error);
      }
    }

    for(;(middle < name_count)
              ? memcmp(name_it + middle*name_byte_length, name.getDataPointer(), name_byte_length) < 0
              : false;
              ++middle);

    name_it += middle*name_byte_length;
    names.insert(name_it - names.begin(), name.toByteArray());
    descriptor.name_block_size += name.getDataSize();
    index_it += middle;
    indexes.insert<virtual_index>(index_it - indexes.begin<virtual_index>(), 0, node_index);
    ++descriptor.index_count;
    ++name_length_it->name_count;
  }

  bool remove(BufferArray name) {
    if(name_length_it != name_lengths.begin<ObjectNameLength>())
      dropPosition();

    const ValType type = name.getValType();
    const ui64 name_length = name.getMemberCount();
    for(;name_length_it != name_length_end; ++name_length_it) {
      if(name_length_it->char_type < type || name_length_it->name_length < name_length) {
        index_it += name_length_it->name_count;
        name_it += toSize(name_length_it->char_type) * name_length_it->name_length * name_length_it->name_count;
      } elif(name_length_it->char_type == type && name_length_it->name_length == name_length) break;
      else return false;
    }

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

      int cmp = memcmp(name_it + middle*name_byte_length, name.getDataPointer(), name_byte_length);

      if(cmp > 0) right = middle - 1;
      elif(cmp < 0) left = middle + 1;
      else break;
    }

    if(middle == -1) {
      name_it = name_end;
      return false;
    } else {
      name_it += middle*name_byte_length;
      index_it += middle;
    }

    names.remove(name_it - names.begin(), name_byte_length);
    descriptor.name_block_size += name.getDataSize();
    indexes.remove<virtual_index>(index_it - indexes.begin<virtual_index>(), 0);
    --descriptor.index_count;
    if(!--name_length_it->name_count) {
      name_lengths.remove<ObjectNameLength>(name_length_it - name_lengths.begin<ObjectNameLength>(), 0);
      --descriptor.length_element_count;
    }

    return true;
  }

  ByteArray getNodeData() {
    return ByteArray{
      ByteArray(&descriptor, sizeof (ObjectDescriptor)),
      name_lengths,
      names,
      indexes
    };
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
  data.pushFront<ui64>(data.length() / toSize(toValueType(descriptor.type)));
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
  data.pushFront<ui64>(data.length() / sizeof (void*));
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
  ByteArray data(9 + finder.getElementCount()*sizeof(void*)*2);
  data.get<VarType>(0) = VarType::object; // Set type
  data.get<ui64>(0, 1) = finder.getElementCount(); // Set length
  {
    NamedVariable* object_element_iterator = data.begin<NamedVariable>(9);
    finder.foreach([this, &object_element_iterator](ObjectElementFinder::ObjectElement element) {
      new(&object_element_iterator->name) BufferArray(element.type, element.name, element.name_size);
      new(&object_element_iterator->variable) Variable(buildVariable(element.index));
      ++object_element_iterator;
    });
  }
  void* variable = data.unfree();
  return *reinterpret_cast<Object*>(&variable);
}

virtual_index FileNodeVisitor::createVariable(Variable variable) {
  switch (variable.typeClass()) {
    case binom::VarTypeClass::primitive: return createPrimitive(std::move(variable.toPrimitive()));
    case binom::VarTypeClass::buffer_array: return createBufferArray(std::move(variable.toBufferArray()));
    case binom::VarTypeClass::array: return createArray(std::move(variable.toArray()));
    case binom::VarTypeClass::object: return createObject(std::move(variable.toObject()));
    case binom::VarTypeClass::invalid_type: throw Exception(ErrCode::binom_invalid_type);
  }
}

virtual_index FileNodeVisitor::createPrimitive(Primitive primitive) {
  return fmm.createNode(primitive.getType(), ByteArray(primitive.getDataPtr(), toSize(primitive.getValType())));
}

virtual_index FileNodeVisitor::createBufferArray(BufferArray buffer_array) {
  return fmm.createNode(buffer_array.getType(), buffer_array.toByteArray());
}

virtual_index FileNodeVisitor::createArray(Array array) {
  if(array.isEmpty()) return fmm.createNode(VarType::array, ByteArray());
  ByteArray data(array.getMemberCount()*sizeof(virtual_index));
  virtual_index* it = data.begin<virtual_index>();
  for(Variable& variable : array) {
    *it = createVariable(std::move(variable));
    ++it;
  }
  return fmm.createNode(VarType::array, std::move(data));
}

virtual_index FileNodeVisitor::createObject(Object object) {
  if(object.isEmpty()) return fmm.createNode(VarType::object, ByteArray());
  ByteArray
      name_length_block,
      name_block,
      index_block(object.getMemberCount()*sizeof(virtual_index));
  virtual_index* index_it = index_block.begin<virtual_index>();
  ObjectNameLength length;
  for(NamedVariable& named_variable : object) {
    if(length.char_type != named_variable.name.getValType() ||
       length.name_length != named_variable.name.getMemberCount()) {
      if(length.char_type != ValType::invalid_type)
        name_length_block += length;
      new(&length) ObjectNameLength{named_variable.name.getValType(), named_variable.name.getMemberCount()};
    }
    name_block += named_variable.name.toByteArray();
    *index_it = createVariable(std::move(named_variable.variable));
    ++index_it;
    ++length.name_count;
  }
  name_length_block += length;
  ObjectDescriptor descriptor{
    name_length_block.length<ObjectNameLength>(),
    name_block.length(),
    index_block.length<virtual_index>()
  };
  return fmm.createNode(VarType::object,
                        ByteArray{
                          ByteArray(&descriptor, sizeof (ObjectDescriptor)),
                          std::move(name_length_block),
                          std::move(name_block),
                          std::move(index_block)
                        });
}

ByteArray FileNodeVisitor::getContainedNodeIndexes(virtual_index node_index) {
  std::unique_ptr<RWGuard> rwg_ptr;
  ScopedRWGuard cur_lk((node_index == this->node_index)? current_rwg : *(rwg_ptr = std::make_unique<RWGuard>(fmm.getRWGuard(node_index))), LockType::shared_lock);
  NodeDescriptor descriptor = fmm.getNodeDescriptor(node_index);
  switch (toTypeClass(descriptor.type)) {
    case binom::VarTypeClass::primitive:
    case binom::VarTypeClass::buffer_array: return ByteArray();
    case binom::VarTypeClass::array: {
      ByteArray indexes = fmm.getNodeData(descriptor);
      ByteArray add_indexes;
      for(virtual_index* index_it = indexes.begin<virtual_index>(),
                       * index_end = indexes.end<virtual_index>();
          index_it != index_end; ++index_it)
        add_indexes += getContainedNodeIndexes(*index_it);
      indexes += std::move(add_indexes);
      return indexes;
    }
    case binom::VarTypeClass::object: {
      ObjectDescriptor object_descriptor = fmm.getNodeDataPart(descriptor, 0, sizeof (ObjectDescriptor)).first<ObjectDescriptor>();
      ByteArray indexes = fmm.getNodeDataPart(descriptor,
                                              sizeof (ObjectDescriptor) +
                                              object_descriptor.length_element_count*sizeof (ObjectNameLength) +
                                              object_descriptor.name_block_size,
                                              object_descriptor.index_count * sizeof (virtual_index)
                                              );
      ByteArray add_indexes;
      for(virtual_index* index_it = indexes.begin<virtual_index>(),
                       * index_end = indexes.end<virtual_index>();
          index_it != index_end; ++index_it)
        add_indexes += getContainedNodeIndexes(*index_it);
      indexes += std::move(add_indexes);
      return indexes;
    }
    case binom::VarTypeClass::invalid_type:
      return ByteArray();
  }
}

VarType FileNodeVisitor::getType() const {
  auto lk = getScopedRWGuard(LockType::shared_lock);
  NodeDescriptor descriptor = getDescriptor();
  if(toTypeClass(descriptor.type) == VarTypeClass::buffer_array && isValueRef())
    return toVarType(toValueType(descriptor.type));
  else return getDescriptor().type;
}

ui64 FileNodeVisitor::getElementCount() const {
  auto lk = getScopedRWGuard(LockType::shared_lock);

  NodeDescriptor descriptor = getDescriptor();
  switch (toTypeClass(descriptor.type)) {
    default:
    case binom::VarTypeClass::invalid_type: return 0;
    case binom::VarTypeClass::primitive: return 1;
    case binom::VarTypeClass::buffer_array:
      if(isValueRef()) return 1;
      return descriptor.size / toSize(toValueType(descriptor.type));
    case binom::VarTypeClass::array: return descriptor.size / sizeof (virtual_index);
    case binom::VarTypeClass::object: {
      ObjectDescriptor obj_desriptor = fmm.getNodeDataPart(descriptor, 0, sizeof (ObjectDescriptor)).get<ObjectDescriptor>(0);
      return obj_desriptor.index_count;
    }
  }
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
      if(isValueRef())
        throw Exception(ErrCode::binom_invalid_type);
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

  node_index = finder.getNodeIndex();
  current_rwg = fmm.getRWGuard(node_index);
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

Variable FileNodeVisitor::getVariable() const {
  if(isValueRef()) {
    ScopedRWGuard lk(current_rwg, LockType::shared_lock);
    NodeDescriptor descriptor = fmm.getNodeDescriptor(node_index);
    ByteArray data = fmm.getNodeDataPart(descriptor,
                                         index*toSize(toValueType(descriptor.type)),
                                         toSize(toValueType(descriptor.type)));
    data.pushFront(descriptor.type);
    void* variable = data.unfree();
    return *reinterpret_cast<Variable*>(&variable);
  } else return buildVariable(node_index);
}

void FileNodeVisitor::setVariable(Variable var) {
  ScopedRWGuard lk(current_rwg, LockType::unique_lock);
  ByteArray data;

  if(isValueRef()) {
    if(var.typeClass() != VarTypeClass::primitive)
      throw Exception(ErrCode::binom_invalid_type);
    NodeDescriptor descriptor = getDescriptor();
    if(toTypeClass(descriptor.type) != VarTypeClass::buffer_array)
      throw Exception(ErrCode::binom_invalid_type);
    ByteArray data;
    switch (toValueType(descriptor.type)) {
      case binom::ValType::byte:
        data.pushBack<ui8>(var.getValue().asUi8());
      break;
      case binom::ValType::word:
        data.pushBack<ui16>(var.getValue().asUi16());
      break;
      case binom::ValType::dword:
        data.pushBack<ui32>(var.getValue().asUi32());
      break;
      case binom::ValType::qword:
        data.pushBack<ui64>(var.getValue().asUi64());
      break;
      default: throw Exception(ErrCode::binom_invalid_type);
    }
    fmm.updateNodeDataPart(node_index, index * toSize(toValueType(descriptor.type)), std::move(data), &descriptor);
    return;
  }

  // Create member nodes & construct data of current node
  switch (var.typeClass()) {
    case binom::VarTypeClass::primitive:
      data = ByteArray(var.toPrimitive().getDataPtr(), var.toPrimitive().getMemberSize());
    break;
    case binom::VarTypeClass::buffer_array:
      data = var.toBufferArray().toByteArray();
    break;
    case binom::VarTypeClass::array:
      for(Variable& element : var.toArray()) {
        data.pushBack<virtual_index>(createVariable(element));
      }
    break;
    case binom::VarTypeClass::object: {
      ByteArray
          name_length_block,
          name_block,
          index_block(var.toObject().getMemberCount()*sizeof(virtual_index));
      virtual_index* index_it = index_block.begin<virtual_index>();
      ObjectNameLength length;
      for(NamedVariable& named_variable : var.toObject()) {
        if(length.char_type != named_variable.name.getValType() ||
           length.name_length != named_variable.name.getMemberCount()) {
          if(length.char_type != ValType::invalid_type)
            name_length_block += length;
          new(&length) ObjectNameLength{named_variable.name.getValType(), named_variable.name.getMemberCount()};
        }
        name_block += named_variable.name.toByteArray();
        *index_it = createVariable(std::move(named_variable.variable));
        ++index_it;
        ++length.name_count;
      }
      name_length_block += length;
      ObjectDescriptor descriptor{
        name_length_block.length<ObjectNameLength>(),
        name_block.length(),
        index_block.length<virtual_index>()
      };

      data = ByteArray{
             ByteArray(&descriptor, sizeof (ObjectDescriptor)),
             std::move(name_length_block),
             std::move(name_block),
             std::move(index_block)
      };

    } break;
    case binom::VarTypeClass::invalid_type:
      throw Exception(ErrCode::binom_invalid_type);
  }

  // Delete member nodes of current node
  ByteArray indexes = getContainedNodeIndexes(node_index);
  for(virtual_index* index_it = indexes.begin<virtual_index>(),
                   * index_end = indexes.end<virtual_index>();
      index_it != index_end; ++index_it)
    fmm.removeNode(*index_it);

  // Update node
  fmm.updateNode(node_index, var.type(), std::move(data));
}

void FileNodeVisitor::pushBack(Variable var) {
  ScopedRWGuard lk(current_rwg, LockType::unique_lock);
  NodeDescriptor descriptor = getDescriptor();
  ByteArray data;

  switch (toTypeClass(descriptor.type)) {
    default: throw Exception(ErrCode::binom_invalid_type);

    case binom::VarTypeClass::buffer_array: {
      data = fmm.getNodeData(descriptor);
      switch (var.typeClass()) {
        default: throw Exception(ErrCode::binom_invalid_type);

        case VarTypeClass::primitive: {
          switch (toValueType(descriptor.type)) {
            case binom::ValType::byte: data.pushBack<ui8>(var.toPrimitive().getValue().asUi8()); break;
            case binom::ValType::word: data.pushBack<ui16>(var.toPrimitive().getValue().asUi16()); break;
            case binom::ValType::dword: data.pushBack<ui32>(var.toPrimitive().getValue().asUi32()); break;
            case binom::ValType::qword: data.pushBack<ui64>(var.toPrimitive().getValue().asUi64()); break;
            case binom::ValType::invalid_type:
            break;
          }
        } break;

        case VarTypeClass::buffer_array: {
           ByteArray::iterator it = data.addSize(toSize(toValueType(descriptor.type)) * var.toBufferArray().getMemberCount());
           switch (toValueType(descriptor.type)) {
             case binom::ValType::byte: for(ValueRef val_ref : var.toBufferArray()) {*reinterpret_cast<ui8*>(it) = val_ref.asUi8(); ++it;} break;
             case binom::ValType::word: for(ValueRef val_ref : var.toBufferArray()) {*reinterpret_cast<ui16*>(it) = val_ref.asUi16(); it += 2;} break;
             case binom::ValType::dword: for(ValueRef val_ref : var.toBufferArray()) {*reinterpret_cast<ui32*>(it) = val_ref.asUi32(); it += 4;} break;
             case binom::ValType::qword: for(ValueRef val_ref : var.toBufferArray()) {*reinterpret_cast<ui64*>(it) = val_ref.asUi64(); it += 8;} break;
             case binom::ValType::invalid_type:
             break;
           }
        } break;
      }
    } break;

    case binom::VarTypeClass::array: {
      data = fmm.getNodeData(descriptor);
      data.pushBack<virtual_index>(createVariable(var));
    } break;
  }

  fmm.updateNode(node_index, descriptor.type, data, &descriptor);
}

void FileNodeVisitor::pushFront(Variable var) {
  ScopedRWGuard lk(current_rwg, LockType::unique_lock);
  NodeDescriptor descriptor = getDescriptor();
  ByteArray data;

  switch (toTypeClass(descriptor.type)) {
    default: throw Exception(ErrCode::binom_invalid_type);

    case binom::VarTypeClass::buffer_array: {
      data = fmm.getNodeData(descriptor);
      switch (var.typeClass()) {
        default: throw Exception(ErrCode::binom_invalid_type);

        case VarTypeClass::primitive: {
          switch (toValueType(descriptor.type)) {
            case binom::ValType::byte: data.pushFront<ui8>(var.toPrimitive().getValue().asUi8()); break;
            case binom::ValType::word: data.pushFront<ui16>(var.toPrimitive().getValue().asUi16()); break;
            case binom::ValType::dword: data.pushFront<ui32>(var.toPrimitive().getValue().asUi32()); break;
            case binom::ValType::qword: data.pushFront<ui64>(var.toPrimitive().getValue().asUi64()); break;
            case binom::ValType::invalid_type:
            break;
          }
        } break;

        case VarTypeClass::buffer_array: {
           ByteArray::iterator it = data.addSizeFront(toSize(toValueType(descriptor.type)) * var.toBufferArray().getMemberCount());
           switch (toValueType(descriptor.type)) {
             case binom::ValType::byte: for(ValueRef val_ref : var.toBufferArray()) {*reinterpret_cast<ui8*>(it) = val_ref.asUi8(); ++it;} break;
             case binom::ValType::word: for(ValueRef val_ref : var.toBufferArray()) {*reinterpret_cast<ui16*>(it) = val_ref.asUi16(); it += 2;} break;
             case binom::ValType::dword: for(ValueRef val_ref : var.toBufferArray()) {*reinterpret_cast<ui32*>(it) = val_ref.asUi32(); it += 4;} break;
             case binom::ValType::qword: for(ValueRef val_ref : var.toBufferArray()) {*reinterpret_cast<ui64*>(it) = val_ref.asUi64(); it += 8;} break;
             case binom::ValType::invalid_type:
             break;
           }
        } break;
      }
    } break;

    case binom::VarTypeClass::array: {
      data = fmm.getNodeData(descriptor);
      data.pushFront<virtual_index>(createVariable(var));
    } break;
  }

  fmm.updateNode(node_index, descriptor.type, std::move(data), &descriptor);
}

void FileNodeVisitor::insert(ui64 index, Variable var) {
  ScopedRWGuard lk(current_rwg, LockType::unique_lock);
  NodeDescriptor descriptor = getDescriptor();
  ByteArray data;

  switch (toTypeClass(descriptor.type)) {
    default: throw Exception(ErrCode::binom_invalid_type);

    case binom::VarTypeClass::buffer_array: {
      data = fmm.getNodeData(descriptor);
      switch (var.typeClass()) {
        default: throw Exception(ErrCode::binom_invalid_type);

        case VarTypeClass::primitive: {
          switch (toValueType(descriptor.type)) {
            case binom::ValType::byte: data.insert<ui8>(index, 0, var.toPrimitive().getValue()); break;
            case binom::ValType::word: data.insert<ui16>(index, 0, var.toPrimitive().getValue()); break;
            case binom::ValType::dword: data.insert<ui32>(index, 0, var.toPrimitive().getValue()); break;
            case binom::ValType::qword: data.insert<ui64>(index, 0, var.toPrimitive().getValue()); break;
            case binom::ValType::invalid_type:
            break;
          }
        } break;

        case VarTypeClass::buffer_array: {
           ByteArray::iterator it = data.addSizeTo(toSize(toValueType(descriptor.type))*index,
                                                   toSize(toValueType(descriptor.type)) * var.toBufferArray().getMemberCount());
           switch (toValueType(descriptor.type)) {
             case binom::ValType::byte: for(ValueRef val_ref : var.toBufferArray()) {*reinterpret_cast<ui8*>(it) = val_ref.asUi8(); ++it;} break;
             case binom::ValType::word: for(ValueRef val_ref : var.toBufferArray()) {*reinterpret_cast<ui16*>(it) = val_ref.asUi16(); it += 2;} break;
             case binom::ValType::dword: for(ValueRef val_ref : var.toBufferArray()) {*reinterpret_cast<ui32*>(it) = val_ref.asUi32(); it += 4;} break;
             case binom::ValType::qword: for(ValueRef val_ref : var.toBufferArray()) {*reinterpret_cast<ui64*>(it) = val_ref.asUi64(); it += 8;} break;
             case binom::ValType::invalid_type:
             break;
           }
        } break;
      }
    } break;

    case binom::VarTypeClass::array: {
      data = fmm.getNodeData(descriptor);
      data.insert<virtual_index>(index, 0, createVariable(var));
    } break;
  }

  fmm.updateNode(node_index, std::move(data), &descriptor);
}

void FileNodeVisitor::insert(BufferArray name, Variable var) {
  ScopedRWGuard lk(current_rwg, LockType::unique_lock);
  NodeDescriptor descriptor = getDescriptor();
  if(descriptor.type != VarType::object)
    throw Exception(ErrCode::binom_invalid_type);

  // Insert in empty object
  if(descriptor.size == 0) {
    ui64 name_block_size = name.getMemberCount()*name.getMemberSize();
    ByteArray data(sizeof (ObjectDescriptor) + sizeof (ObjectNameLength) + name_block_size + sizeof(virtual_index));
    new(data.begin<ObjectDescriptor>()) ObjectDescriptor{
      1, name.getMemberCount()*name.getMemberSize(), 1
    };
    new(data.begin<ObjectNameLength>(sizeof (ObjectDescriptor))) ObjectNameLength {
      name.getValType(), name.getMemberCount(), 1
    };
    data.get<virtual_index>(0, sizeof (ObjectDescriptor) + sizeof (ObjectNameLength) + name_block_size) = createVariable(var);
    fmm.updateNode(node_index, data, &descriptor);
    return;
  }

  ObjectElementFinder finder(fmm.getNodeData(descriptor));
  finder.insert(name, createVariable(var));
  fmm.updateNode(node_index, finder.getNodeData(), &descriptor);
}

void FileNodeVisitor::remove(ui64 index, ui64 count) {
  ScopedRWGuard lk(current_rwg, LockType::unique_lock);
  NodeDescriptor descriptor = getDescriptor();
  switch (toTypeClass(descriptor.type)) {
    default: throw Exception(ErrCode::binom_invalid_type);
    case binom::VarTypeClass::buffer_array:
      if(!descriptor.size) throw Exception(ErrCode::binom_out_of_range);
      if(index + count > descriptor.size/toSize(toValueType(descriptor.type)))
         throw Exception(ErrCode::binom_out_of_range);
      fmm.updateNode(node_index,
                     fmm.getNodeData(descriptor).remove(index*toSize(toValueType(descriptor.type)), count*toSize(toValueType(descriptor.type))),
                     &descriptor);
    return;
    case binom::VarTypeClass::array:
      if(!descriptor.size) throw Exception(ErrCode::binom_out_of_range);
      ByteArray data = fmm.getNodeData(descriptor);
      ByteArray indexes = data.takeFrom<virtual_index>(index, count);
      ByteArray add_indexes;
      for(virtual_index* index_it = indexes.begin<virtual_index>(),
                       * index_end = indexes.end<virtual_index>();
          index_it != index_end; ++index_it)
        add_indexes += getContainedNodeIndexes(*index_it);
      indexes += std::move(add_indexes);
      for(virtual_index* index_it = indexes.begin<virtual_index>(),
                       * index_end = indexes.end<virtual_index>();
          index_it != index_end; ++index_it)
        fmm.removeNode(*index_it);
      fmm.updateNode(node_index, data, &descriptor);
    return;
  }
}

void FileNodeVisitor::remove(BufferArray name) {
  ScopedRWGuard lk(current_rwg, LockType::unique_lock);
  NodeDescriptor descriptor = getDescriptor();
  if(descriptor.type != VarType::object)
    throw Exception(ErrCode::binom_invalid_type);
  if(descriptor.size == 0)
    throw Exception(ErrCode::binom_out_of_range);
  ObjectElementFinder finder(fmm.getNodeData(descriptor));
  if(!finder.remove(std::move(name)))
    throw Exception(ErrCode::binom_out_of_range);
}

void FileNodeVisitor::remove(Path path) {
  if(Path::iterator it = ++path.begin();it == path.end())
      switch (it->type()) {
      case binom::PathNodeType::index: remove(it->index()); return;
      case binom::PathNodeType::name: remove(it->name()); return;
      }

    Path::PathNode last_node = *path.begin();
    FileNodeVisitor visitor(*this);
    for(Path::iterator it = ++path.begin(), end = path.end(); it != end ; ++it) {
      switch (last_node.type()) {
        case PathNodeType::index: visitor.stepInside(last_node.index()); break;
        case PathNodeType::name:  visitor.stepInside(last_node.name()); break;
      }
      last_node = *it;
    }
    switch (last_node.type()) {
      case binom::PathNodeType::index: visitor.remove(last_node.index()); break;
      case binom::PathNodeType::name: visitor.remove(last_node.name()); break;
    }
}

NodeVector FileNodeVisitor::findAll(Query query, NodeVector node_vector) {

}

FileNodeVisitor FileNodeVisitor::find(Query query) {

}
