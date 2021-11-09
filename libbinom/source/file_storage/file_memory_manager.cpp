#include "libbinom/include/file_storage/file_memory_manager.h"

using namespace binom;

FileMemoryManager::FileMemoryManager(std::string_view file_path, bool force_init) : file(file_path) {init(force_init);}

void FileMemoryManager::init(bool force_init) {
  if(file.isEmpty() || force_init) {
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

    break;
    case binom::DBHeader::VersionDifference::minor:

    break;
  }

  if(db_header.first_node_page_index) {
    real_index next_page_index = db_header.first_node_page_index;
    NodePageInfo page_info;
    while (next_page_index) {
      page_info.page_position = next_page_index;
      file.read(page_info.descriptor, next_page_index);
      node_page_vector.push_back(page_info);
      next_page_index = page_info.descriptor.next_node_page;
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
      next_page_index = page_info.descriptor.next_heap_page;
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

void FileMemoryManager::allocNodePage() {
  std::scoped_lock lk(node_page_mtx);
  NodePageDescriptor descriptor;
  if(node_page_vector.empty()) {
    db_header.first_node_page_index = file.addSize(node_page_size);
    file.write(offsetof(DBHeader, first_node_page_index), db_header.first_node_page_index);
    node_page_vector.push_back(NodePageInfo{descriptor, db_header.first_node_page_index});
    return;
  }
  NodePageInfo& last = node_page_vector.back();
  last.descriptor.next_node_page = file.addSize(node_page_size);
//  file.write(last.descriptor.next_node_page, descriptor);
  file.write(last.page_position + offsetof(NodePageDescriptor, next_node_page), last.descriptor.next_node_page);
  node_page_vector.push_back(NodePageInfo{descriptor, last.descriptor.next_node_page});
}

void FileMemoryManager::allocHeapPage() {
  std::scoped_lock lk(heap_page_mtx);
  HeapPageDescriptor descriptor;
  if(heap_page_vector.empty()) {
    db_header.first_heap_page_index = file.addSize(heap_page_size);
    file.write(offsetof(DBHeader, first_heap_page_index), db_header.first_heap_page_index);
    heap_page_vector.push_back(HeapPageInfo{descriptor, db_header.first_heap_page_index});
    heap_map.expandMemory(heap_page_data_size);
    return;
  }
  HeapPageInfo& last = heap_page_vector.back();
  last.descriptor.next_heap_page = file.addSize(heap_page_size);
  file.write(last.page_position, last.descriptor);
  file.write(last.descriptor.next_heap_page, descriptor);
  heap_page_vector.push_back(HeapPageInfo{descriptor, last.descriptor.next_heap_page});
  heap_map.expandMemory(heap_page_data_size);
}

VMemoryBlock FileMemoryManager::allocHeapBlock(block_size size) {
  if(!size) return VMemoryBlock{0,0};
  VMemoryBlock v_block = heap_map.allocBlock(size);
  while (v_block.isEmpty()) {
    allocHeapPage();
    v_block = heap_map.allocBlock(size);
  }
  return v_block;
}

virtual_index FileMemoryManager::allocNode() {

  virtual_index v_index = 1;
  if(node_page_vector.empty()) allocNodePage();
  for(auto it = node_page_vector.begin(), last = --node_page_vector.end(); true; ++it) {
    // If page is busy
    if(it->descriptor.node_map.value() == 0xFFFFFFFFFFFFFFFF) {
      // If all page is busy
      if(it == last) {
        ui64 offset = it - node_page_vector.begin();
        allocNodePage();
        last = --node_page_vector.end();
        it = node_page_vector.begin() + offset;
      }
      v_index += node_page_node_count;
      continue;
    }

    // Finded page with free node(s)
    for(auto bit : it->descriptor.node_map)
      if(bit.get()) {++v_index; continue;}
      else {
        bit.set(true);
        file.write(it->page_position + offsetof(NodePageDescriptor, node_map), it->descriptor.node_map);
        return v_index;
      }
  }
}

real_index FileMemoryManager::translateVNodeIndex(virtual_index v_index) {
  if(v_index == 0) return offsetof(DBHeader, root_node);
  else --v_index;
  return node_page_vector[v_index / 64].page_position +
         sizeof (NodePageDescriptor) +
      (v_index%64)*sizeof(NodeDescriptor);
}

real_index FileMemoryManager::translateVHeapIndex(virtual_index v_index) {
  return heap_page_vector[v_index / heap_page_data_size].page_position +
         sizeof (HeapPageDescriptor) +
      v_index%heap_page_data_size;
}

RMemoryBlockVector FileMemoryManager::translateVMemoryBlock(VMemoryBlock v_mem_block) {
  RMemoryBlockVector blocks;
  HeapPageVector::iterator it = heap_page_vector.begin() + v_mem_block.v_index / heap_page_data_size;
  {
    real_index in_page_index = v_mem_block.v_index % heap_page_data_size;
    RMemoryBlock r_block{
      it->page_position + sizeof(HeapPageDescriptor) + in_page_index,
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

void FileMemoryManager::writeToVBlock(VMemoryBlock block, ByteArray data, real_index shift) {
  if(block.isEmpty()) return;
  if(shift + data.length() > block.size)
    throw Exception(ErrCode::binomdb_memory_management_error, "Out of memory block");
  RMemoryBlockVector r_blocks = translateVMemoryBlock(block);

  ByteArray::iterator it = data.begin();
  const ByteArray::iterator end = data.end();
  for(auto r_block : r_blocks) {

    if(shift) {
      if(shift >= r_block.size) {
        shift -= r_block.size;
        continue;
      } else {
        r_block.r_index += shift;
        r_block.size -= shift;
        shift = 0;
      }
    }

    if(r_block.size < ui64(end - it)) {
      file.writeBuffer(it, r_block.r_index, r_block.size);
      it += r_block.size;
    } else {
      file.writeBuffer(it, r_block.r_index, end - it);
      return;
    }

  }
}

ByteArray FileMemoryManager::getNodeDataPart(NodeDescriptor descriptor, real_index shift, block_size size) {
  ByteArray data;
  switch (toTypeClass(descriptor.type)) {
    case binom::VarTypeClass::primitive:
      throw Exception(ErrCode::binomdb_memory_management_error, "Get part of data of primitive data types is unsupported operation");

    case binom::VarTypeClass::buffer_array:
    case binom::VarTypeClass::array:
    case binom::VarTypeClass::object: break;

    default:
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
      ByteArray::iterator it = data.addSize(size);
      file.readBuffer(it, block.r_index, size);
      return data;
    }
  }

  return data;
}

NodeFullInfo FileMemoryManager::getFullNodeInfo(virtual_index node_index) {
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

        default:
        case binom::ValType::invalid_type:
          throw Exception(ErrCode::binom_invalid_type);
      }
    return {node_index, descriptor, data};

    case binom::VarTypeClass::buffer_array:
    case binom::VarTypeClass::array:
    case binom::VarTypeClass::object: break;

    default:
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

  return {node_index, descriptor, data};
}

ByteArray FileMemoryManager::getNodeData(NodeDescriptor descriptor) {
  ByteArray data;
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

        default:
        case binom::ValType::invalid_type:
          throw Exception(ErrCode::binom_invalid_type);
      }
    return data;

    case binom::VarTypeClass::buffer_array:
    case binom::VarTypeClass::array:
    case binom::VarTypeClass::object:
      if(!descriptor.size) return ByteArray();
    break;

    default:
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

virtual_index FileMemoryManager::createNode(VarType type, ByteArray data) {

  switch (toTypeClass(type)) {
    case binom::VarTypeClass::primitive: {
      virtual_index node_index = allocNode();
      NodeDescriptor descriptor;
      switch (toValueType(type)) {
        case binom::ValType::byte:
          descriptor = {type, data.get<ui8>(0)};
        break;
        case binom::ValType::word:
          descriptor = {type, data.get<ui16>(0)};
        break;
        case binom::ValType::dword:
          descriptor = {type, data.get<ui32>(0)};
        break;
        case binom::ValType::qword:
          descriptor = {type, data.get<ui64>(0)};
        break;
        case binom::ValType::invalid_type:
          throw Exception(ErrCode::binom_invalid_type);
      }
      file.write(translateVNodeIndex(node_index), descriptor);
      if(!node_index) db_header.root_node = descriptor;
      return node_index;
    }break;

    case binom::VarTypeClass::buffer_array:
    case binom::VarTypeClass::array:
    case binom::VarTypeClass::object: break;

    default:
    case binom::VarTypeClass::invalid_type:
      throw Exception(ErrCode::binom_invalid_type);
  }

  VMemoryBlock v_block = allocHeapBlock(data.length());
  NodeDescriptor descriptor{type, v_block.v_index, v_block.size};
  writeToVBlock(v_block, std::move(data));

  virtual_index node_index = allocNode();
  file.write(translateVNodeIndex(node_index), descriptor);
  if(!node_index) db_header.root_node = descriptor;
  return node_index;

}

void FileMemoryManager::updateNode(virtual_index node_index, VarType type, ByteArray data, NodeDescriptor* descriptor_ptr) {
  NodeDescriptor descriptor = descriptor_ptr? *descriptor_ptr : getNodeDescriptor(node_index);
  // If undefined type
  if(type == VarType::end) type = descriptor.type;
  virtual_index rm_index = virtual_index(-1);

  if(toTypeClass(descriptor.type) != VarTypeClass::primitive &&
     !descriptor.isNoData())
      rm_index = descriptor.index;

  switch (toTypeClass(type)) {

    case binom::VarTypeClass::primitive:
      descriptor.type = type;
      switch (toValueType(type)) {
        case binom::ValType::byte:
          descriptor = {type, data.get<ui8>(0)};
        break;
        case binom::ValType::word:
          descriptor = {type, data.get<ui16>(0)};
        break;
        case binom::ValType::dword:
          descriptor = {type, data.get<ui32>(0)};
        break;
        case binom::ValType::qword:
          descriptor = {type, data.get<ui64>(0)};
        break;
        case binom::ValType::invalid_type:
          throw Exception(ErrCode::binom_invalid_type);
      }
      if(!node_index) {
        removeNode(0);
        db_header.root_node = descriptor;
      }
      file.write(translateVNodeIndex(node_index), descriptor);
      if(rm_index != virtual_index(-1))
        heap_map.freeBlock(rm_index);
    return;




    case binom::VarTypeClass::buffer_array:
    case binom::VarTypeClass::array:
    case binom::VarTypeClass::object: {
      if(rm_index != virtual_index(-1)) {
        if(data.length() == descriptor.size) { // If size of allocated block == size of data for update
          writeToVBlock(VMemoryBlock{descriptor.index, descriptor.size}, std::move(data));
          if(descriptor.type != type) {
            descriptor.type = type;
            file.write(translateVNodeIndex(node_index), descriptor);
          }
          return;
        }
        VMemoryBlock block = allocHeapBlock(data.length());
        writeToVBlock(block, std::move(data));
        descriptor = NodeDescriptor{type, block.v_index, block.size};
        file.write(translateVNodeIndex(node_index), descriptor);
        if(!node_index) db_header.root_node = descriptor;
        heap_map.freeBlock(rm_index);
        return;
      } else {
        VMemoryBlock block = allocHeapBlock(data.length());
        writeToVBlock(block, std::move(data));
        descriptor = NodeDescriptor{type, block.v_index, block.size};
        file.write(translateVNodeIndex(node_index), descriptor);
        if(!node_index) db_header.root_node = descriptor;
        return;
      }
    }




    case binom::VarTypeClass::invalid_type:
      throw Exception(ErrCode::binom_invalid_type);

  }
}

void FileMemoryManager::updateNodeDataPart(virtual_index node_index, real_index shift, ByteArray data, NodeDescriptor* descriptor_ptr) {
  NodeDescriptor descriptor = descriptor_ptr? *descriptor_ptr : getNodeDescriptor(node_index);
  if(toTypeClass(descriptor.type) == VarTypeClass::primitive)
    throw Exception(ErrCode::binomdb_memory_management_error, "Primitive types hasn't heap space");
  writeToVBlock({descriptor.index, descriptor.size}, std::move(data), shift);
}

void FileMemoryManager::insertNodeDataPart(virtual_index node_index, std::multimap<virtual_index, ByteArray> insert_blocks, NodeDescriptor* descriptor_ptr) {
  NodeDescriptor descriptor = descriptor_ptr? *descriptor_ptr : getNodeDescriptor(node_index);
  if(toTypeClass(descriptor.type) == VarTypeClass::primitive)
    throw Exception(ErrCode::binomdb_memory_management_error, "Primitive types hasn't heap space");

  virtual_index rm_index = -1;
  if(!descriptor.isNoData())
      rm_index = descriptor.index;

  ui64 required_memory = descriptor.size;
  for(std::multimap<virtual_index, ByteArray>::reference insert_block : insert_blocks) {
    if(insert_block.first > descriptor.size)
      throw Exception(ErrCode::binomdb_memory_management_error, "Out of memory insertion range");
    required_memory += insert_block.second.length();
  }

  VMemoryBlock block = allocHeapBlock(required_memory);
  ui64 last_index = 0;
  ui64 inserted_size = 0;
  for(std::multimap<virtual_index, ByteArray>::reference insert_block : insert_blocks) {
    if(insert_block.first > last_index) {
      writeToVBlock(block, getNodeDataPart(node_index, last_index, insert_block.first - last_index), last_index + inserted_size);
      last_index = insert_block.first;
    }

    writeToVBlock(block, insert_block.second, last_index + inserted_size);
    inserted_size += insert_block.second.length();
    last_index = insert_block.first;
  }

  if(last_index < descriptor.size) {
    writeToVBlock(block, getNodeDataPart(descriptor, last_index, descriptor.size - last_index), last_index + inserted_size);
  }

  descriptor = NodeDescriptor{descriptor.type, block.v_index, block.size};
  file.write(translateVNodeIndex(node_index), descriptor);
  if(rm_index != virtual_index(-1))heap_map.freeBlock(rm_index);
  if(descriptor_ptr) *descriptor_ptr = descriptor;

}

void FileMemoryManager::removeNodeDataParts(virtual_index node_index, std::list<RMemoryBlock> remove_blocks, NodeDescriptor* descriptor_ptr) {
  NodeDescriptor descriptor = descriptor_ptr? *descriptor_ptr : getNodeDescriptor(node_index);
  if(toTypeClass(descriptor.type) == VarTypeClass::primitive)
    throw Exception(ErrCode::binomdb_memory_management_error, "Primitive types hasn't heap space");
  if(!descriptor.size)
    return;

  virtual_index rm_index = descriptor.index;

  remove_blocks.sort([](RMemoryBlock& left, RMemoryBlock& right)->bool{return left.r_index < right.r_index;});

  ui64 required_memory = descriptor.size;
  { // Calculate required memory
    RMemoryBlock* last_block_ptr = nullptr;
    for(auto it = remove_blocks.begin(), end = remove_blocks.end(); it != end; ++it) {
      RMemoryBlock& remove_block = *it;
      if(last_block_ptr)
        if(ui64 last_end_index = last_block_ptr->r_index + last_block_ptr->size;
           last_end_index >= remove_block.r_index) { // Right edge of prev block in cur block
          if(remove_block.r_index + remove_block.size <= last_end_index) { // Right edge of cur block in prev block
            auto rm = it;
            --it;
            remove_blocks.erase(rm);
            continue;
          }
          ui64 erased_size = remove_block.size - (last_end_index - remove_block.r_index);
          required_memory -= erased_size;
          last_block_ptr->size += erased_size;
          auto rm = it;
          --it;
          remove_blocks.erase(rm);
          continue;
        }

      required_memory -= remove_block.size;
      last_block_ptr = &*it;
      continue;
    }
  }

  VMemoryBlock block = allocHeapBlock(required_memory);
  ui64 last_index = 0;
  ui64 skipped_size = 0;
  for(RMemoryBlock& skip_block : remove_blocks) {
    if(last_index < skip_block.r_index) {
      writeToVBlock(block, getNodeDataPart(descriptor, last_index, skip_block.r_index - last_index), last_index - skipped_size);
    }
    skipped_size += skip_block.size;
    last_index = skip_block.r_index + skip_block.size;
  }

  if(last_index < descriptor.size) {
    writeToVBlock(block, getNodeDataPart(descriptor, last_index, descriptor.size - last_index), last_index - skipped_size);
  }

  descriptor = NodeDescriptor{descriptor.type, block.v_index, block.size};
  file.write(translateVNodeIndex(node_index), descriptor);
  heap_map.freeBlock(rm_index);
  if(descriptor_ptr) *descriptor_ptr = descriptor;
}


void FileMemoryManager::removeNode(virtual_index node_index) {

  if(node_index == 0) {
    node_page_vector.clear();
    heap_page_vector.clear();
    heap_map.clear();
    return init(true);
  }

  NodeDescriptor descriptor = getNodeDescriptor(node_index);

  if(toTypeClass(descriptor.type) != VarTypeClass::primitive)
    heap_map.freeBlock(descriptor.index);

  --node_index; // Without root node
  NodePageInfo& page_info = node_page_vector[node_index / 64];
  page_info.descriptor.node_map.set(node_index%64, false);
  file.write(page_info.page_position, page_info.descriptor);

}
