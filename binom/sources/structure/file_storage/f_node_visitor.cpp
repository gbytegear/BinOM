#include "binom/includes/structure/file_storage/f_node_visitor.h"

using namespace binom;

void FileNodeVisitor::loadNodeVisitor(ui64 index) {
  descriptor = vmemory->getNodeDescriptor(index);
}

void FileNodeVisitor::updateNodeVisitor() {
  vmemory->setNodeDescriptor(descriptor);
}

ByteArray FileNodeVisitor::loadData() {return vmemory->getData (descriptor.descriptor.data_index, descriptor.descriptor.block_size);}

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








void FileNodeVisitor::set(Variable& variable){
  switch (variable.typeClass()) {
  case VarTypeClass::primitive: return set(variable.toPrimitive());
  case VarTypeClass::buffer_array: return set(variable.toBufferArray());
  case VarTypeClass::array: return set(variable.toArray());
  case VarTypeClass::object: return set(variable.toObject());
  default: throw SException(ErrCode::binom_invalid_type);
  }
}

void FileNodeVisitor::set(Primitive& primitive) {
  free();
  switch (primitive.getType()) {
    case VarType::byte:
      descriptor.descriptor.data_index = vmemory->setPrimitive(static_cast<ui8>(primitive.getValue().asUnsigned()));
      descriptor.descriptor.block_size = 1;
      descriptor.descriptor.type = VarType::byte;
    break;
    case VarType::word:
      descriptor.descriptor.data_index = vmemory->setPrimitive(static_cast<ui16>(primitive.getValue().asUnsigned()));
      descriptor.descriptor.block_size = 2;
      descriptor.descriptor.type = VarType::word;
    break;
    case VarType::dword:
      descriptor.descriptor.data_index = vmemory->setPrimitive(static_cast<ui16>(primitive.getValue().asUnsigned()));
      descriptor.descriptor.block_size = 4;
      descriptor.descriptor.type = VarType::dword;
    break;
    case VarType::qword:
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
  free();
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
  free();
  descriptor.descriptor.data_index = vmemory->setData(data);
  descriptor.descriptor.block_size = data.length();
  descriptor.descriptor.type = VarType::array;
  updateNodeVisitor();
}

void FileNodeVisitor::set(Object& object) {
  ByteArray data(pack(object));
  free();
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







void FileNodeVisitor::free() {
  switch (getType ()) {
  case VarType::byte: vmemory->freePrimitive<ui8>(descriptor.descriptor.data_index);break;
  case VarType::word: vmemory->freePrimitive<ui16>(descriptor.descriptor.data_index);break;
  case VarType::dword: vmemory->freePrimitive<ui32>(descriptor.descriptor.data_index);break;
  case VarType::qword: vmemory->freePrimitive<ui64>(descriptor.descriptor.data_index);break;
  case VarType::byte_array:
  case VarType::word_array:
  case VarType::dword_array:
  case VarType::qword_array:
    vmemory->freeData(descriptor.descriptor.data_index);
  break;
  case VarType::array:
  case VarType::object: throw SException(ErrCode::binom_invalid_type, "Impthis!");
  default: throw SException(ErrCode::binom_invalid_type);
  }
}








FileNodeVisitor& FileNodeVisitor::stepInside(ui64 index) {
  if(getType() != VarType::array) throw SException(ErrCode::binom_invalid_type);
  ByteArray array_data = vmemory->getData(descriptor.descriptor);
  if(array_data.length<Element>() >= index) throw SException(ErrCode::binom_out_of_range);
  loadNodeVisitor(array_data.get<Element>(index).node_index);
  return *this;
}

FileNodeVisitor& FileNodeVisitor::stepInside(BufferArray name) {
  if(getType() != VarType::object) throw SException(ErrCode::binom_invalid_type);
  ByteArray data = loadData();
  ObjectDescriptor object_des = data.takeFront<ObjectDescriptor>();

  // TODO: Change on iterators
  ByteArray name_des_block = data.takeFront(object_des.name_length_block_size),
            name_block = data.takeFront(object_des.name_block_size),
            index_block = data.takeBack(object_des.element_count*sizeof(Element));

  ui64 middle;
  {
    ui64 right = name_des_block.length<NameDescriptor>();
    ui64 left = 0;

    while (true) {
      if(left <= right) throw SException(ErrCode::binom_out_of_range);
      middle = (left + right) / 2;
      NameDescriptor& des = name_des_block.get<NameDescriptor>(middle);
      if(des.length > name.getDataSize()) right = middle - 1;
      elif(des.length > name.getDataSize()) left = middle + 1;
      elif(des.length == name.getDataSize()) break;
    }
  }

  ui64 index = 0;
  {
    ByteArray::iterator name_block_start;
    {
      ui64 name_block_pos = 0;
      for(ui64 i = 0; i < middle; ++i) {
        NameDescriptor& des = name_des_block.get<NameDescriptor>(i);
        index += des.count;
        name_block_pos += des.count * des.length;
      }
      name_block_start = name_block.begin () + name_block_pos;
    }

    ui64 right = name_des_block.get<NameDescriptor>(middle).count;
    ui64 left = 0;
    ui64 middle;

    while(true) {
      if(left <= right) throw SException(ErrCode::binom_out_of_range);
      middle = (left + right) / 2;
      int res = memcmp((name_block_start + middle*name.getDataSize()),
                       name.getDataPointer(),
                       name.getDataSize());
      if(res > 0) right = middle - 1;
      elif(res < 0) left = middle + 1;
      elif(res == 0) break;
    }
    index += middle;
  }

  loadNodeVisitor(index_block.get<Element>(index).node_index);
  return *this;
}

FileNodeVisitor& FileNodeVisitor::operator=(Variable var) {

}
