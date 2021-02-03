#include "binom/includes/structure/file_storage/file_virtual_memory_controller.h"

using namespace binom;


FileVirtualMemoryController* MemoryBlockList::parent() {
  return reinterpret_cast<FileVirtualMemoryController*>(
         reinterpret_cast<byte*>(this) -
         offsetof(FileVirtualMemoryController, heap_block_list));
}

void MemoryBlockList::split(MemoryBlockList::MemoryBlock& block, f_size _size) {
  MemoryBlock* new_block = new MemoryBlock{block.index + _size, block.size - _size, block.used, block.next, &block};
  if(block.next) block.next->prev = new_block;
  block.next = new_block;
  block.size = _size;
}

void MemoryBlockList::alloc(MemoryBlockList::MemoryBlock& block, f_size _size) {
  if(_size != block.size)split(block, _size);
  block.used = true;
}

void MemoryBlockList::free(MemoryBlockList::MemoryBlock& block) {
  block.used = false;

  if(block.next) if(!block.next->used) { // Absorb next
    MemoryBlock* it = block.next;
    while (it && !it->used) {
      block.size += it->size;
      block.next = it->next;
      if(it->next)
        it->next->prev = &block;
      delete it;
      it = block.next;
    }
  }

  if(block.prev) if(!block.prev->used) { // Absorb prev
    MemoryBlock* it = block.prev;
    while (it && !it->used) {
      it->size += it->next->size;
      MemoryBlock* del = it->next;
      it->next = it->next->next;
      if(it->next)
        it->next->prev = it;
      delete del;
      it = it->prev;
    }
  }

}

void MemoryBlockList::addMemory(f_size size) {
  if(_last->used) {
    MemoryBlock* new_block = new MemoryBlock{_last->index + _last->size, size, false, nullptr, _last};
    _last = _last->next = new_block;
  } else {
    _last->size += size;
  }
}

MemoryBlockList::MemoryBlock MemoryBlockList::allocBlock(f_size size) {
  for(MemoryBlock& block : *this)
    if(!block.used && block.size >= size) {
      alloc(block, size);
      return block;
    } else if(!block.next) { // If there is not enough memory in the existing pages
      parent()->createHeapPage();
      if(!block.used) {
        while(block.size < size)
          parent()->createHeapPage();
        alloc(block, size);
        return block;
      } else {
        MemoryBlock& new_page_block = *block.next;
        while(new_page_block.size < size)
          parent()->createHeapPage();
        alloc(new_page_block, size);
        return new_page_block;
      }
  }
  return empty;
}

MemoryBlockList::MemoryBlock MemoryBlockList::findBlock(f_virtual_index index) {
  for(MemoryBlock& block : *this)
    if(block.index == index) return block;
  return empty;
}

void MemoryBlockList::freeBlock(f_virtual_index index) {
  for(MemoryBlock& block : *this)
    if(block.index == index) free(block);
}

MemoryBlockList::MemoryBlock MemoryBlockList::allocBlock(f_virtual_index index, f_size size) {
  for(MemoryBlock& block : *this)
    if(index >= block.index && index < block.index + block.size) {
      if(index != block.index) {
        split(block, index - block.index);
        alloc(*block.next, size);
        return *block.next;
      } else {
        alloc(block, size);
        return block;
      }
    }
  return empty;
}










void FileVirtualMemoryController::init() {
  if(file.isEmpty()) {
    file.write(0, header);
  } else {
    file.read(0, header);

    if(header.first_node_page_index) {
      f_real_index next_node_page_index = header.first_node_page_index;
      NodePageDescriptor page_descriptor;
      while (next_node_page_index) {
        file.read(next_node_page_index, page_descriptor);
        node_page_list.insertPage(next_node_page_index, page_descriptor);
        next_node_page_index = page_descriptor.next_node_page;
      }
    }

    if(header.first_heap_page_index) {
      f_real_index next_heap_page_index = header.first_heap_page_index;
      HeapPageDescriptor page_descriptor;
      while(next_heap_page_index) {
        file.read(next_heap_page_index, page_descriptor);
        heap_page_list.insertPage(next_heap_page_index, page_descriptor);
        next_heap_page_index = page_descriptor.next_heap_page;
        heap_block_list.addMemory(heap_page_size - sizeof(HeapPageDescriptor));
      }
    }

    if(header.first_byte_page_index) {
      f_real_index next_byte_page_index = header.first_byte_page_index;
      BytePageDescriptor page_descriptor;
      while (next_byte_page_index) {
        file.read(next_byte_page_index, page_descriptor);
        byte_page_list.insertPage(next_byte_page_index, page_descriptor);
        next_byte_page_index = page_descriptor.next_byte_page;
      }
    }



    switch (toTypeClass(header.root_node.type)) {
      case VarTypeClass::buffer_array:
      case VarTypeClass::array:
      case VarTypeClass::object:
        heap_block_list.allocBlock(header.root_node.index, header.root_node.size);
      default:;
    }

    for(NodePageList::PageNode& page : node_page_list)
      for(BitIterator it : page.descriptor.node_map)
        if(it.get()) {
          NodeDescriptor descriptor;
          file.read(page.index + sizeof (NodePageDescriptor) + it.getBitIndex()*sizeof (NodeDescriptor), descriptor);
          switch (toTypeClass(descriptor.type)) {
            case VarTypeClass::buffer_array:
            case VarTypeClass::array:
            case VarTypeClass::object:
              heap_block_list.allocBlock(descriptor.index, descriptor.size);
            default:continue;
          }
        }

  }
}

void FileVirtualMemoryController::createNodePage() {
  f_real_index node_page_index = file.addSize(node_page_size);
  NodePageDescriptor page_descriptor;
  file.write(node_page_index, page_descriptor);
  if(node_page_list.isEmpty()) {
    header.first_node_page_index = node_page_index;
    file.write(offsetof(DBHeader, first_node_page_index), node_page_index);
  } else {
    file.write(node_page_list.last().index + offsetof(NodePageDescriptor, next_node_page), node_page_index);
  }
  node_page_list.insertPage(node_page_index, page_descriptor);
}

void FileVirtualMemoryController::createHeapPage() {
  f_real_index heap_page_index = file.addSize(heap_page_size);
  HeapPageDescriptor page_descriptor;
  file.write(heap_page_index, page_descriptor);
  if(heap_page_list.isEmpty()) {
    header.first_heap_page_index = heap_page_index;
    file.write(offsetof(DBHeader, first_heap_page_index), heap_page_index);
  } else {
    file.write(heap_page_list.last().index + offsetof(HeapPageDescriptor, next_heap_page), heap_page_index);
  }
  heap_page_list.insertPage(heap_page_index, page_descriptor);
  heap_block_list.addMemory(heap_page_size - sizeof(HeapPageDescriptor));
  // Add size to memory_map
}

void FileVirtualMemoryController::createBytePage() {
  f_real_index byte_page_index = file.addSize(byte_page_size);
  BytePageDescriptor page_descriptor;
  file.write(byte_page_index, page_descriptor);
  if(byte_page_list.isEmpty()) {
    header.first_byte_page_index = byte_page_index;
    file.write(offsetof(DBHeader, first_byte_page_index), byte_page_index);
  } else {
    file.write(byte_page_list.last().index + offsetof(BytePageDescriptor, next_byte_page), byte_page_index);
  }
  byte_page_list.insertPage(byte_page_index, page_descriptor);
}

f_real_index FileVirtualMemoryController::getRealNodePos(f_virtual_index v_index) {
  if(v_index == 0) return offsetof(DBHeader, root_node);
  return node_page_list[(v_index - 1) / 64].index + sizeof (NodePageDescriptor) + // Get index of page
         ((v_index - 1)%64)*sizeof(NodeDescriptor); // Get offset from start of page
}

f_real_index FileVirtualMemoryController::getRealHeapPos(f_virtual_index v_index) {
  constexpr f_size data_of_heap_page_size = heap_page_size - sizeof(HeapPageDescriptor);
  return heap_page_list[v_index / data_of_heap_page_size].index + sizeof (HeapPageDescriptor) + // Get index of page
      v_index%data_of_heap_page_size;                             // Get offset from start of page
}

f_real_index FileVirtualMemoryController::getRealBytePos(f_virtual_index v_index) {
  return byte_page_list[v_index / 64].index + sizeof (BytePageDescriptor) + // Get index of page
      v_index%64;                             // Get offset from start of page
}

f_virtual_index FileVirtualMemoryController::allocNode(NodeDescriptor descriptor) {
  if(header.root_node.type == VarType::end) {
    header.root_node = descriptor;
    file.write(offsetof(DBHeader, root_node), descriptor);
    return 0;
  }

  if(!header.first_node_page_index)
    createNodePage();

  f_virtual_index node_virtual_index = 1;
  for(NodePageList::PageNode& page : node_page_list) {

    ui8 index_in_page = 0;
    for(BitIterator& bit : page.descriptor.node_map) {
      if(!bit.get()) {
        // Write node
        file.write(page.index + sizeof(NodePageDescriptor) + index_in_page * sizeof(NodeDescriptor), descriptor);
        // Update node bit map
        bit.set(true);
        file.write(page.index + offsetof(NodePageDescriptor, node_map), page.descriptor.node_map);


        return node_virtual_index;
      }

      ++index_in_page;
      ++node_virtual_index;
    }

    // If all pages are busy, create a new page
    if(!page.next)
      createNodePage();
  }

  throw SException(ErrCode::any);
}

void FileVirtualMemoryController::setNode(f_virtual_index v_index, NodeDescriptor descriptor) {
  file.write(getRealNodePos(v_index), descriptor);
}

NodeDescriptor FileVirtualMemoryController::loadNodeDescriptor(f_virtual_index v_index) {
  NodeDescriptor descriptor;
  file.read(getRealNodePos(v_index), descriptor);
  return descriptor;
}

void FileVirtualMemoryController::freeNode(f_virtual_index v_index) {
  if(v_index == 0) {
    header.root_node.type = VarType::end;
    file.write(offsetof(DBHeader, root_node), header.root_node);
    return;
  }
  NodePageList::PageNode& page = node_page_list[(v_index - 1)/64];
  page.descriptor.node_map.set((v_index - 1)%64, false);
  file.write(page.index + offsetof(NodePageDescriptor, node_map), page.descriptor.node_map);
}

ByteArray FileVirtualMemoryController::getRealHeapBlocks(f_virtual_index index, f_size size) {
  ByteArray result;
  HeapPageList::PageIterator it = heap_page_list.begin();
  it += index/heap_data_page_size;
  {
    ui64 index_in_page = index%heap_data_page_size;
    RealBlock block {
      it->index + sizeof (HeapPageDescriptor) + index_in_page,
      heap_data_page_size - index_in_page
    };

    if(block.size >= size) {
      block.size = size;
      result.pushBack(block);
      return result;
    } else {
      size -= block.size;
      result.pushBack(block);
    }
  }

  while (size) {
    if((++it).isEnd()) throw SException(ErrCode::binomdb_page_isnt_exist);
    RealBlock block{
      it->index + sizeof (HeapPageDescriptor),
      heap_data_page_size
    };

    if(block.size >= size) {
      block.size = size;
      result.pushBack(block);
      return result;
    } else {
      size -= block.size;
      result.pushBack(block);
    }
  }

  return result;
}

FileVirtualMemoryController::VMemoryBlock FileVirtualMemoryController::allocHeapData(const ByteArray data) {
  MemoryBlockList::MemoryBlock data_block = heap_block_list.allocBlock(data.length());
  ByteArray real_block_array = getRealHeapBlocks(data_block.index, data_block.size);
  ByteArray::iterator data_it = data.begin();
  for(RealBlock* it = real_block_array.begin<RealBlock>();
      it != real_block_array.end<RealBlock>();
      ++it)
    data_it = file.write(it->r_index, data_it, it->size);
  return {data_block.index, data_block.size};
}

ByteArray FileVirtualMemoryController::loadHeapData(f_virtual_index data_index) {
  MemoryBlockList::MemoryBlock data_block = heap_block_list.findBlock(data_index);
  if(data_block.isEmpty()) throw SException(ErrCode::any); // WARNING: Reaplace any
  ByteArray real_block_array = getRealHeapBlocks(data_block.index, data_block.size);
  ByteArray data(data_block.size);
  ByteArray::iterator data_it = data.begin();
  for(RealBlock* it = real_block_array.begin<RealBlock>();
      it != real_block_array.end<RealBlock>();
      ++it)
    data_it = file.read(it->r_index, data_it, it->size);
  return data;
}

f_virtual_index FileVirtualMemoryController::createNode(VarType type, ByteArray data) {
  switch (toTypeClass(type)) {
  default: throw SException(ErrCode::binom_invalid_type);
  case VarTypeClass::primitive: return createNode(toValueType(type), data.get<ui64>(0));
  case VarTypeClass::buffer_array:
  case VarTypeClass::array:
  case VarTypeClass::object:
    VMemoryBlock data_block = allocHeapData(data);
    NodeDescriptor descriptor{type, data_block.v_index, data_block.size};
    return allocNode(descriptor);
  }
}

f_virtual_index FileVirtualMemoryController::createNode(ValType type, ui64 number) {
  // TODO: ...
}

void FileVirtualMemoryController::updateNode(f_virtual_index node_index, ByteArray data, VarType type) {
  // TODO: ...
}

void FileVirtualMemoryController::updateNode(f_virtual_index node_index, ui64 number, ValType type) {
  // TODO: ...
}

ByteArray FileVirtualMemoryController::loadData(f_virtual_index node_index) {
  NodeDescriptor descriptor(loadNodeDescriptor(node_index));
  switch (toTypeClass(descriptor.type)) {
  default: throw SException(ErrCode::binom_invalid_type);
//  TODO:  case VarTypeClass::primitive: return createNode(toValueType(type), data.get<ui64>(0));
  case VarTypeClass::buffer_array:
  case VarTypeClass::array:
  case VarTypeClass::object: return loadHeapData(descriptor.index);
  }
}

ui64 FileVirtualMemoryController::loadNumber(f_virtual_index node_index) {
  // TODO: ...
}

void FileVirtualMemoryController::free(f_virtual_index node_index) {
  // TODO: ... (Think about Array/Object handling!)
}
