#include "libbinom/include/file_storage/file_node_iterator.h"

using namespace binom;

FileNodeVisitor::NodeIterator::NodeIterator(FileNodeVisitor& node_visitor, bool is_end)
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
      if(!node_visitor.getElementCount()) return;
      obj_descriptor = node_visitor.fmm
                       ->getNodeDataPart( node_visitor.node_index, 0, sizeof(ObjectDescriptor) )
                       .get<ObjectDescriptor>(0);
      if(is_end) {
        index = obj_descriptor.index_count;
        name_index = 0;
        name_shift = obj_descriptor.name_block_size;
        name_block_descriptor = ObjectNameLength{};
      } else {
        name_block_descriptor = node_visitor.fmm
                                ->getNodeDataPart( node_visitor.node_index, sizeof(ObjectDescriptor), sizeof(ObjectNameLength) )
                                .get<ObjectNameLength>(0);
      }
    break;
    case binom::VarTypeClass::invalid_type:
      throw Exception(ErrCode::binom_invalid_type);
  }
}

FileNodeVisitor::NodeIterator::NodeIterator(FileNodeVisitor& node_visitor, ObjectElementPosition pos)
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

FileNodeVisitor::NodeIterator::NodeIterator(FileNodeVisitor& node_visitor, ui64 index)
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

FileNodeVisitor::NodeIterator::NodeIterator(const binom::FileNodeVisitor::NodeIterator& other)
  : index(other.index),
    obj_descriptor(other.obj_descriptor),
    name_block_index(other.name_block_index),
    name_shift(other.name_shift),
    name_index(other.name_index),
    name_block_descriptor(other.name_block_descriptor),
    container_type(other.container_type),
    node_visitor(other.node_visitor) {}

FileNodeVisitor::NodeIterator::NodeIterator(const binom::FileNodeVisitor::NodeIterator&& other)
  : index(other.index),
    obj_descriptor(other.obj_descriptor),
    name_block_index(other.name_block_index),
    name_shift(other.name_shift),
    name_index(other.name_index),
    name_block_descriptor(other.name_block_descriptor),
    container_type(other.container_type),
    node_visitor(other.node_visitor) {}

bool FileNodeVisitor::NodeIterator::isEnd() {
  switch (toTypeClass(container_type)) {
    case binom::VarTypeClass::primitive: return index == 1;
    case binom::VarTypeClass::buffer_array:
    case binom::VarTypeClass::array: return index == node_visitor.getElementCount();
    case binom::VarTypeClass::object: return index == obj_descriptor.index_count;
    case binom::VarTypeClass::invalid_type: return true;
  }
}

binom::FileNodeVisitor::NodeIterator& binom::FileNodeVisitor::NodeIterator::operator--() {
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
        name_block_descriptor = node_visitor.fmm->getNodeDataPart(
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

binom::FileNodeVisitor::NodeIterator& FileNodeVisitor::NodeIterator::operator++() {
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
          name_block_descriptor = node_visitor.fmm->getNodeDataPart(
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


bool FileNodeVisitor::NodeIterator::operator==(binom::FileNodeVisitor::NodeIterator iterator) const {
  return node_visitor.node_index == iterator.node_visitor.node_index &&
      index == iterator.index;
}

bool FileNodeVisitor::NodeIterator::operator!=(binom::FileNodeVisitor::NodeIterator iterator) const {
  return node_visitor.node_index != iterator.node_visitor.node_index ||
                                    index != iterator.index;
}

FileNodeVisitor FileNodeVisitor::NodeIterator::operator*() {
  if(isEnd()) return FileNodeVisitor(*node_visitor.fmm, nullptr);
  switch (toTypeClass(container_type)) {
    case binom::VarTypeClass::primitive: return node_visitor;
    case binom::VarTypeClass::buffer_array:
    return FileNodeVisitor(*node_visitor.fmm,
                           node_visitor.node_index,
                           index);
    case binom::VarTypeClass::array:
    return FileNodeVisitor(
          *node_visitor.fmm,
          node_visitor.fmm->getNodeDataPart(
            node_visitor.node_index,
            sizeof (virtual_index)*index,
            sizeof (virtual_index))
          .get<virtual_index>(0) );
    case binom::VarTypeClass::object:
    return FileNodeVisitor(
          *node_visitor.fmm,
          node_visitor.fmm->getNodeDataPart(
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
    return FileNodeVisitor(*node_visitor.fmm, nullptr);
  }
}

