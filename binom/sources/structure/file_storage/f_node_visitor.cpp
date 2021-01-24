#include "binom/includes/structure/file_storage/f_node_visitor.h"

using namespace binom;

void FileNodeVisitor::loadNodeVisitor(ui64 index) {
  descriptor = vmemory->getNodeDescriptor(index);
}

void FileNodeVisitor::updateNodeVisitor() {
  vmemory->setNodeDescriptor(descriptor);
}

ByteArray FileNodeVisitor::pack(Object& object) {
  NameDescriptor name_descriptor;
  ByteArray object_des_block(0),
            name_des_block(0),
            name_block(0),
            index_block(0);
  name_block.pushBack(object.getMemberCount());
  for(NamedVariable& var : object) {
    if(name_descriptor.count != var.name.getMemberCount() && &var != object.begin()) {
      name_des_block.pushBack(name_descriptor);
      name_descriptor.length = var.name.getMemberCount();
      name_descriptor.count = 1;
    } elif(&var != object.begin()) {
      name_descriptor.length = var.name.getMemberCount();
      name_descriptor.count = 1;
    } else ++name_descriptor.count;

    Element element{create(var.variable)};
    name_block.pushBack(var.name.toByteArray());
    index_block.pushBack(element);
  }

  name_des_block.pushBack(name_descriptor);

  {
    ObjectDescriptor object_descriptor{name_des_block.length(),
                                       name_block.length(),
                                       index_block.length()/sizeof(Element)};
    object_des_block.pushBack(object_descriptor);
  }
  return ByteArray({object_des_block, name_des_block, name_block, index_block});
}

void FileNodeVisitor::set(Primitive& primitive) {
  switch (primitive.getType()) {
    case VarType::byte:
      vmemory->freePrimitive<ui8>(descriptor.descriptor.data_index);
      descriptor.descriptor.data_index = vmemory->setPrimitive(static_cast<ui8>(primitive.getValue().asUnsigned()));
      descriptor.descriptor.block_size = 1;
      descriptor.descriptor.type = VarType::byte;
    break;
    case VarType::word:
      vmemory->freePrimitive<ui16>(descriptor.descriptor.data_index);
      descriptor.descriptor.data_index = vmemory->setPrimitive(static_cast<ui16>(primitive.getValue().asUnsigned()));
      descriptor.descriptor.block_size = 2;
      descriptor.descriptor.type = VarType::word;
    break;
    case VarType::dword:
      vmemory->freePrimitive<ui32>(descriptor.descriptor.data_index);
      descriptor.descriptor.data_index = vmemory->setPrimitive(static_cast<ui16>(primitive.getValue().asUnsigned()));
      descriptor.descriptor.block_size = 4;
      descriptor.descriptor.type = VarType::dword;
    break;
    case VarType::qword:
      vmemory->freePrimitive<ui64>(descriptor.descriptor.data_index);
      descriptor.descriptor.data_index = vmemory->setPrimitive(static_cast<ui16>(primitive.getValue().asUnsigned()));
      descriptor.descriptor.block_size = 8;
      descriptor.descriptor.type = VarType::qword;
    break;
    default: throw SException(ErrCode::binom_invalid_type);
  }
  updateNodeVisitor();
}

void FileNodeVisitor::set(BufferArray& buffer) {
  ByteArray array = buffer.toByteArray();
  vmemory->freeData(descriptor.descriptor.data_index);
  descriptor.descriptor.data_index = vmemory->setData(array);
  descriptor.descriptor.block_size = array.length();
  descriptor.descriptor.type = buffer.getType();
  updateNodeVisitor();
}

void FileNodeVisitor::set(Array& array) {
  ByteArray data(0);
  for(Variable& var : array) {
    Element element{create(var)};
    data.pushBack(element);
  }
  vmemory->freeData(descriptor.descriptor.data_index);
  descriptor.descriptor.data_index = vmemory->setData(data);
  descriptor.descriptor.block_size = data.length();
  descriptor.descriptor.type = VarType::array;
  updateNodeVisitor();
}

void FileNodeVisitor::set(Object& object) {
  ByteArray data(pack(object));
  descriptor.descriptor.data_index = vmemory->setData(data);
  descriptor.descriptor.block_size = data.length();
  descriptor.descriptor.type = VarType::object;
  updateNodeVisitor();
}

ui64 FileNodeVisitor::create(Variable& variable) {
  switch (variable.typeClass()) {
    case VarTypeClass::primitive: return create(variable.toPrimitive());
    case VarTypeClass::buffer_array: return create(variable.toBufferArray());
    case VarTypeClass::array: return create(variable.toArray());
    case VarTypeClass::object: return create(variable.toObject());
    default: throw SException(ErrCode::binom_invalid_type);
  }
}

ui64 FileNodeVisitor::create(Primitive& primitive) {
  NodeDescriptor descriptor;
  switch (primitive.getType()) {
    case VarType::byte:
      descriptor.data_index = vmemory->setPrimitive(static_cast<ui8>(primitive.getValue().asUnsigned()));
      descriptor.block_size = 1;
      descriptor.type = VarType::byte;
    break;
    case VarType::word:
      descriptor.data_index = vmemory->setPrimitive(static_cast<ui16>(primitive.getValue().asUnsigned()));
      descriptor.block_size = 2;
      descriptor.type = VarType::word;
    break;
    case VarType::dword:
      descriptor.data_index = vmemory->setPrimitive(static_cast<ui16>(primitive.getValue().asUnsigned()));
      descriptor.block_size = 4;
      descriptor.type = VarType::dword;
    break;
    case VarType::qword:
      descriptor.data_index = vmemory->setPrimitive(static_cast<ui16>(primitive.getValue().asUnsigned()));
      descriptor.block_size = 8;
      descriptor.type = VarType::qword;
    break;
    default: throw SException(ErrCode::binom_invalid_type);
  }
  return vmemory->setNodeDescriptor(descriptor);
}

ui64 FileNodeVisitor::create(BufferArray& buffer) {
  NodeDescriptor descriptor;
  ByteArray array = buffer.toByteArray();
  descriptor.data_index = vmemory->setData(array);
  descriptor.block_size = array.length();
  descriptor.type = buffer.getType();
  return vmemory->setNodeDescriptor(descriptor);
}

ui64 FileNodeVisitor::create(Array& array) {
  NodeDescriptor descriptor;
  ByteArray data(0);
  for(Variable& var : array) {
    Element element{create(var)};
    data.pushBack(element);
  }
  descriptor.data_index = vmemory->setData(data);
  descriptor.block_size = data.length();
  descriptor.type = VarType::array;
  return vmemory->setNodeDescriptor(descriptor);
}

ui64 FileNodeVisitor::create(Object& object) {
  NodeDescriptor descriptor;
  ByteArray data(pack(object));
  descriptor.data_index = vmemory->setData(data);
  descriptor.block_size = data.length();
  descriptor.type = VarType::object;
  return vmemory->setNodeDescriptor(descriptor);
}

FileNodeVisitor& FileNodeVisitor::stepInside(ui64 index) {
  if(getType() != VarType::array) throw SException(ErrCode::binom_invalid_type);
  ByteArray array_data = vmemory->getData(descriptor.descriptor);
  if(array_data.length()/sizeof(Element) >= index) throw SException(ErrCode::binom_out_of_range);
  loadNodeVisitor(reinterpret_cast<Element*>(array_data.begin() + index*sizeof(Element))->node_index);
  return *this;
}

FileNodeVisitor& FileNodeVisitor::operator=(Variable var) {

}
