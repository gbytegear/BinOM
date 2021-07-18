#ifndef FMM_CONTAINERS_H
#define FMM_CONTAINERS_H

#define DEBUG

#ifdef DEBUG
#define IF_DEBUG(expr) expr
#include <iostream>
#else
#define IF_DEBUG(expr)
#endif

#include "file_structs.h"
#include <vector>
#include <map>
#include <mutex>

namespace binom {

typedef ui64 virtual_index;
typedef ui64 real_index;
typedef ui64 block_size;

typedef std::vector<NodePageDescriptor> NodePageVector;
typedef std::vector<BytePageDescriptor> BytePageVector;
typedef std::vector<HeapPageDescriptor> HeapPageVector;

struct VMemoryBlock {
  virtual_index v_index = 0;
  block_size size = 0;
  inline bool isEmpty() const {return !v_index && !size;}
};

struct RMemoryBlock {
  real_index r_index;
  block_size size;
};

class HeapMap { // TODO: test it (Doesn't work!)

  struct HeapBlock {
    bool is_busy = false;
    block_size size = 0;
  };

  typedef std::map<virtual_index, HeapBlock> BlockMap;
  typedef BlockMap::iterator block_iterator;
  typedef std::multimap<block_size, block_iterator> FreeBlockMap;
  typedef FreeBlockMap::iterator free_block_iterator;

  BlockMap block_map;
  FreeBlockMap free_block_map;
  std::mutex mtx;

  free_block_iterator findFree(block_iterator block_it) {
    auto range = free_block_map.equal_range(block_it->second.size);
    for(auto it = range.first; it != range.second; ++it)
      if(it->second == block_it) return it;
    return free_block_map.end();
  }

  inline free_block_iterator findFree(block_size size) {return free_block_map.lower_bound(size);}

  void updateSizeOfFreeBlock(block_iterator block_it, block_size old_size) {
    auto range = free_block_map.equal_range(old_size);
    for(auto it = range.first; it != range.second; ++it)
      if(it->second == block_it) {
        auto node = free_block_map.extract(it);
        node.key() = block_it->second.size;
        free_block_map.insert(std::move(node));
        return;
      }
  }

  void splitBlock(block_iterator it, block_size size) {
    if(it->second.size >= size) return;
    block_size old_size = it->second.size;
    block_iterator new_it = block_map.emplace(
                              block_size(it->first + it->second.size),
                              HeapBlock{it->second.is_busy, old_size - size}
                              ).first;
    if(it->second.is_busy) {
      updateSizeOfFreeBlock(it, old_size);
      free_block_map.emplace(new_it->second.size, new_it);
    }
  }

  void deleteFromFree(block_iterator block_it) {
    auto range = free_block_map.equal_range(block_it->second.size);
    for(auto it = range.first; it != range.second; ++it)
      if(it->second == block_it) {
        free_block_map.erase(it);
        return;
      }
  }

  void addToFree(block_iterator block_it) {
    free_block_map.emplace(block_it->second.size, block_it);
  }

  void mergeFreeBlocks(block_iterator first_it, block_iterator second_it) {
    if(first_it == second_it) return;
    elif(first_it->first > second_it->first) std::swap(first_it, second_it);
    block_size old_size = first_it->second.size;
    block_iterator it = first_it;
    do{
      ++it;
      first_it->second.size += it->second.size;
      deleteBlock(it);
    } while(it != second_it);
    updateSizeOfFreeBlock(first_it, old_size);
  }

  void deleteBlock(block_iterator block_it) {
    if(!block_it->second.is_busy) deleteFromFree(block_it);
    block_map.erase(block_it);
  }

  void freeBlock(block_iterator it) {
    it->second.is_busy = false;
    addToFree(it);

    // Find free range
    block_iterator first_it = it;
    block_iterator second_it = it;
    for(block_iterator end = block_map.begin();
        first_it != end && !first_it->second.is_busy;
        --first_it);
    if(first_it->second.is_busy) ++first_it;

    for(block_iterator end = --block_map.end();
        second_it != end && !second_it->second.is_busy;
        ++second_it);
    if(second_it->second.is_busy) --second_it;

    mergeFreeBlocks(first_it, second_it);
  }

  void expandBlock(block_iterator it, block_size add) {
    block_size old_size = it->second.size;
    it->second.size += add;
    if(!it->second.is_busy) updateSizeOfFreeBlock(it, old_size);
  }

public:

  VMemoryBlock allocBlock(block_size size) {
    std::scoped_lock s_lock(mtx);
    free_block_iterator free_block_it = findFree(size);
    if(free_block_it == free_block_map.cend()) return VMemoryBlock{0,0};
    block_iterator block_it = free_block_it->second;
    splitBlock(free_block_it->second, size);
    free_block_map.erase(free_block_it);
    return VMemoryBlock{block_it->first, block_it->second.size};
  }

  void freeBlock(virtual_index v_index) {
    std::scoped_lock s_lock(mtx);
    if(block_iterator it = block_map.find(v_index); it != block_map.cend())
      freeBlock(it);
  }

  void expandMemory(block_size add) {
    std::scoped_lock s_lock(mtx);
    if(block_map.empty()) {
      block_iterator block_it = block_map.emplace(0, HeapBlock{false, add}).first;
      addToFree(block_it);
      return;
    }
    block_iterator it = --block_map.end();
    if(it->second.is_busy) {
      block_map.emplace(it->first + it->second.size, HeapBlock{false, add});
    } else expandBlock(it, add);
  }

  IF_DEBUG(
    void check() {
      for(auto block : block_map) {
        std::clog << "[ index: " << block.first << ", size: " << block.second.size << ", is busy: " << block.second.is_busy << " ]\n";
      }
    }
  )

};

}

#endif // FMM_CONTAINERS_H
