#ifndef FILE_VIRTUAL_MEMORY_CONTROLLER_H
#define FILE_VIRTUAL_MEMORY_CONTROLLER_H

#include "file_io.h"
#include "file_structs.h"

namespace binom {


class FileVirtualMemoryController;

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
        if(!current) throw SException(ErrCode::binomdb_page_isnt_exist);
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
    throw SException(ErrCode::binomdb_page_isnt_exist);
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





class FileVirtualMemoryController {
//public: // WARNING: For testing!!!
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

  struct VMemoryBlock {f_virtual_index v_index;f_size size;};
  struct RealBlock {f_real_index r_index; f_size size;};

  // Node management
  f_virtual_index allocNode(NodeDescriptor descriptor);
  void setNode(f_virtual_index v_index, NodeDescriptor descriptor);
  void freeNode(f_virtual_index v_index);

  // Heap data management
  ByteArray getRealHeapBlocks(f_virtual_index index, f_size size);
  VMemoryBlock allocHeapData(const ByteArray data);
  ByteArray loadHeapData(f_virtual_index data_index);
  void freeHeapData(f_virtual_index index) {heap_block_list.freeBlock(index);}

  // Primitive data management
  constexpr ui8 toSize(ValType type);
  f_virtual_index allocByteBlock(ValType type);
  f_virtual_index allocByteData(ValType type, ByteArray data);
  void setByteData(f_virtual_index index, ValType type, ByteArray data);
  ByteArray loadByteData(f_virtual_index index, ValType type);
  void freeByteData(f_virtual_index index, ValType type);

  friend class MemoryBlockList;
public:
  FileVirtualMemoryController(const char* filename) : file(filename) {init();}
  FileVirtualMemoryController(std::string filename) : file(filename) {init();}

  // DB Props
  f_size  getFileSize() {return file.size();}
  ui64    getNodePageCount() {return node_page_list.getPageCount();}
  ui64    getHeapPageCount() {return heap_page_list.getPageCount();}
  ui64    getBytePageCount() {return byte_page_list.getPageCount();}

  // DB Back-end IO interface
  f_virtual_index createNode(VarType type, ByteArray data);
  void            updateNode(f_virtual_index node_index, VarType type, ByteArray data);
  void            free(f_virtual_index node_index);
  void            markNodeAsBusy(f_virtual_index node_index);

  ByteArray       loadDataByNode(f_virtual_index node_index);
  ByteArray       loadHeapDataByIndex(f_virtual_index heap_index) { return loadHeapData(heap_index); }
  ByteArray       loadHeapDataPartByIndex(f_virtual_index heap_index, f_real_index shift, f_size size);
  ByteArray       loadHeapDataPartByNode(f_virtual_index node_index, f_real_index shift, f_size size) { return loadHeapDataPartByIndex(loadNodeDescriptor(node_index).index, shift, size); }
  ByteArray       loadByteDataByIndex(f_virtual_index byte_index, ValType type) { return loadByteData( byte_index, type); }
  NodeDescriptor  loadNodeDescriptor(f_virtual_index v_index);

  void clear();

};

}

#endif // FILE_VIRTUAL_MEMORY_CONTROLLER_H
