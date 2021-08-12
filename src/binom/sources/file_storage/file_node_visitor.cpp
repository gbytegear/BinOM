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
  return *this;
}

FileNodeVisitor& FileNodeVisitor::stepInside(BufferArray name) {
  auto lk = getScopedRWGuard(LockType::shared_lock);

  NodeDescriptor descriptor = getDescriptor();
  ObjectDescriptor object_descriptor = fmm.getNodeDataPart(node_index, 0, sizeof (ObjectDescriptor)).first<ObjectDescriptor>();
  ByteArray length_elements;
  ByteArray name_str;
  fmm.getNodeDataPart(node_index,
                      sizeof (ObjectDescriptor),
                      object_descriptor.length_element_count * sizeof (ObjectNameLength)
                      + object_descriptor.name_block_size)
      .split(object_descriptor.length_element_count * sizeof (ObjectNameLength), length_elements, name_str);

  ui64 name_array_shift = 0;
  ObjectNameLength* name_length_des = length_elements.begin<ObjectNameLength>();
  for(ObjectNameLength* end = length_elements.end<ObjectNameLength>(); name_length_des != end; ++name_length_des) {
    if(name_length_des->char_type != name.getValType() || name_length_des->name_length != name.getMemberCount())
      name_array_shift += toSize(name_length_des->char_type)*name_length_des->name_length;
    else break;
  }

  i64 shift_to_name = 0;
  {
    const ValType char_type = name_length_des->char_type;
    const i64 name_count = name_length_des->name_count;
    const ui64 name_length = name_length_des->name_length;

    byte* pointer = name_str.begin<byte>(name_array_shift);
    i64 left = 0;
    i64 right = name_length_des->name_count;

    for(;;) {
      shift_to_name = (left + right) / 2;

      if(left > right || shift_to_name > name_count) {
        shift_to_name = -1;
        break;
      }

      BufferArray selected_name(char_type, pointer + shift_to_name*toSize(char_type), name_length);

      if(selected_name > name) right = shift_to_name - 1;
      if(selected_name < name) left = shift_to_name + 1;
      else break;
    }

    if(shift_to_name == -1)
      throw Exception(ErrCode::binom_out_of_range);
  }


  return *this;
}
