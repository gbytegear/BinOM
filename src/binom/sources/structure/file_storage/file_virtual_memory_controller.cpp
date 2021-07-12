#include "binom/includes/structure/file_storage/file_virtual_memory_controller.h"

// Disclamer:
// If you opened this file with the intention of changing it, I must tell you ...
// "Prepare to feel much pain"
// Thats f@ckin shit control memory of file when DBNodeVisitor create/delete/change
// BinOM node

// Good luck and don't break anything here

using namespace binom;



void FileVirtualMemoryController::init() {
  // If file is empty
  // -> create database header
  // -!-> read database header & parse pages & heap blocks
  if(file.isEmpty()) {
    file.write(0, header);
  } else {
    file.read(0, header);

    // Parse node pages
    if(header.first_node_page_index) {
      f_real_index next_node_page_index = header.first_node_page_index;
      NodePageDescriptor page_descriptor;
      while (next_node_page_index) {
        file.read(next_node_page_index, page_descriptor);
        node_page_list.insertPage(next_node_page_index, page_descriptor);
        next_node_page_index = page_descriptor.next_node_page;
      }
    }

    // Parse heap pages
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

    // Parse byte pages
    if(header.first_byte_page_index) {
      f_real_index next_byte_page_index = header.first_byte_page_index;
      BytePageDescriptor page_descriptor;
      while (next_byte_page_index) {
        file.read(next_byte_page_index, page_descriptor);
        byte_page_list.insertPage(next_byte_page_index, page_descriptor);
        next_byte_page_index = page_descriptor.next_byte_page;
      }
    }


    // If root type is complex(buffer_array/array/object)
    // -> alloc in heap list block with index and size from root node descriptor
    switch (toTypeClass(header.root_node.type)) {
      case VarTypeClass::buffer_array:
      case VarTypeClass::array:
      case VarTypeClass::object:
        heap_block_list.allocBlock(header.root_node.index, header.root_node.size);
      default:;
    }

    // Parse heap blocks
    for(NodePageVector::PageNode& page : node_page_list)
      for(BitIterator it : page.descriptor.node_map)
        if(it.get()) {
          NodeDescriptor descriptor;
          file.read(page.index + sizeof (NodePageDescriptor) + it.getBitIndex()*sizeof (NodeDescriptor), descriptor);
          switch (toTypeClass(descriptor.type)) {
            case VarTypeClass::buffer_array:
            case VarTypeClass::array:
            case VarTypeClass::object:
              if(descriptor.size == 0) continue; // For empty containers
              heap_block_list.allocBlock(descriptor.index, descriptor.size);
            continue;
            default:continue;
          }
        }

  }
}


// Memory page create methods

void FileVirtualMemoryController::createNodePage() {
  f_real_index node_page_index = file.addSize(node_page_size); // Alloc memory
  NodePageDescriptor page_descriptor;
  file.write(node_page_index, page_descriptor); // Write node page header in start of node page

  // If node page list is empty
  // -> Write real index of created page in database header
  // -!-> Write real index of created page in last node page header
  if(node_page_list.isEmpty()) {
    header.first_node_page_index = node_page_index;
    file.write(offsetof(DBHeader, first_node_page_index), node_page_index);
  } else {
    file.write(node_page_list.last().index + offsetof(NodePageDescriptor, next_node_page), node_page_index);
  }
  node_page_list.insertPage(node_page_index, page_descriptor); // Add to list
}

void FileVirtualMemoryController::createHeapPage() {
  f_real_index heap_page_index = file.addSize(heap_page_size); // Alloc memory
  HeapPageDescriptor page_descriptor;
  file.write(heap_page_index, page_descriptor); // Write heap page header in start of heap page

  // If heap page list is empty
  // -> Write real index of created page in database header
  // -!-> Write real index of created page in last heap page header
  if(heap_page_list.isEmpty()) {
    header.first_heap_page_index = heap_page_index;
    file.write(offsetof(DBHeader, first_heap_page_index), heap_page_index);
  } else {
    file.write(heap_page_list.last().index + offsetof(HeapPageDescriptor, next_heap_page), heap_page_index);
  }
  heap_page_list.insertPage(heap_page_index, page_descriptor); // Add to list

  // Add memory size to last heap block
  // or create new free heap block in the list end, if last heap block is busy
  heap_block_list.addMemory(heap_page_size - sizeof(HeapPageDescriptor));
}

void FileVirtualMemoryController::createBytePage() {
  f_real_index byte_page_index = file.addSize(byte_page_size); // Alloc memory
  BytePageDescriptor page_descriptor;
  file.write(byte_page_index, page_descriptor); // Write byte page header in start of byte page

  // If byte page list is empty
  // -> Write real index of created page in database header
  // -!-> Write real index of created page in last byte page header
  if(byte_page_list.isEmpty()) {
    header.first_byte_page_index = byte_page_index;
    file.write(offsetof(DBHeader, first_byte_page_index), byte_page_index);
  } else {
    file.write(byte_page_list.last().index + offsetof(BytePageDescriptor, next_byte_page), byte_page_index);
  }
  byte_page_list.insertPage(byte_page_index, page_descriptor); // Add to list
}


// Virtual index translation methods

f_real_index FileVirtualMemoryController::getRealNodePos(f_virtual_index v_index) {
  if(v_index == 0) return offsetof(DBHeader, root_node); // Return root node
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
  // The root node is not involved in finding free nodes
  //  if(header.root_node.type == VarType::end) {
  //    header.root_node = descriptor;
  //    file.write(offsetof(DBHeader, root_node), descriptor);
  //    return 0;
  //  }

  if(!header.first_node_page_index)
    createNodePage();

  f_virtual_index node_virtual_index = 1; // Calculated virtual index

  for(NodePageVector::PageIterator it = node_page_list.begin();
      it != node_page_list.end();
      ++it
      ) { // Loop through node pages
    NodePageVector::PageNode& page = *it;
    ui8 index_in_page = 0; // Shift from start of node page

    // Try to find free node by header bitmap
    for(BitIterator& bit : page.descriptor.node_map) {
      if(!bit.get()) { // Finded free position
        // Mark node position as busy
        bit.set(true);
        file.write(page.index + offsetof(NodePageDescriptor, node_map), page.descriptor.node_map);
        // Write node descriptor to finded
        file.write(page.index + sizeof(NodePageDescriptor) + index_in_page * sizeof(NodeDescriptor), descriptor);
        return node_virtual_index;
      }
      ++index_in_page;
      ++node_virtual_index;
    }

    // If all pages are busy, create a new page
    if(it.isLast()) createNodePage();
  } // NOTE: Infinity loop
  throw Exception(ErrCode::binomdb_memory_management_error, "Out of memory allocation loop!");
}

void FileVirtualMemoryController::setNode(f_virtual_index v_index, NodeDescriptor descriptor) {
  file.write(getRealNodePos(v_index), descriptor);
}

NodeDescriptor FileVirtualMemoryController::loadNodeDescriptor(f_virtual_index v_index) {
  NodeDescriptor descriptor;
  file.read(getRealNodePos(v_index), descriptor);
  return descriptor;
}

void FileVirtualMemoryController::clear() { // Reconstruct
  file.resize(0);
//  std::string filename(file.path().string());

  node_page_list.~NodePageVector();
  new(&node_page_list) NodePageVector();
  byte_page_list.~BytePageVector();
  new(&byte_page_list) BytePageVector();
  heap_page_list.~HeapPageVector();
  new(&heap_page_list) HeapPageVector();
  heap_block_list.~MemoryBlockList();
  new(&heap_block_list) MemoryBlockList();

  init();
}

void FileVirtualMemoryController::freeNode(f_virtual_index v_index) {
  if(v_index == 0) {
    header.root_node.type = VarType::end;
    file.write(offsetof(DBHeader, root_node), header.root_node);
    return;
  }
  NodePageVector::PageNode& page = node_page_list[(v_index - 1)/64];
  page.descriptor.node_map.set((v_index - 1)%64, false);
  file.write(page.index + offsetof(NodePageDescriptor, node_map), page.descriptor.node_map);
}

ByteArray FileVirtualMemoryController::getRealHeapBlocks(f_virtual_index index, f_size size) {
  ByteArray result;
  HeapPageVector::PageIterator it = heap_page_list.begin();
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
    if((++it).isEnd()) throw Exception(ErrCode::binomdb_page_isnt_exist);
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
  if(data_block.isEmpty()) throw Exception(ErrCode::binomdb_memory_management_error);
  ByteArray real_block_array = getRealHeapBlocks(data_block.index, data_block.size);
  ByteArray data(data_block.size);
  ByteArray::iterator data_it = data.begin();
  for(RealBlock* it = real_block_array.begin<RealBlock>();
      it != real_block_array.end<RealBlock>();
      ++it)
    data_it = file.read(it->r_index, data_it, it->size);
  return data;
}

f_virtual_index FileVirtualMemoryController::allocByteData(ValType type, ByteArray data) {
  f_virtual_index block_index = allocByteBlock(type); // TODO: Error (return 0xffffffffffffffff)
  setByteData(block_index, type, data);
  return block_index;
}

f_virtual_index FileVirtualMemoryController::allocByteBlock(ValType type) {
  const ui8 need_byte_count = toSize(type);
  ui8 byte_count = 0;

  if(byte_page_list.isEmpty()) createBytePage();

  f_virtual_index index = 0;
  for(BytePageVector::PageIterator it = byte_page_list.begin();
      it != byte_page_list.end();
      ++it
      ) {
    BytePageVector::PageNode& page = *it;
    if(page.descriptor.byte_map.value() == 0xFFFFFFFFFFFFFFFFull) {
      index += 64;
      // if all page is busy
      if(it.isLast()) createBytePage(); // add new page if this page last
      continue;
    } else {
      for(BitIterator bit : page.descriptor.byte_map)
        if(!bit.get()) {
          // if byte is free
          if(++byte_count == need_byte_count) {
            // all bytes is found
            for(BitIterator bit_it = bit - (byte_count-1); bit_it <= bit; ++bit_it)
              // set byte flags as busy
              bit_it.set(true);
            file.write(page.index + offsetof(BytePageDescriptor, byte_map), page.descriptor.byte_map);
            return index;
          }
          continue;
        } else {
          if(!byte_count) {++index;continue;}
          index += byte_count;
          byte_count = 0;
        }
      index += byte_count;
      byte_count = 0;
      if(it.isLast())
        createBytePage(); // add new page if this page last
      continue;
    }
  } // NOTE: Infinity loop
  throw Exception(ErrCode::binomdb_memory_management_error, "Out of memory allocation loop!");
}

void FileVirtualMemoryController::setByteData(f_virtual_index index, ValType type, ByteArray data) {
  file.write(getRealBytePos(index), (void*)data.begin(), toSize(type));
}

ByteArray FileVirtualMemoryController::loadByteData(f_virtual_index index, ValType type) {
  ByteArray data(toSize(type));
  file.read(getRealBytePos(index), data);
  return data;
}

void FileVirtualMemoryController::freeByteData(f_virtual_index index, ValType type) {
  BytePageVector::PageNode& page = byte_page_list[index / 64];
  BitIterator bit = page.descriptor.byte_map.begin() + index%64;
  const ui8 size = toSize(type);
  for(ui8 i = 0;i < size;(++bit,++i))
    bit.set(false);
  file.write(page.index + offsetof(BytePageDescriptor, byte_map), page.descriptor.byte_map);
}

f_virtual_index FileVirtualMemoryController::createNode(VarType type, ByteArray data) {
  switch (toTypeClass(type)) {
    default: throw Exception(ErrCode::binom_invalid_type);
    case VarTypeClass::primitive: {
      NodeDescriptor descriptor{type, allocByteData(toValueType(type), data)};
      return allocNode(descriptor);
    }
    case VarTypeClass::buffer_array:
    case VarTypeClass::array:
    case VarTypeClass::object:
      VMemoryBlock data_block = (data.isEmpty())? VMemoryBlock{0,0} : allocHeapData(data);
      NodeDescriptor descriptor{type, data_block.v_index, data_block.size};
    return allocNode(descriptor);
  }
}


void FileVirtualMemoryController::updateNode(f_virtual_index node_index, VarType type, ByteArray data) {

  switch (toTypeClass(type)) { // Alloc new data block
    default: throw Exception(ErrCode::binom_invalid_type);
    case VarTypeClass::primitive: {
      NodeDescriptor descriptor{type, allocByteData(toValueType(type), data)};
      setNode(node_index, descriptor);
      return;
    }
    case VarTypeClass::buffer_array:
    case VarTypeClass::array:
    case VarTypeClass::object: {
      VMemoryBlock data_block = (data.isEmpty())? VMemoryBlock{0,0} : allocHeapData(data);
      NodeDescriptor descriptor{type, data_block.v_index, data_block.size};
      setNode(node_index, descriptor);
    }
  }

}

void FileVirtualMemoryController::freeNodeData(f_virtual_index node_index) {
  NodeDescriptor descriptor(loadNodeDescriptor(node_index));
  switch (toTypeClass(descriptor.type)) { // Free data block
    default: throw Exception(ErrCode::binom_invalid_type);
    case VarTypeClass::primitive:
      freeByteData(descriptor.index, toValueType(descriptor.type));
    return;
    case VarTypeClass::buffer_array:
    case VarTypeClass::array:
    case VarTypeClass::object:
      if(descriptor.index == 0 &&
         descriptor.size == 0) return;
      freeHeapData(descriptor.index);
    return;
  }
}

ByteArray FileVirtualMemoryController::loadDataByNode(f_virtual_index node_index) {
  NodeDescriptor descriptor(loadNodeDescriptor(node_index));
  if(descriptor.size == 0) return ByteArray();
  switch (toTypeClass(descriptor.type)) {
    default: throw Exception(ErrCode::binom_invalid_type);
    case VarTypeClass::primitive: return loadByteData(descriptor.index, toValueType(descriptor.type));
    case VarTypeClass::buffer_array:
    case VarTypeClass::array:
    case VarTypeClass::object: return loadHeapData(descriptor.index);
  }
}

ByteArray FileVirtualMemoryController::loadHeapDataPartByIndex(f_virtual_index heap_index, f_real_index shift, f_size size) {
  if(!size) return ByteArray();
  MemoryBlockList::MemoryBlock data_block = heap_block_list.findBlock(heap_index);
  if(data_block.isEmpty()) throw Exception(ErrCode::binomdb_memory_management_error);
  ByteArray real_block_array = getRealHeapBlocks(data_block.index, data_block.size);

  ByteArray data(size);
  ByteArray::iterator data_it = data.begin();

  for(RealBlock* it = real_block_array.begin<RealBlock>(); it != real_block_array.end<RealBlock>(); ++it)
    if(it->size <= shift) { // Search start real block

      shift -= it->size;
      continue;

    } else {

      if(shift) {
        f_size r_size = (size < it->size)? size : it->size - shift;
        data_it = file.read(it->r_index + shift, data_it, r_size);
        size -= r_size;
        shift = 0;
        if(size) continue;
        else return data;
      } else {
        f_size r_size = (size < it->size)? size : it->size;
        data_it = file.read(it->r_index, data_it, r_size);
        size -= r_size;
        if(size) continue;
        else return data;
      }

    }

  return data;
}

void FileVirtualMemoryController::free(f_virtual_index node_index) {
  NodeDescriptor descriptor(loadNodeDescriptor(node_index));
  freeNode(node_index);
  switch (toTypeClass(descriptor.type)) { // Free data block
    default: throw Exception(ErrCode::binom_invalid_type);
    case VarTypeClass::primitive:
      freeByteData(descriptor.index, toValueType(descriptor.type));
    return;
    case VarTypeClass::buffer_array:
    case VarTypeClass::array:
    case VarTypeClass::object:
      freeHeapData(descriptor.index);
    return;
  }
}

void FileVirtualMemoryController::markNodeAsBusy(f_virtual_index node_index) {
  if(node_index == 0) return;
  NodePageVector::PageNode& page = node_page_list[(node_index - 1)/64];
  page.descriptor.node_map.set((node_index - 1)%64, true);
  file.write(page.index + offsetof(NodePageDescriptor, node_map), page.descriptor.node_map);
}

bool FileVirtualMemoryController::isBusyNode(f_virtual_index node_index) {
  if(!node_index) {
    return header.root_node.type != VarType::end;
  }
  return node_page_list[(node_index - 1)/64].descriptor.node_map.get((node_index - 1)%64);
}

void FileVirtualMemoryController::setHeapDataPartByNode(f_virtual_index node_index, f_real_index shift, ByteArray data) {
  // Validations:
  if(!data.length()) return;
  NodeDescriptor descriptor = loadNodeDescriptor(node_index);
  switch (toTypeClass(descriptor.type)) {
  case binom::VarTypeClass::invalid_type:
  case binom::VarTypeClass::primitive: throw Exception(ErrCode::binom_invalid_type);
  case binom::VarTypeClass::buffer_array:
  case binom::VarTypeClass::array:
  case binom::VarTypeClass::object: break;
  }
  if(shift + data.length() >= descriptor.size)
    throw Exception(ErrCode::binomdb_memory_management_error, "Out of memory block range");
  f_real_index index = getRealHeapPos(descriptor.index);
  file.write(index + shift, data);
}
