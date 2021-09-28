#include "binom/includes/file_storage/file_node_visitor.h"

using namespace binom;


FileNodeVisitor::NodeIterator::NodeIterator(NodeIterator&& other)
  : fmm(other.fmm), parent_node_index(other.parent_node_index), container_type(other.container_type), data(std::move(other.data)) {other.container_type = VarType::invalid_type;}

FileNodeVisitor::NodeIterator::NodeIterator(FileMemoryManager& fmm, virtual_index parent_node_index, VarType type, ByteArray data)
  : fmm(fmm), parent_node_index(parent_node_index), container_type(type), data(toTypeClass(type), std::move(data)) {}

FileNodeVisitor::NodeIterator::NodeIterator(FileMemoryManager& fmm,
                                            virtual_index parent_node_index,
                                            VarType type,
                                            virtual_index node_index,
                                            size_t count,
                                            real_index index)
                                 : fmm(fmm), parent_node_index(parent_node_index), container_type(type), data(node_index, count, index) {}

FileNodeVisitor::NodeIterator::~NodeIterator() {
  switch (toTypeClass(container_type)) {
    case binom::VarTypeClass::buffer_array:
      data.buffer_array_data.~BufferArrayData();
    return;
    case binom::VarTypeClass::array:
      data.array_data.~ArrayData();
    return;
    case binom::VarTypeClass::object:
      data.object_data.~ObjectData();
    return;
    default: return;
  }
}

FileNodeVisitor::NodeIterator& FileNodeVisitor::NodeIterator::operator++() {
  switch (toTypeClass(container_type)) {
    default: throw Exception(ErrCode::binom_invalid_type);
    case binom::VarTypeClass::buffer_array:
      ++data.buffer_array_data.index;
    break;
    case binom::VarTypeClass::array:
      if(data.array_data.isEmpty()) return *this;
      ++data.array_data.index_it;
    break;
    case binom::VarTypeClass::object:
      if(data.object_data.isEmpty()) return *this;
      if(--data.object_data.name_count) {
        data.object_data.name_it += data.object_data.name_length_it->name_length *
                                    toSize(data.object_data.name_length_it->char_type);
        ++data.object_data.index_it;
      } else {
        data.object_data.name_it += data.object_data.name_length_it->name_length *
                                    toSize(data.object_data.name_length_it->char_type);
        ++data.object_data.index_it;
        ++data.object_data.name_length_it;
        data.object_data.name_count = data.object_data.name_length_it->name_count;
      }
    break;
  }
  return *this;
}

std::optional<BufferArray> FileNodeVisitor::NodeIterator::getName() {
  if(container_type != VarType::object)
    return std::optional<BufferArray>();
  if(data.object_data.isEmpty())
    return std::optional<BufferArray>();
  return BufferArray(data.object_data.name_length_it->char_type,
                     data.object_data.name_it,
                     data.object_data.name_length_it->name_length);
}


bool FileNodeVisitor::NodeIterator::isEnd() {
  switch (toTypeClass(container_type)) {
    case binom::VarTypeClass::buffer_array:
    return data.buffer_array_data.index == data.buffer_array_data.size;
    case binom::VarTypeClass::array:
      if(data.array_data.isEmpty()) return true;
    return data.array_data.index_it == data.array_data.indexes.end<virtual_index>();
    case binom::VarTypeClass::object:
      if(data.object_data.isEmpty()) return true;
    return data.object_data.index_it == data.object_data.indexes.end<virtual_index>();
    default: return true;
  }
}

FileNodeVisitor FileNodeVisitor::NodeIterator::operator*() {
  switch (toTypeClass(container_type)) {
    case binom::VarTypeClass::buffer_array:
    return FileNodeVisitor(fmm, data.buffer_array_data.node_index, data.buffer_array_data.index);
    case binom::VarTypeClass::array:
      if(data.array_data.isEmpty()) return FileNodeVisitor(fmm, nullptr);
    return FileNodeVisitor(fmm, *data.array_data.index_it);
    case binom::VarTypeClass::object:
      if(data.object_data.isEmpty()) return FileNodeVisitor(fmm, nullptr);
    return FileNodeVisitor(fmm, *data.object_data.index_it,
                           NamePosition{
                             parent_node_index,
                             data.object_data.name_length_it->char_type,
                             data.object_data.name_length_it->name_length,
                             sizeof (ObjectDescriptor) + data.object_data.name_lengths.length() + ui64(data.object_data.name_it - data.object_data.names.begin())
                           });
    default: return FileNodeVisitor(fmm, nullptr);
  }
}



