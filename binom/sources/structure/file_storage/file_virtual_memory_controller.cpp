#include "binom/includes/structure/file_storage/file_virtual_memory_controller.h"

using namespace binom;

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

void FileVirtualMemoryController::loadNode(f_virtual_index v_index, NodeDescriptor& descriptor) {
  file.read(getRealNodePos(v_index), descriptor);
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
