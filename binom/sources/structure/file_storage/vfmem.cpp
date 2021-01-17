#include "binom/includes/structure/file_storage/vfmem.h"

using namespace binom;

void VFMemory::init() {
  if(file.isEmpty ()) {
    file.resize(sizeof(DBHeader) + node_segement_size + primitive_segment_size + header.data_segment_size);
    file.write(0, header);
  } else {

  }
}

VFMemory::NodeSegmentList::SegmentNode& VFMemory::createNodeSegment() {
  ui64 descriptor_index = file.addSize(node_segement_size);
  file.write(descriptor_index, NodeSegmentDescriptor{0, {0}});
  file.write(node_segment_list.last().block.index, descriptor_index);
  return *node_segment_list.insertSegement({descriptor_index, node_segement_size}, {0, {0}});
}

void VFMemory::loadNodeSegments() {
  NodeSegmentDescriptor descriptor;
  file.read(sizeof(DBHeader), descriptor);
  node_segment_list.first() = {nullptr, descriptor, MemoryBlock{sizeof(DBHeader), node_segement_size}};
  while(descriptor.next_segment) {
    ui64 segment_index = descriptor.next_segment;
    file.read(segment_index, descriptor);
    node_segment_list.insertSegement(MemoryBlock{segment_index, node_segement_size}, descriptor);
  }
}

VFMemory::PrimitiveSegmentList::SegmentNode& VFMemory::createPrimitiveSegment() {
  ui64 descriptor_index = file.addSize(primitive_segment_size);
  file.write(descriptor_index, PrimitiveSegmentDescriptor{0, {0}});
  file.write(primitive_segment_list.last().block.index, descriptor_index);
  return *primitive_segment_list.insertSegement({descriptor_index, primitive_segment_size}, {0, {0}});
}

void VFMemory::loadPrimitiveSegments() {
  PrimitiveSegmentDescriptor descriptor;
  file.read(sizeof(DBHeader) + node_segement_size, descriptor);
  primitive_segment_list.first() = {nullptr, descriptor, MemoryBlock{sizeof(DBHeader) + node_segement_size, primitive_segment_size}};
  while(descriptor.next_segment) {
    ui64 segment_index = descriptor.next_segment;
    file.read(segment_index, descriptor);
    primitive_segment_list.insertSegement(MemoryBlock{segment_index, primitive_segment_size}, descriptor);
  }
}
