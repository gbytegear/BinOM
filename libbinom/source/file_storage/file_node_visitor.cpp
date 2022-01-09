#include "libbinom/include/file_storage/file_node_visitor.h"
#include "libbinom/include/file_storage/file_object_element_finder.h"

#include <functional>

using namespace binom;

Variable FileNodeVisitor::buildVariable(virtual_index node_index) const {
  RWGuard rwg = fmm->getRWGuard(node_index);
  ScopedRWGuard lk(rwg, LockType::shared_lock);
  NodeDescriptor descriptor = fmm->getNodeDescriptor(node_index);
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
  ScopedRWGuard cur_lk(lk? *lk : *(rwg_ptr = std::make_unique<RWGuard>(fmm->getRWGuard(node_index))), LockType::shared_lock);
  NodeDescriptor descriptor = _descriptor? *_descriptor : fmm->getNodeDescriptor(node_index);
  if(toTypeClass(descriptor.type) != VarTypeClass::primitive)
    throw Exception(ErrCode::binom_invalid_type);
  return Primitive(toValueType(descriptor.type), &descriptor.index);
}

BufferArray FileNodeVisitor::buildBufferArray(virtual_index node_index, const NodeDescriptor* _descriptor, FileNodeVisitor::ScopedRWGuard* lk) const {
  std::unique_ptr<RWGuard> rwg_ptr;
  ScopedRWGuard cur_lk(lk? *lk : *(rwg_ptr = std::make_unique<RWGuard>(fmm->getRWGuard(node_index))), LockType::shared_lock);
  NodeDescriptor descriptor = _descriptor? *_descriptor : fmm->getNodeDescriptor(node_index);
  if(toTypeClass(descriptor.type) != VarTypeClass::buffer_array)
    throw Exception(ErrCode::binom_invalid_type);
  ByteArray data = fmm->getNodeData(descriptor);
  data.pushFront<ui64>(data.length() / toSize(toValueType(descriptor.type)));
  data.pushFront(descriptor.type);
  void* variable = data.unfree();
  return *reinterpret_cast<BufferArray*>(&variable);
}

Array FileNodeVisitor::buildArray(virtual_index node_index, const NodeDescriptor* _descriptor, FileNodeVisitor::ScopedRWGuard* lk) const {
  std::unique_ptr<RWGuard> rwg_ptr;
  ScopedRWGuard cur_lk(lk? *lk : *(rwg_ptr = std::make_unique<RWGuard>(fmm->getRWGuard(node_index))), LockType::shared_lock);
  NodeDescriptor descriptor = _descriptor? *_descriptor : fmm->getNodeDescriptor(node_index);
  if(toTypeClass(descriptor.type) != VarTypeClass::array)
    throw Exception(ErrCode::binom_invalid_type);
  ByteArray data = fmm->getNodeData(descriptor);
  {
    void** array_it = data.begin<void*>();
    for(auto it = data.begin<virtual_index>(), end = data.end<virtual_index>(); it != end; (++it, ++array_it))
      new(array_it) Variable(buildVariable(*it));
    data.resize(data.length<virtual_index>()*PTR_SZ);
  }
  data.pushFront<ui64>(data.length() / sizeof (void*));
  data.pushFront(descriptor.type);
  void* variable = data.unfree();
  return *reinterpret_cast<Array*>(&variable);
}

Object FileNodeVisitor::buildObject(virtual_index node_index, const NodeDescriptor* _descriptor, FileNodeVisitor::ScopedRWGuard* lk) const {
  std::unique_ptr<RWGuard> rwg_ptr;
  ScopedRWGuard cur_lk(lk? *lk : *(rwg_ptr = std::make_unique<RWGuard>(fmm->getRWGuard(node_index))), LockType::shared_lock);
  NodeDescriptor descriptor = _descriptor? *_descriptor : fmm->getNodeDescriptor(node_index);
  if(toTypeClass(descriptor.type) != VarTypeClass::object)
    throw Exception(ErrCode::binom_invalid_type);
  if(descriptor.isNoData())
    return Object();

//  ObjectElementFinder finder(fmm.getNodeData(descriptor), node_index);
  ObjectElementFinder finder(*fmm, node_index, cur_lk.getRWGuard());
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
  return fmm->createNode(primitive.getType(), ByteArray(primitive.getDataPtr(), toSize(primitive.getValType())));
}

virtual_index FileNodeVisitor::createBufferArray(BufferArray buffer_array) {
  return fmm->createNode(buffer_array.getType(), buffer_array.toByteArray());
}

virtual_index FileNodeVisitor::createArray(Array array) {
  if(array.isEmpty()) return fmm->createNode(VarType::array, ByteArray());
  ByteArray data(array.getMemberCount()*sizeof(virtual_index));
  virtual_index* it = data.begin<virtual_index>();
  for(Variable& variable : array) {
    *it = createVariable(std::move(variable));
    ++it;
  }
  return fmm->createNode(VarType::array, std::move(data));
}

virtual_index FileNodeVisitor::createObject(Object object) {
  if(object.isEmpty()) return fmm->createNode(VarType::object, ByteArray());
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
  return fmm->createNode(VarType::object,
                        ByteArray{
                          ByteArray(&descriptor, sizeof (ObjectDescriptor)),
                          std::move(name_length_block),
                          std::move(name_block),
                          std::move(index_block)
                        });
}

ByteArray FileNodeVisitor::getContainedNodeIndexes(virtual_index node_index) {
  std::unique_ptr<RWGuard> rwg_ptr;
  ScopedRWGuard cur_lk((node_index == this->node_index)? current_rwg : *(rwg_ptr = std::make_unique<RWGuard>(fmm->getRWGuard(node_index))), LockType::shared_lock);
  NodeDescriptor descriptor = fmm->getNodeDescriptor(node_index);
  switch (toTypeClass(descriptor.type)) {
    case binom::VarTypeClass::primitive:
    case binom::VarTypeClass::buffer_array: return ByteArray();
    case binom::VarTypeClass::array: {
      if(descriptor.isEmpty()) return ByteArray();
      ByteArray indexes = fmm->getNodeData(descriptor);
      ByteArray add_indexes;
      for(virtual_index* index_it = indexes.begin<virtual_index>(),
                       * index_end = indexes.end<virtual_index>();
          index_it != index_end; ++index_it)
        add_indexes += getContainedNodeIndexes(*index_it);
      indexes += std::move(add_indexes);
      return indexes;
    }
    case binom::VarTypeClass::object: {
      if(descriptor.isEmpty()) return ByteArray();
      ObjectDescriptor object_descriptor = fmm->getNodeDataPart(descriptor, 0, sizeof (ObjectDescriptor)).first<ObjectDescriptor>();
      ByteArray indexes = fmm->getNodeDataPart(descriptor,
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

NodeDescriptor FileNodeVisitor::getDescriptor() const {
  if(node_index == null_index) return NodeDescriptor::null();
  auto lk = getScopedRWGuard(LockType::shared_lock);
  return fmm->getNodeDescriptor(node_index);
}

NodeFullInfo FileNodeVisitor::getFullNodeInfo() {
  if(node_index == null_index) return {null_index, NodeDescriptor::null(), ByteArray()};
  auto lk = getScopedRWGuard(LockType::shared_lock);
  return fmm->getFullNodeInfo(node_index);
}

FileNodeVisitor& FileNodeVisitor::setNull() {
  fmm = nullptr;
  node_index = null_index;
  index = null_index;
  return *this;
}

VarType FileNodeVisitor::getType() const {
  if(isNull()) return VarType::invalid_type;
  auto lk = getScopedRWGuard(LockType::shared_lock);
  NodeDescriptor descriptor = getDescriptor();
  if(toTypeClass(descriptor.type) == VarTypeClass::buffer_array && isValueRef())
    return toVarType(toValueType(descriptor.type));
  else return getDescriptor().type;
}

ui64 FileNodeVisitor::getElementCount() const {
  if(isNull()) return 0;
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
      if(descriptor.isEmpty()) return 0;
      ObjectDescriptor obj_desriptor = fmm->getNodeDataPart(descriptor, 0, sizeof (ObjectDescriptor)).get<ObjectDescriptor>(0);
      return obj_desriptor.index_count;
    }
  }
}

std::optional<BufferArray> FileNodeVisitor::getName() {
  if(isNull()) return std::optional<BufferArray>();
  auto lk = getScopedRWGuard(LockType::shared_lock);
  if(name_pos.isNull())
    return std::optional<BufferArray>();
  ByteArray data = fmm->getNodeDataPart(name_pos.parent_node_index, name_pos.name_pos, name_pos.length * toSize(name_pos.char_type));
  return BufferArray(name_pos.char_type, data.begin(), name_pos.length);
}

FileNodeVisitor& FileNodeVisitor::stepInside(ui64 index) {
  if(isNull()) return *this;
  auto lk = getScopedRWGuard(LockType::shared_lock);

  name_pos.setNull();
  NodeDescriptor descriptor = getDescriptor();
  switch (toTypeClass(descriptor.type)) {
    case binom::VarTypeClass::primitive:
    case binom::VarTypeClass::object:
    case binom::VarTypeClass::invalid_type:
      setNull();
    return *this;

    case binom::VarTypeClass::buffer_array:
      if(isValueRef())
        throw Exception(ErrCode::binom_invalid_type);
      if(descriptor.size / toSize(toValueType(descriptor.type)) <= index) {
        setNull();
        break;
      }
      this->index = index;
    break;
    case binom::VarTypeClass::array:
      if(descriptor.size / sizeof (virtual_index) <= index) {
        setNull();
        break;
      }
      node_index = fmm->getNodeDataPart(descriptor, index * sizeof (virtual_index), sizeof(virtual_index)).first<virtual_index>();
    break;
  }

  current_rwg = fmm->getRWGuard(node_index);
  return *this;
}

FileNodeVisitor& FileNodeVisitor::stepInside(BufferArray name) {
  if(isNull()) return *this;
  auto lk = getScopedRWGuard(LockType::shared_lock);

  // Validation
  NodeDescriptor descriptor = getDescriptor();
  if(descriptor.type != VarType::object) {
    setNull();
    return *this;
  }
  elif(!descriptor.size) {
    setNull();
    current_rwg = fmm->getRWGuard(node_index);
    return *this;
  }

  ObjectElementFinder finder(const_cast<FileNodeVisitor&>(*this));
  if(!finder.findElement(std::move(name))){
    setNull();
    current_rwg = fmm->getRWGuard(node_index);
    return *this;
  }

  node_index = finder.getNodeIndex();
  name_pos = finder.getNamePosition();
  current_rwg = fmm->getRWGuard(node_index);
  return *this;
}

FileNodeVisitor& FileNodeVisitor::stepInside(Path path) {
  for(const Path::PathNode& path_node : path)
    if(isNull()) return *this;
    else switch (path_node.type()) {
        case binom::PathNodeType::index: stepInside(path_node.index()); continue;
        case binom::PathNodeType::name: stepInside(path_node.name()); continue;
      }
  return *this;
}

Variable FileNodeVisitor::getVariable() const {
  if(isNull()) return nullptr;
  if(isValueRef()) {
    ScopedRWGuard lk(current_rwg, LockType::shared_lock);
    NodeDescriptor descriptor = fmm->getNodeDescriptor(node_index);
    ByteArray data = fmm->getNodeDataPart(descriptor,
                                         index*toSize(toValueType(descriptor.type)),
                                         toSize(toValueType(descriptor.type)));
    data.pushFront(toVarType(toValueType(descriptor.type)));
    void* variable = data.unfree();
    return *reinterpret_cast<Variable*>(&variable);
  } else return buildVariable(node_index);
}

bool FileNodeVisitor::contains(ui64 index) {
  if(isNull()) return false;
  auto lk = getScopedRWGuard(LockType::shared_lock);
  switch (getTypeClass()) {
    case binom::VarTypeClass::primitive: return false;
    case binom::VarTypeClass::buffer_array: return index < getElementCount();
    case binom::VarTypeClass::array: return index < getElementCount();
    case binom::VarTypeClass::object: return false;
    default: return false;
  }
}

bool FileNodeVisitor::contains(BufferArray name) {
  if(isNull()) return false;
  auto lk = getScopedRWGuard(LockType::shared_lock);
  NodeDescriptor descriptor = getDescriptor();
  if(descriptor.type != VarType::object)
    return false;
  elif(descriptor.size == 0)
    return false;
  ObjectElementFinder finder(const_cast<FileNodeVisitor&>(*this));
  return finder.findElement(std::move(name));
}

bool FileNodeVisitor::contains(Path path) {
  if(isNull()) return false;
  FileNodeVisitor node(*this);
  for(const Path::PathNode& path_node : path)
    if(node.isNull()) return false;
    else switch (path_node.type()) {
        case binom::PathNodeType::index: node.stepInside(path_node.index()); continue;
        case binom::PathNodeType::name: node.stepInside(path_node.name()); continue;
      }
  return !node.isNull();
}

void FileNodeVisitor::setVariable(Variable var) {
  throwIfNull();
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
    fmm->updateNodeDataPart(node_index, index * toSize(toValueType(descriptor.type)), std::move(data), &descriptor);
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
      if(var.toObject().isEmpty()) break;
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
    fmm->removeNode(*index_it);

  // Update node
  fmm->updateNode(node_index, var.type(), std::move(data));
}

void FileNodeVisitor::setVariable(ui64 index, Variable var) {throwIfNull(); return contains(index)? getChild(index).setVariable(std::move(var)) : insert(index, std::move(var));}
void FileNodeVisitor::setVariable(BufferArray name, Variable var) {throwIfNull(); return contains(name)? getChild(std::move(name)).setVariable(std::move(var)) : insert(std::move(name), std::move(var));}
void FileNodeVisitor::setVariable(Path path, Variable var) {throwIfNull(); return getChild(std::move(path)).setVariable(std::move(var));}

void FileNodeVisitor::pushBack(Variable var) {
  throwIfNull();
  insert(getElementCount(), std::move(var));
}

void FileNodeVisitor::pushFront(Variable var) {
  throwIfNull();
  insert(0, std::move(var));
}

void FileNodeVisitor::insert(ui64 index, Variable var) {
  throwIfNull();
  ScopedRWGuard lk(current_rwg, LockType::unique_lock);
  NodeDescriptor descriptor = getDescriptor();

  switch (toTypeClass(descriptor.type)) {
    default: throw Exception(ErrCode::binom_invalid_type);

    case binom::VarTypeClass::buffer_array:
      switch (var.typeClass()) {
        default: throw Exception(ErrCode::binom_invalid_type);

        case VarTypeClass::primitive: {
          switch (toValueType(descriptor.type)) {
            case binom::ValType::byte: {
              ui8 value = var.getValue();
              fmm->insertNodeDataPart(node_index, {{index*sizeof(value), ByteArray(&value, sizeof (value))}}, &descriptor);
            } return;
            case binom::ValType::word: {
              ui16 value = var.getValue();
              fmm->insertNodeDataPart(node_index, {{index*sizeof(value), ByteArray(&value, sizeof (value))}}, &descriptor);
            } return;
            case binom::ValType::dword: {
              ui32 value = var.getValue();
              fmm->insertNodeDataPart(node_index, {{index*sizeof(value), ByteArray(&value, sizeof (value))}}, &descriptor);
            } return;
            case binom::ValType::qword: {
              ui64 value = var.getValue();
              fmm->insertNodeDataPart(node_index, {{index*sizeof(value), ByteArray(&value, sizeof (value))}}, &descriptor);
            } return;
            case binom::ValType::invalid_type:
            return;
          }
        } return;

        case VarTypeClass::buffer_array: {
          BufferArray data(toValueType(descriptor.type));
          for(ValueRef val_ref : var.toBufferArray())
            data.pushBack(val_ref);
          fmm->insertNodeDataPart(node_index, {{index*toSize(toValueType(descriptor.type)), data.toByteArray()}}, &descriptor);
        } return;
      }

    case binom::VarTypeClass::array: {
      virtual_index new_node_index = createVariable(std::move(var));
      fmm->insertNodeDataPart(node_index,
                             {{sizeof (virtual_index) * index, ByteArray(&new_node_index, sizeof (virtual_index))}},
                             &descriptor);
    } break;
  }
}

void FileNodeVisitor::insert(BufferArray name, Variable var) {
  throwIfNull();
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
    memcpy(&data.get(sizeof (ObjectDescriptor) + sizeof (ObjectNameLength)), name.getDataPointer(), name_block_size);
    data.get<virtual_index>(0, sizeof (ObjectDescriptor) + sizeof (ObjectNameLength) + name_block_size) = createVariable(var);
    fmm->updateNode(node_index, data, &descriptor);
    return;
  }

  ObjectElementFinder finder(const_cast<FileNodeVisitor&>(*this));
  finder.insert(std::move(name), createVariable(var));
}

void FileNodeVisitor::remove(ui64 index, ui64 count) {
  throwIfNull();
  ScopedRWGuard lk(current_rwg, LockType::unique_lock);
  NodeDescriptor descriptor = getDescriptor();
  switch (toTypeClass(descriptor.type)) {
    default: throw Exception(ErrCode::binom_invalid_type);
    case binom::VarTypeClass::buffer_array:
      if(!descriptor.size) throw Exception(ErrCode::binom_out_of_range);
      if(index + count > descriptor.size/toSize(toValueType(descriptor.type)))
         throw Exception(ErrCode::binom_out_of_range);
      fmm->removeNodeDataParts(node_index, {{index*toSize(toValueType(descriptor.type)), count*toSize(toValueType(descriptor.type))}}, &descriptor);
    return;
    case binom::VarTypeClass::array:
      if(!descriptor.size) throw Exception(ErrCode::binom_out_of_range);
      ByteArray indexes = fmm->getNodeDataPart(node_index, index * sizeof (virtual_index), count * sizeof (virtual_index));
      ByteArray add_indexes;
      for(virtual_index* index_it = indexes.begin<virtual_index>(),
                       * index_end = indexes.end<virtual_index>();
          index_it != index_end; ++index_it)
        add_indexes += getContainedNodeIndexes(*index_it);
      indexes += std::move(add_indexes);
      fmm->removeNodeDataParts(node_index, {{index * sizeof (virtual_index), count * sizeof (virtual_index)}}, &descriptor);
      for(virtual_index* index_it = indexes.begin<virtual_index>(),
                       * index_end = indexes.end<virtual_index>();
          index_it != index_end; ++index_it)
        fmm->removeNode(*index_it);
    return;
  }
}

void FileNodeVisitor::remove(BufferArray name) {
  throwIfNull();
  ScopedRWGuard lk(current_rwg, LockType::unique_lock);
  if(getElementCount()) {
    ObjectElementFinder finder(const_cast<FileNodeVisitor&>(*this));
    finder.remove(std::move(name));
  }
}

void FileNodeVisitor::remove(Path path) {
  throwIfNull();
  if(Path::iterator it = ++path.begin(), rm_it = path.begin();it == path.end())
      switch (rm_it->type()) {
      case binom::PathNodeType::index: remove(rm_it->index()); return;
      case binom::PathNodeType::name: remove(rm_it->name()); return;
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

bool FileNodeVisitor::test(Query query, ui64 index) noexcept {
    TestExpressionFrame test_expr;
    for(Query::QueryEpression& expr : query) test_expr(expr, *this, index);
    return test_expr.getValue();
}

NodeVector FileNodeVisitor::findSet(Query query, NodeVector node_vector) {
  if(isNull()) return node_vector;
  if(!isIterable()) return node_vector;
  for(auto it = begin(), _end = end(); it != _end; ++it) {
    auto node = *it;
    if(node.test(query, it.index))
      node_vector.emplace_back(std::unique_ptr<NodeVisitorBase>(new FileNodeVisitor(node)));
  }
  return node_vector;
}

FileNodeVisitor FileNodeVisitor::find(Query query) {
  if(isNull()) return FileNodeVisitor();
  if(!isIterable()) return FileNodeVisitor(*fmm, nullptr);
  for(auto it = begin(), _end = end(); it != _end; ++it) {
    auto node = *it;
    if(node.test(query, it.index))
      return node;
  }
  return FileNodeVisitor(*fmm, nullptr);
}

FileNodeVisitor FileNodeVisitor::findFrom(ui64 index, Query query) {
  if(isNull()) return FileNodeVisitor();
  if(!isIterable()) return FileNodeVisitor(*fmm, nullptr);
  for(auto it = beginFrom(index), _end = end(); it != _end; ++it) {
    auto node = *it;
    if(node.test(query, it.index))
      return node;
  }
  return FileNodeVisitor(*fmm, nullptr);
}

FileNodeVisitor FileNodeVisitor::findFrom(BufferArray name, Query query) {
  if(isNull()) return FileNodeVisitor();
  if(!isIterable()) return FileNodeVisitor(*fmm, nullptr);
  for(auto it = beginFrom(name), _end = end(); it != _end; ++it) {
    auto node = *it;
    if(node.test(query, it.index))
      return node;
  }
  return FileNodeVisitor(*fmm, nullptr);
}

NodeVector FileNodeVisitor::findSetFrom(ui64 index, Query query, NodeVector node_vector) {
  if(isNull()) return node_vector;
  if(!isIterable()) return node_vector;
  for(auto it = beginFrom(index), _end = end(); it != _end; ++it) {
    auto node = *it;
    if(node.test(query, it.index))
      node_vector.emplace_back(std::unique_ptr<NodeVisitorBase>(new FileNodeVisitor(node)));
  }
  return node_vector;
}

NodeVector FileNodeVisitor::findSetFrom(BufferArray name, Query query, NodeVector node_vector) {
  if(isNull()) return node_vector;
  if(!isIterable()) return node_vector;
  for(auto it = beginFrom(name), _end = end(); it != _end; ++it) {
    auto node = *it;
    if(node.test(query, it.index))
      node_vector.emplace_back(std::unique_ptr<NodeVisitorBase>(new FileNodeVisitor(node)));
  }
  return node_vector;
}

binom::FileNodeVisitor::operator Variable() {return getVariable();}

FileNodeVisitor::NodeIterator FileNodeVisitor::beginFrom(ui64 index) {return NodeIterator(*this, index);}

FileNodeVisitor::NodeIterator FileNodeVisitor::beginFrom(BufferArray name) {
  throwIfNull();
  if(getType() != VarType::object)
    return NodeIterator(*this, true);
  if(ObjectElementFinder finder(static_cast<FileNodeVisitor&>(*this)); finder.findElement(std::move(name)))
    return NodeIterator(*this, finder.getObjectElementPosition());
  else return NodeIterator(*this, true);
}
FileNodeVisitor::NodeIterator FileNodeVisitor::begin() {return NodeIterator(*this);}
FileNodeVisitor::NodeIterator FileNodeVisitor::end() {return NodeIterator(*this, true);}
