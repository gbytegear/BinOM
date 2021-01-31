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



class FileVirtualMemoryController {
public:

private:
  FileIO file;
  NodePageList node_page_list;
  BytePageList byte_page_list;
  HeapPageList heap_page_list;

  DBHeader header;

public:
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


};

}

#endif // FILE_VIRTUAL_MEMORY_CONTROLLER_H
