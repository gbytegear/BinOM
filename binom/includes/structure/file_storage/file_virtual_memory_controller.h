#ifndef FILE_VIRTUAL_MEMORY_CONTROLLER_H
#define FILE_VIRTUAL_MEMORY_CONTROLLER_H

#include "file_io.h"
#include "file_structs.h"

namespace binom {


template<typename DescriptorType>
class PageList {
public:
  struct PageNode {
    PageNode* next = nullptr;
    DescriptorType descriptor;
    f_real_index index = 0;
    ~PageNode() {if(next){delete next; next = nullptr;}}
  };

  class PageIterator {
    PageNode* current = nullptr;
  public:
    PageIterator(PageNode* node) : current(node) {}
    PageIterator(PageIterator& other) : current(other.current) {}
    PageIterator(PageIterator&& other) : current(other.current) {}

    PageIterator& operator++() {current = current->next; return *this;}
    PageNode& operator*() {return *current;}
    PageNode* operator->() {return current;}
    bool operator==(PageIterator other) const {return current == other.current;}
    bool operator!=(PageIterator other) const {return current != other.current;}
  };

private:
  PageNode first_page;
  PageNode* last_page = &first_page;
public:

  PageNode& last() {return *last_page;}
  PageNode& first() {return first_page;}

  bool isEmpty() {return first_page.index == 0;}

  PageIterator begin() {return &first_page;}
  PageIterator end() {return nullptr;}

  PageNode& operator[](ui64 index) {
    for(PageNode& node : *this) {
      if(index == 0) return node;
      --index;
    }
    throw SException(ErrCode::binomdb_segment_isnt_exist);
  }

  PageNode* insertPage(f_real_index index, DescriptorType descriptor) {
    if(isEmpty()) return &(first_page = PageNode{nullptr, descriptor, index});
    else return last_page = last_page->next = new PageNode{nullptr, descriptor, index};
  }

};

template class PageList<NodePageDescriptor>;
template class PageList<BytePageDescriptor>;
template class PageList<HeapPageDescriptor>;

typedef PageList<NodePageDescriptor> NodePageList;
typedef PageList<BytePageDescriptor> BytePageList;
typedef PageList<HeapPageDescriptor> HeapPageList;


/*
 * {0, 5} <-> {5, 10}
 *
 * {0, 5}.split(3)
 *
 * {0, 3} <-> {3, 2} <-> {5, 10};
*/

class MemoryBlockList {
public:
  struct MemoryBlock {
    f_virtual_index index = 0;
    f_size size = 0;

    bool used = false;
    MemoryBlock* next = nullptr;
    MemoryBlock* prev = nullptr;

    bool isEmpty() {return (index == 0 && size == 0);}
  };

  static constexpr MemoryBlock empty{0,0,true,nullptr,nullptr};

private:


  void split(MemoryBlock& block, f_size _size);
  void alloc(MemoryBlock& block, f_size _size);
  void free(MemoryBlock& block);

  MemoryBlock _first;
  MemoryBlock* _last = &_first;
public:
  class MemoryBlockIterator {
    MemoryBlock* current;
  public:
    MemoryBlockIterator(MemoryBlock* ptr) : current(ptr) {}
    MemoryBlockIterator(MemoryBlockIterator& other) : current(other.current) {}
    MemoryBlockIterator(MemoryBlockIterator&& other) : current(other.current) {}

    MemoryBlockIterator& operator++() {current = current->next; return *this;}
    MemoryBlockIterator operator++(int) {MemoryBlock* prev = current;current = current->next; return prev;}
    MemoryBlockIterator& operator--() {current = current->prev; return *this;}
    MemoryBlockIterator operator--(int) {MemoryBlock* next = current;current = current->prev; return next;}
    bool operator==(MemoryBlockIterator& other) {return current == other.current;}
    bool operator!=(MemoryBlockIterator& other) {return current != other.current;}
    MemoryBlock& operator*() {return *current;}
  };

  MemoryBlock& first() {return _first;}
  MemoryBlock& last() {return *_last;}

  void addMemory(f_size size);

  MemoryBlock allocBlock(f_size size);
  void freeBlock(f_virtual_index index);
  MemoryBlock allocBlock(f_virtual_index index, f_size size);

  MemoryBlockIterator begin() {return &_first;}
  MemoryBlockIterator end() {return nullptr;}

};





class FileVirtualMemoryController {
public:

private:
public: // WARNING: For testing
  FileIO file;
  NodePageList node_page_list;
  BytePageList byte_page_list;
  HeapPageList heap_page_list;
  MemoryBlockList heap_block_list;

  DBHeader header;

  void init();

  // Page creation
  void createNodePage();
  void createHeapPage();
  void createBytePage();

  // Virtual index translation
  f_real_index getRealNodePos(f_virtual_index v_index);
  f_real_index getRealHeapPos(f_virtual_index v_index);
  f_real_index getRealBytePos(f_virtual_index v_index);

  // Node management
  f_virtual_index allocNode(NodeDescriptor descriptor);
  void setNode(f_virtual_index v_index, NodeDescriptor descriptor);
  void loadNode(f_virtual_index v_index, NodeDescriptor& descriptor);
  void freeNode(f_virtual_index v_index);

public:
  FileVirtualMemoryController(const char* filename) : file(filename) {init();}
  FileVirtualMemoryController(std::string filename) : file(filename) {init();}

  f_virtual_index allocData(const ByteArray data);
  f_virtual_index setData(f_virtual_index node_index);
  ByteArray loadData(f_virtual_index node_index);
  ByteArray loadData(NodeDescriptor descriptor);



};

}

#endif // FILE_VIRTUAL_MEMORY_CONTROLLER_H
