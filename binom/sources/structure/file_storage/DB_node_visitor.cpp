#include "binom/includes/structure/file_storage/DB_node_visitor.h"

using namespace binom;


NodeDescriptor DBNodeVisitor::loadNode(f_virtual_index node_index) {return fvmc.loadNodeDescriptor(node_index);;}
void DBNodeVisitor::updateNode() {node_descriptor = loadNode(node_index);}
ByteArray DBNodeVisitor::loadData() {return fvmc.loadDataByNode(node_index);}

f_virtual_index DBNodeVisitor::createVariable(Variable var) {
  switch (var.typeClass()) {
    case VarTypeClass::primitive: return createPrimitive(std::move(var.toPrimitive()));
    case VarTypeClass::buffer_array: return createBufferArray(std::move(var.toBufferArray()));
    case VarTypeClass::array: return createArray(std::move(var.toArray()));
    case VarTypeClass::object: return createObject(std::move(var.toObject()));
    default: throw SException(ErrCode::binom_invalid_type);
  }
}

f_virtual_index DBNodeVisitor::createPrimitive(Primitive primitive) {
  return fvmc.createNode(primitive.getType(),
                         ByteArray(primitive.getDataPtr(), toSize(primitive.getValType())));
}

f_virtual_index DBNodeVisitor::createBufferArray(BufferArray buffer_array) {
  return fvmc.createNode(buffer_array.getType(), buffer_array.toByteArray());
}

f_virtual_index DBNodeVisitor::createArray(Array array) {
  ByteArray array_data(array.getMemberCount() * sizeof (f_virtual_index));
  {
    f_virtual_index* index_it = array_data.begin<f_virtual_index>();
    for(Variable& var : array) {
      *index_it = createVariable(var);
      ++index_it;
    }
  }
  return fvmc.createNode(VarType::array, std::move(array_data));
}

f_virtual_index DBNodeVisitor::createObject(Object object) {
  ByteArray name_length_segment,
            name_segment,
            index_segment;
  ObjectNameLength name_length;
  for(NamedVariable& var : object) {
    ui64 symbol_count = var.name.getMemberCount();
    if(symbol_count != name_length.name_length) {
      if(object.begin() == &var) {
        name_length.name_length = symbol_count;
      } else {
        name_length_segment.pushBack(name_length);
        name_length.name_length = symbol_count;
      }
    }

    name_segment += var.name.toByteArray();
    index_segment.pushBack<f_virtual_index>(createVariable(var.variable));
  }

  ObjectDescriptor object_descriptor{
        name_length_segment.length<ObjectNameLength>(),
        name_segment.length(),
        index_segment.length<f_virtual_index>()
  };

  ByteArray data{
    ByteArray(&object_descriptor, sizeof (ObjectDescriptor)),
    name_length_segment,
    name_segment,
    index_segment
  };

  return fvmc.createNode(VarType::object, std::move(data));
}

void DBNodeVisitor::deleteNode(f_virtual_index node_index) {
  NodeDescriptor descriptor = fvmc.loadNodeDescriptor(node_index);
  switch (toTypeClass(descriptor.type)) {
    case VarTypeClass::primitive:
    case VarTypeClass::buffer_array:
      fvmc.free(node_index);
    break;
    case VarTypeClass::array: {
      ByteArray data(fvmc.loadHeapDataByIndex(descriptor.index));
      for(f_virtual_index* index_it = data.begin<f_virtual_index>(); index_it != data.end<f_virtual_index>(); ++index_it)
        deleteNode(*index_it);
      fvmc.free(node_index);
      break;
    }
    case VarTypeClass::object:

    break;
  }
}

DBNodeVisitor::DBNodeVisitor(FileVirtualMemoryController& fvmc, f_virtual_index node_index)
  : fvmc(fvmc),
    node_index(node_index) {updateNode();}

DBNodeVisitor::DBNodeVisitor(DBNodeVisitor& other)
  : fvmc(other.fvmc),
    node_descriptor(other.node_descriptor),
    node_index(other.node_index) {}

DBNodeVisitor::DBNodeVisitor(DBNodeVisitor&& other)
  : fvmc(other.fvmc),
    node_descriptor(other.node_descriptor),
    node_index(other.node_index) {}

DBNodeVisitor& DBNodeVisitor::operator=(DBNodeVisitor other) {return *(new(this) DBNodeVisitor(other));}
DBNodeVisitor& DBNodeVisitor::operator=(f_virtual_index _node_index) {node_index = _node_index; updateNode(); return *this;}

VarType DBNodeVisitor::getType() {return node_descriptor.type;}
VarTypeClass DBNodeVisitor::getTypeClass() {return toTypeClass(getType());}

bool DBNodeVisitor::isPrimitive() {return getTypeClass() == VarTypeClass::primitive;}
bool DBNodeVisitor::isBufferArray() {return getTypeClass() == VarTypeClass::buffer_array;}
bool DBNodeVisitor::isArray() {return getTypeClass() == VarTypeClass::array;}
bool DBNodeVisitor::isObject() {return getTypeClass() == VarTypeClass::object;}

DBNodeVisitor& DBNodeVisitor::stepInside(ui64 index) {
  if(isPrimitive() || isObject() || is_value_ptr) throw SException(ErrCode::binom_invalid_type);
  if(isArray()) {
    if(node_descriptor.size / sizeof (f_virtual_index) >= index) throw SException(ErrCode::binom_out_of_range);
    node_index = fvmc.loadHeapDataPartByIndex(node_descriptor.index,
                                              index*sizeof(f_virtual_index),
                                              sizeof (f_virtual_index))
                 .get<f_virtual_index>(0);
    updateNode();
  } else if(isBufferArray()) {
    if(node_descriptor.size / toSize(toValueType(getType())) >= index) throw SException(ErrCode::binom_out_of_range);
    value_index = index;
    is_value_ptr = true;
  }
  return *this;
}

DBNodeVisitor& DBNodeVisitor::stepInside(BufferArray name) {
  if(!isObject() || is_value_ptr) throw SException(ErrCode::binom_invalid_type);

}

DBNodeVisitor DBNodeVisitor::getChild(ui64 index) {return DBNodeVisitor(*this).stepInside(index);}
DBNodeVisitor DBNodeVisitor::getChild(BufferArray name) {return DBNodeVisitor(*this).stepInside(std::move(name));}

DBNodeVisitor DBNodeVisitor::operator[](ui64 index) {return DBNodeVisitor(*this).stepInside(index);}
DBNodeVisitor DBNodeVisitor::operator[](BufferArray name) {return DBNodeVisitor(*this).stepInside(std::move(name));}

DBNodeVisitor& DBNodeVisitor::operator()(ui64 index) {return stepInside(index);}
DBNodeVisitor& DBNodeVisitor::operator()(BufferArray name) {return stepInside(std::move(name));}
