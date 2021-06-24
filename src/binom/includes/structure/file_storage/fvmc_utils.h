#ifndef FVMC_UTILS_H
#define FVMC_UTILS_H

#include "file_structs.h"
#include "file_io.h"
#include "../rwguard.h"

namespace binom {

class FileVirtualMemoryController;
class MemoryBlockList;

template<typename DescriptorType>
class PageVector {
public:

private:
  ByteArray data;
public:

  struct PageNode {
    DescriptorType descriptor;
    f_real_index index = 0;
  };

  class PageIterator {
    ByteArray* ptr = nullptr;
    ui64 index = 0;
  public:
    PageIterator(ByteArray* ptr, ui64 index) : ptr(ptr), index(index) {}
    PageIterator(PageIterator& other) : ptr(other.ptr), index(other.index) {}
    PageIterator(PageIterator&& other) : ptr(other.ptr), index(other.index) {}

    PageIterator& operator++() {++index; return *this;}
    PageNode& operator*() {return ptr->get<PageNode>(index);}
    PageNode* operator->() {return &ptr->get<PageNode>(index);}
    bool operator==(PageIterator other) const {return ptr == other.ptr && index == other.index;}
    bool operator!=(PageIterator other) const {return ptr != other.ptr || index != other.index;}
    PageIterator& operator+=(ui64 index) {
      this->index += index;
      return *this;
    }

    bool isLast() {return &ptr->get<PageNode>(index) == (ptr->end<PageNode>() - 1);}
    bool isEnd() {return &ptr->get<PageNode>(index) == ptr->end<PageNode>();}

  };

  PageVector() = default;
  PageVector(const PageVector& other) : data(other.data) {}
  PageVector(PageVector&& other) : data(std::move(other.data)) {other.data.unfree();}

  PageNode& last() {return data.last<PageNode>();}
  PageNode& first() {return data.first<PageNode>();}

  bool isEmpty() {return data.isEmpty();}

  PageIterator begin() {return {&data, 0};}
  PageIterator end() {return {&data, data.length<PageNode>()};}

  PageNode& operator[](ui64 index) {return data.get<PageNode>(index);}

  PageNode* insertPage(f_real_index index, DescriptorType descriptor) {
    data.pushBack<PageNode>({descriptor, index});
    return &data.last<PageNode>();
  }

  ui64 getPageCount() {return data.length<PageNode>();}

};

template class PageVector<NodePageDescriptor>;
template class PageVector<BytePageDescriptor>;
template class PageVector<HeapPageDescriptor>;

typedef PageVector<NodePageDescriptor> NodePageVector;
typedef PageVector<BytePageDescriptor> BytePageVector;
typedef PageVector<HeapPageDescriptor> HeapPageVector;


/*
 * {0, 5} <-> {5, 10}
 *
 * {0, 5}.split(3)
 *
 * {0, 3} <-> {3, 2} <-> {5, 10};
*/

class MemoryBlockList {
  FileVirtualMemoryController* parent();
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

  MemoryBlockList() = default;
  MemoryBlockList(const MemoryBlockList& other) = delete;
  MemoryBlockList(MemoryBlockList&& other)
    : _first(other._first),
      _last(other._last) {
    if(_first.next)
      _first.next->prev = &_first;
    other._first = MemoryBlock{};
    other._last = &other._first;
  }

  ~MemoryBlockList() {

  }

  MemoryBlock& first() {return _first;}
  MemoryBlock& last() {return *_last;}

  void addMemory(f_size size);

  MemoryBlock allocBlock(f_size size);
  MemoryBlock findBlock(f_virtual_index index);
  void freeBlock(f_virtual_index index);
  MemoryBlock allocBlock(f_virtual_index index, f_size size);

  MemoryBlockIterator begin() {return &_first;}
  MemoryBlockIterator end() {return nullptr;}

};



// LEGACY

/*
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
    PageIterator& operator+=(ui64 index) {
      while (index != 0) {
        if(!current) throw Exception(ErrCode::binomdb_page_isnt_exist);
        current = current->next;
        --index;
      }
      return *this;
    }

    bool isEnd() {return !current;}

  };

private:
  PageNode first_page;
  PageNode* last_page = &first_page;
public:
  PageList() = default;
  PageList(const PageList& other) = delete;
  PageList(PageList&& other)
    : first_page(other.first_page),
      last_page(other.last_page) {
    other.first_page = PageNode{};
    other.last_page = &other.first_page;
  }

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
    throw Exception(ErrCode::binomdb_page_isnt_exist);
  }

  PageNode* insertPage(f_real_index index, DescriptorType descriptor) {
    if(isEmpty()) return &(first_page = PageNode{nullptr, descriptor, index});
    else return last_page = last_page->next = new PageNode{nullptr, descriptor, index};
  }

  ui64 getPageCount() {
    if(!first_page.index) return 0;
    ui64 count = 0;
    for([[maybe_unused]]PageNode& node : *this) ++count;
    return count;
  }

};

template class PageList<NodePageDescriptor>;
template class PageList<BytePageDescriptor>;
template class PageList<HeapPageDescriptor>;

typedef PageList<NodePageDescriptor> NodePageList;
typedef PageList<BytePageDescriptor> BytePageList;
typedef PageList<HeapPageDescriptor> HeapPageList;
*/


}


#endif // FVMC_UTILS_H
