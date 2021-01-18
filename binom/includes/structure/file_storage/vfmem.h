#ifndef VFMEM_H
#define VFMEM_H

#include "file_io.h"
#include "file_structs.h"

namespace binom {


//! Virtual File Memory
class VFMemoryController {

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

public:

  VFMemoryController(std::string filename) : file(std::move(filename)) {init();}
  VFMemoryController(const char* filename) : file(filename) {init();}

  ui64 getFileSize() {return file.size();}
  ui64 getDataSegmentsSize();
  ui64 getNodeSegmentsSize();
  ui64 getPrimitiveSegmentsSize();

  ui64 getDataSegmentsCount();
  ui64 getNodeSegmentsCount();
  ui64 getPrimitiveSegmentsCount();

private:
  FileIO file;

  DBHeader header;
  NodeSegmentList node_segment_list;
  PrimitiveSegmentList primitive_segment_list;
  DataSegmentList data_segment_list;

};

}

#endif
