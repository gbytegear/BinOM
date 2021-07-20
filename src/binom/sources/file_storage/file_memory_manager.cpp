#include "binom/includes/file_storage/file_memory_manager.h"

using namespace binom;

FMemoryManager::FMemoryManager(std::string_view file_path) : file(file_path) {
  init();
}

void FMemoryManager::init() {
  if(file.isEmpty()) {
    file.resize(sizeof(db_header));
    file.write(0, db_header);
    return;
  }

  file.read(db_header, 0);

  switch (db_header.checkFileVersion()) {
    case binom::DBHeader::VersionDifference::identical: break;
    case binom::DBHeader::VersionDifference::file_type:
      throw Exception(ErrCode::binomdb_invalid_file, "Invalid file type");
    case binom::DBHeader::VersionDifference::major:
      std::cerr << "Warning: major version difference!\n";
    break;
    case binom::DBHeader::VersionDifference::minor:
      std::cerr << "Warning: minor version difference!\n";
    break;
  }

  if(db_header.first_node_page_index) {
    real_index next_page_index = db_header.first_node_page_index;
    NodePageInfo page_info;
    while (next_page_index) {
      page_info.page_position = next_page_index;
      file.read(page_info.descriptor, next_page_index);
      node_page_vector.push_back(page_info);
    }
  }

  if(db_header.first_heap_page_index) {
    real_index next_page_index = db_header.first_heap_page_index;
    HeapPageInfo page_info;
    while (next_page_index) {
      page_info.page_position = next_page_index;
      file.read(page_info.descriptor, next_page_index);
      heap_page_vector.push_back(page_info);
      heap_map.expandMemory(heap_page_data_size);
    }
  }

  switch (toTypeClass(db_header.root_node.type)) {
    case VarTypeClass::buffer_array:
    case VarTypeClass::array:
    case VarTypeClass::object:
      heap_map.occupyBlock(db_header.root_node.index, db_header.root_node.size);
    default: break;
  }

  for(auto node_page_info : node_page_vector) {
    for(auto bit : node_page_info.descriptor.node_map)
      if(bit.get()) {
        NodeDescriptor descriptor;
        file.read(descriptor,
                  node_page_info.page_position +
                  sizeof (NodePageDescriptor) +
                  bit.getBitIndex()*sizeof (NodeDescriptor));
        switch (toTypeClass(descriptor.type)) {
          case VarTypeClass::buffer_array:
          case VarTypeClass::array:
          case VarTypeClass::object:
            if(descriptor.size == 0) continue; // For empty containers
            heap_map.occupyBlock(descriptor.index, descriptor.size);
          continue;
          default:continue;
        }
      }
  }

}

void FMemoryManager::allocNodePage() {
  NodePageInfo& last = node_page_vector.back();
  NodePageDescriptor descriptor;
  last.descriptor.next_node_page = file.addSize(node_page_size);
  file.write(last.descriptor.next_node_page, descriptor);
  node_page_vector.push_back(NodePageInfo{descriptor, last.descriptor.next_node_page});
}

void FMemoryManager::allocHeapPage() {
  std::scoped_lock lk(heap_page_mtx);
  HeapPageDescriptor descriptor;
  HeapPageInfo& last = heap_page_vector.back();
  last.descriptor.next_heap_page = file.addSize(heap_page_size);
  file.write(last.descriptor.next_heap_page, descriptor);
  heap_page_vector.push_back(HeapPageInfo{descriptor, last.descriptor.next_heap_page});
  heap_map.expandMemory(heap_page_data_size);
}

real_index FMemoryManager::translateVNodeIndex(virtual_index v_index) {
  if(v_index == 0) return offsetof(DBHeader, root_node);
  else --v_index;
  return node_page_vector[v_index / 64].page_position +
         sizeof (NodePageDescriptor) +
      (v_index%64)*sizeof(NodeDescriptor);
}

real_index FMemoryManager::translateVHeapIndex(virtual_index v_index) {
  return heap_page_vector[v_index / heap_page_data_size].page_position +
         sizeof (HeapPageDescriptor) +
      v_index%heap_page_data_size;
}

RMemoryBlockVector FMemoryManager::translateVMemoryBlock(VMemoryBlock v_mem_block) {
  RMemoryBlockVector blocks;
  HeapPageVector::iterator it = heap_page_vector.begin() + v_mem_block.v_index / heap_page_data_size;
  {
    real_index in_page_index = v_mem_block.v_index % heap_page_data_size;
    RMemoryBlock r_block{
      it->page_position + sizeof (HeapPageDescriptor) + in_page_index,
      heap_page_data_size - in_page_index
    };

    if(r_block.size >= v_mem_block.size) {
      r_block.size = v_mem_block.size;
      blocks.push_back(r_block);
      return blocks;
    }

    v_mem_block.size -= r_block.size;
    blocks.push_back(r_block);
  }

  while(v_mem_block.size) {
    if(++it == heap_page_vector.cend())
      throw Exception(ErrCode::binomdb_page_isnt_exist, "Virtual heap memory block out of heap pages memory");
    RMemoryBlock r_block {
      it->page_position + sizeof (HeapPageDescriptor),
      heap_page_data_size
    };

    if(r_block.size >= v_mem_block.size) {
      r_block.size = v_mem_block.size;
      blocks.push_back(r_block);
      return blocks;
    }

    v_mem_block.size -= r_block.size;
    blocks.push_back(r_block);
  }
  return blocks;
}

NodeDescriptor FMemoryManager::getNodeDescriptor(virtual_index node_index) {
  return file.read<NodeDescriptor>(translateVNodeIndex(node_index));
}

ByteArray FMemoryManager::getNodeData(virtual_index node_index) {
  ByteArray data;
  NodeDescriptor descriptor = getNodeDescriptor(node_index);
  switch (toTypeClass(descriptor.type)) {
    case binom::VarTypeClass::primitive:
      switch (toValueType(descriptor.type)) {
        case binom::ValType::byte:
          data.pushBack(&descriptor.index, 1);
        break;
        case binom::ValType::word:
          data.pushBack(&descriptor.index, 2);
        break;
        case binom::ValType::dword:
          data.pushBack(&descriptor.index, 4);
        break;
        case binom::ValType::qword:
          data.pushBack(&descriptor.index, 8);
        break;
        case binom::ValType::invalid_type:
          throw Exception(ErrCode::binom_invalid_type);
      }
    return data;

    case binom::VarTypeClass::buffer_array:
    case binom::VarTypeClass::array:
    case binom::VarTypeClass::object: break;

    case binom::VarTypeClass::invalid_type:
      throw Exception(ErrCode::binom_invalid_type);
  }

  RMemoryBlockVector blocks;
  if(VMemoryBlock block = heap_map.find(descriptor.index); block.isEmpty())
    throw Exception(ErrCode::binomdb_memory_management_error, "Virtual heap memory block doesn't finded");
  else blocks = translateVMemoryBlock(block);

  for(auto block : blocks) {
    ByteArray::iterator it = data.addSize(block.size);
    file.readBuffer(it, block.r_index, block.size);
  }

  return data;
}

ByteArray FMemoryManager::getNodeDataPart(virtual_index node_index, real_index shift, block_size size) {
  ByteArray data;
  NodeDescriptor descriptor = getNodeDescriptor(node_index);
  switch (toTypeClass(descriptor.type)) {
    case binom::VarTypeClass::primitive:
      throw Exception(ErrCode::binomdb_memory_management_error, "Get part of data of primitive data types is unsupported operation");

    case binom::VarTypeClass::buffer_array:
    case binom::VarTypeClass::array:
    case binom::VarTypeClass::object: break;

    case binom::VarTypeClass::invalid_type:
      throw Exception(ErrCode::binom_invalid_type);
  }

  RMemoryBlockVector blocks;
  if(VMemoryBlock block = heap_map.find(descriptor.index); block.isEmpty())
    throw Exception(ErrCode::binomdb_memory_management_error, "Virtual heap memory block doesn't finded");
  else blocks = translateVMemoryBlock(block);

  for(auto block : blocks) {
    if(shift) {
      if(shift >= block.size) {
        shift -= block.size;
        continue;
      } else {
        block.r_index += shift;
        block.size -= shift;
        shift = 0;
      }
    }

    if(size >= block.size) {
      ByteArray::iterator it = data.addSize(block.size);
      file.readBuffer(it, block.r_index, block.size);
      size -= block.size;
    } else {
      ByteArray::iterator it = data.addSize(block.size);
      file.readBuffer(it, block.r_index, size);
      return data;
    }
  }

  return data;
}

