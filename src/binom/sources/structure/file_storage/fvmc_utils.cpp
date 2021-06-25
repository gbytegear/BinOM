#include "binom/includes/structure/file_storage/fvmc_utils.h"
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
  else _last = new_block;
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
      else
        _last = &block;
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
        if(!block.next) parent()->createHeapPage();
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
    if(block.index == index) { free(block); return; }
    elif(block.index > index) return;
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
