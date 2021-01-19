#ifndef VFMEM_H
#define VFMEM_H

#include "file_io.h"
#include "file_structs.h"

namespace binom {

//! Pointer on bit in BitMap. Can be used as iterator.
struct BitPointer {
  BitMap* bitmap = nullptr;
  ui8 bit = 0;
public:
  BitPointer(BitMap* map) : bitmap(map), bit(0) {}
  BitPointer(BitMap* map, ui8 bit) : bitmap(map), bit(bit) {}
  BitPointer(const BitPointer& other) : bitmap(other.bitmap), bit(other.bit) {}
  BitPointer(const BitPointer&& other) : bitmap(other.bitmap), bit(other.bit) {}

  BitPointer& operator=(const BitPointer& other) {bitmap = other.bitmap; bit = other.bit; return *this;}
  BitPointer& operator=(const BitPointer&& other) {bitmap = other.bitmap; bit = other.bit; return *this;}

  bool operator*() {return (*bitmap).get(bit);}

  bool get() {return (*bitmap).get(bit);}
  bool set(bool value) {return (*bitmap).set(bit, value);}

  bool isEnd() {return bit == 64;}
  bool isBegin() {return bit == 0;}

  BitPointer& operator++() {++bit;return *this;}
  BitPointer operator++(int) {BitPointer last(*this);++bit;return last;}
  BitPointer& operator--() {--bit;return *this;}
  BitPointer operator--(int) {BitPointer last(*this);--bit;return last;}
};




//! Virtual File Memory Controller
class VFMemoryController {
public:

  struct MemoryBlock { ui64 index; ui64 size; };

private:

  static constexpr ui64 node_segement_size = sizeof(NodeSegmentDescriptor) + 64*sizeof(NodeDescriptor);
  static constexpr ui64 primitive_segment_size = sizeof(PrimitiveSegmentDescriptor) + 64;




  template<typename Descriptor>
  class SegmentList {
  public:

    struct SegmentNode {
      SegmentNode* next = nullptr;
      Descriptor descriptor;
      MemoryBlock block;
    };

    class SegmentIterator {
      SegmentNode* current = nullptr;
    public:
      SegmentIterator(SegmentNode* node) : current(node) {}

      SegmentIterator& operator++() {current = current->next; return *this;}
      SegmentNode& operator*() {return *current;}
      bool operator==(SegmentIterator& other) const {return current == other.current;}
      bool operator!=(SegmentIterator& other) const {return current != other.current;}
    };

    SegmentNode& last() {return *last_segment;}
    SegmentNode& first() {return first_segment;}

    SegmentNode& operator[](ui64 index) {
      for(SegmentNode& node : *this) {
        if(index == 0) return node;
        --index;
      }
      throw SException(ErrCode::binomdb_segment_isnt_exist);
    }

    SegmentNode* insertSegement(MemoryBlock memory_block, Descriptor descriptor) {
      return last_segment = last_segment->next = new SegmentNode{nullptr, descriptor, memory_block};
    }

    SegmentIterator begin() {return &first_segment;}
    SegmentIterator end() {return nullptr;}

  private:
    SegmentNode first_segment;
    SegmentNode* last_segment = &first_segment;
  };

  typedef SegmentList<NodeSegmentDescriptor> NodeSegmentList;
  typedef SegmentList<PrimitiveSegmentDescriptor> PrimitiveSegmentList;
  typedef SegmentList<DataSegmentDescriptor> DataSegmentList;

  void init();

  NodeSegmentList::SegmentNode& createNodeSegment();
  void loadNodeSegments();
  PrimitiveSegmentList::SegmentNode& createPrimitiveSegment();
  void loadPrimitiveSegments();
  DataSegmentList::SegmentNode& createDataSegment(ui64 size = 4096);
  void loadDataSegments();
  void loadFreeMemoryList();

public:
  VFMemoryController(std::string filename) : file(std::move(filename)) {init();}
  VFMemoryController(const char* filename) : file(filename) {init();}


  // File info
  ui64 getFileSize() {return file.size();}

  ui64 getDataSegmentsSize();
  ui64 getNodeSegmentsSize();
  ui64 getPrimitiveSegmentsSize();

  ui64 getDataSegmentsCount();
  ui64 getNodeSegmentsCount();
  ui64 getPrimitiveSegmentsCount();


  // NodeDescriptor management
  NodeDescriptor getNodeDescriptor(ui64 index);
  ui64 setNodeDescriptor(ui64 index, NodeDescriptor descriptor);
  ui64 setNodeDescriptor(NodeDescriptor descriptor);
  void freeNodeDescriptor(ui64 index);
  ui64 findFreeNodeDescriptor();


  // Primitive memory mangement



  // Data memory magement




private:
  FileIO file;

  DBHeader header;
  NodeSegmentList node_segment_list;
  PrimitiveSegmentList primitive_segment_list;
  DataSegmentList data_segment_list;

};

}

#endif
