#ifndef FMM_CONTAINERS_H
#define FMM_CONTAINERS_H

#include "file_structs.h"
#include <vector>
#include <map>

namespace binom {

typedef ui64 virtual_index;
typedef ui64 real_index;
typedef ui64 block_size;

typedef std::vector<NodePageDescriptor> NodePageVector;
typedef std::vector<BytePageDescriptor> BytePageVector;
typedef std::vector<HeapPageDescriptor> HeapPageVector;

struct VMemoryBlock {
  virtual_index v_index;
  block_size size;
};

struct RMemoryBlock {
  real_index r_index;
  block_size size;
};

class HeapMap {

  struct HeapBlock {
    bool is_busy = false;
    block_size size = 0;
  };

  typedef std::map<virtual_index, HeapBlock> BlockMap;
  typedef BlockMap::iterator block_iterator;
  typedef std::multimap<block_size, block_iterator> FreeBlockMap;

  BlockMap block_map;
  FreeBlockMap free_block_map;

  void deleteFromFree(block_iterator block_it) {
    auto range = free_block_map.equal_range(block_it->second.size);
    for(auto it = range.first; it != range.second; ++it)
      if(it->second == block_it) {
        free_block_map.erase(it);
        return;
      }
  }

  void deleteBlock(block_iterator block_it) {
    if(!block_it->second.is_busy) deleteFromFree(block_it);
    block_map.erase(block_it);
  }

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

  void freeBlock(block_iterator it) {
    it->second.is_busy = false;

    auto first_it = it,
         second_it = it;
    for(auto end = block_map.begin();
        first_it != end && !first_it->second.is_busy;
        --first_it);
    if(first_it->second.is_busy) ++first_it;

    for(auto end = --block_map.end();
        second_it != end && !second_it->second.is_busy;
        ++second_it);
    if(second_it->second.is_busy) --second_it;

    mergeFreeBlocks(first_it, second_it);
  }

  void expandBlock(block_iterator it, block_size add) {
    block_size old_size = it->second.size;
    it->second.size += add;
    if(it->second.is_busy) updateSizeOfFreeBlock(it, old_size);
  }

  // TODO here...
  /* findFreeBlock(block_size size) {return free_block_map.lower_bound(size)}
  */

public:




};

}

#endif // FMM_CONTAINERS_H
