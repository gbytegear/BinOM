#ifndef VFMEM_H
#define VFMEM_H

#include "file_io.h"
#include "file_structs.h"

namespace binom {

class VFMemory {

  static constexpr ui64 node_segement_size = sizeof(NodeSegmentDescriptor) + 64*sizeof(NodeDescriptor);
  static constexpr ui64 primitive_segment_size = sizeof(PrimitiveSegmentDescriptor) + 64;

  struct MemoryBlock { ui64 index; ui64 size; };




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

    SegmentNode* insertSegement(MemoryBlock memory_block, Descriptor descriptor) {
      return last_segment = last_segment->next = new SegmentNode{nullptr, descriptor, memory_block};
    }

    SegmentIterator begin() {return &first_segment;}
    SegmentIterator end() {return nullptr;}

  private:
    SegmentNode first_segment;
    SegmentNode* last_segment = &first_segment;
  };




  class DataSegmentList {
  public:
    struct SegmentNode {
      SegmentNode* next = nullptr;
      MemoryBlock block;
    };

    SegmentNode* insertSegement(MemoryBlock memory_block) {
      return last_segment = last_segment->next = new SegmentNode{nullptr, memory_block};
    }

  private:
    SegmentNode first_segment;
    SegmentNode* last_segment = &first_segment;
  };

  typedef SegmentList<NodeSegmentDescriptor> NodeSegmentList;
  typedef SegmentList<PrimitiveSegmentDescriptor> PrimitiveSegmentList;

  void init();

  NodeSegmentList::SegmentNode& createNodeSegment();
  void loadNodeSegments();
  PrimitiveSegmentList::SegmentNode& createPrimitiveSegment();
  void loadPrimitiveSegments();

public:
  VFMemory(std::string filename) : file(std::move(filename)) {init();}
  VFMemory(const char* filename) : file(filename) {init();}

  ui64 getFileSize() {return file.size();}

private:
  FileIO file;

  DBHeader header;
  NodeSegmentList node_segment_list;
  PrimitiveSegmentList primitive_segment_list;

};

}

#endif
