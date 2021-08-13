#include "binom/includes/file_storage/file_node_visitor.h"

using namespace binom;

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
      node_index = fmm.getNodeDataPart(node_index, index * sizeof (virtual_index), sizeof(virtual_index)).first<virtual_index>();
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

  // Load data (ObjectDescriptor, ObjectNameLengths, names)
  ObjectDescriptor object_descriptor = fmm.getNodeDataPart(node_index, 0, sizeof (ObjectDescriptor)).first<ObjectDescriptor>();
  ByteArray length_elements;
  ByteArray name_str;
  fmm.getNodeDataPart(node_index,
                      sizeof (ObjectDescriptor),
                      object_descriptor.length_element_count * sizeof (ObjectNameLength)
                      + object_descriptor.name_block_size)
      .split(object_descriptor.length_element_count * sizeof (ObjectNameLength), length_elements, name_str);

  // Find O(n) ObjectNameLength, names block shift and first index in name block
  ui64 index = 0;
  ui64 name_block_shift = 0;
  ObjectNameLength* name_length_des = length_elements.begin<ObjectNameLength>();
  for(ObjectNameLength* end = length_elements.end<ObjectNameLength>(); name_length_des != end; ++name_length_des) {
    if(name_length_des->char_type != name.getValType() || name_length_des->name_length != name.getMemberCount()) {
      index += name_length_des->name_count;
      name_block_shift += toSize(name_length_des->char_type) * name_length_des->name_length * name_length_des->name_count;
    } else break;
  }

  // Find name in names block O(log(n))
  {
    i64 name_index_in_block = 0;
    const ValType char_type = name_length_des->char_type;
    const i64 name_count = name_length_des->name_count;
    const ui64 name_byte_length = name_length_des->name_length*toSize(char_type);

    byte* pointer = name_str.begin<byte>(name_block_shift);
    i64 left = 0;
    i64 right = name_length_des->name_count;

    while(true) {
      name_index_in_block = (left + right) / 2;

      if(left > right || name_index_in_block > name_count) {
        name_index_in_block = -1;
        break;
      }

      int cmp = memcmp(pointer + name_index_in_block*toSize(char_type), name.getDataPointer(), name_byte_length);

      if(cmp > 0) right = name_index_in_block - 1;
      elif(cmp < 0) left = name_index_in_block + 1;
      else break;
    }

    if(name_index_in_block == -1)
      throw Exception(ErrCode::binom_out_of_range);

    index += name_index_in_block;
  }

  // Load node_index
  node_index = fmm.getNodeDataPart(node_index,
                                   sizeof (ObjectDescriptor) +
                                   object_descriptor.length_element_count*sizeof(ObjectNameLength) +
                                   object_descriptor.name_block_size +
                                   index*sizeof(virtual_index)
                                   ,sizeof(virtual_index)).first<virtual_index>();

  // Change RWGuard
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
