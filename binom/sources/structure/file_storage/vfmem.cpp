#include "binom/includes/structure/file_storage/vfmem.h"

using namespace binom;

void VFMemoryController::init() {
  if(file.isEmpty ()) {
    DataSegmentDescriptor data_descriptor{0, header.data_segment_size};

    file.resize(sizeof(DBHeader) + node_segement_size + primitive_segment_size + sizeof(DataSegmentDescriptor) + header.data_segment_size);
    file.write(0, header);
    file.write(sizeof(DBHeader) + node_segement_size + primitive_segment_size, data_descriptor);

    // Init segment lists
    node_segment_list.first() = {nullptr,
                                NodeSegmentDescriptor{0, {0}},
                                MemoryBlock{sizeof(DBHeader), node_segement_size}};

    primitive_segment_list.first() = {nullptr,
                                     PrimitiveSegmentDescriptor{0, {0}},
                                     MemoryBlock{sizeof(DBHeader) + node_segement_size, primitive_segment_size}};

    data_segment_list.first() = {nullptr,
                                data_descriptor,
                                MemoryBlock{sizeof(DBHeader) + node_segement_size + primitive_segment_size,
                                            sizeof (DataSegmentDescriptor) + data_descriptor.segment_size}};

  } else {
    loadNodeSegments();
    loadPrimitiveSegments();
    loadDataSegments();
  }
}

VFMemoryController::NodeSegmentList::SegmentNode& VFMemoryController::createNodeSegment() {
  ui64 descriptor_index = file.addSize(node_segement_size);
  file.write(descriptor_index, NodeSegmentDescriptor{0, {0}});
  file.write(node_segment_list.last().block.index, descriptor_index);
  return *node_segment_list.insertSegement({descriptor_index, node_segement_size}, {0, {0}});
}

void VFMemoryController::loadNodeSegments() {
  NodeSegmentDescriptor descriptor;
  file.read(sizeof(DBHeader), descriptor);
  node_segment_list.first() = {nullptr, descriptor, MemoryBlock{sizeof(DBHeader), node_segement_size}};
  while(descriptor.next_segment) {
    ui64 segment_index = descriptor.next_segment;
    file.read(segment_index, descriptor);
    node_segment_list.insertSegement(MemoryBlock{segment_index, node_segement_size}, descriptor);
  }
}

VFMemoryController::PrimitiveSegmentList::SegmentNode& VFMemoryController::createPrimitiveSegment() {
  ui64 descriptor_index = file.addSize(primitive_segment_size);
  file.write(descriptor_index, PrimitiveSegmentDescriptor{0, {0}});
  file.write(primitive_segment_list.last().block.index, descriptor_index);
  return *primitive_segment_list.insertSegement({descriptor_index, primitive_segment_size}, {0, {0}});
}

void VFMemoryController::loadPrimitiveSegments() {
  PrimitiveSegmentDescriptor descriptor;
  file.read(sizeof(DBHeader) + node_segement_size, descriptor);
  primitive_segment_list.first() = {nullptr, descriptor, MemoryBlock{sizeof(DBHeader) + node_segement_size, primitive_segment_size}};
  while(descriptor.next_segment) {
    ui64 segment_index = descriptor.next_segment;
    file.read(segment_index, descriptor);
    primitive_segment_list.insertSegement(MemoryBlock{segment_index, primitive_segment_size}, descriptor);
  }
}

VFMemoryController::DataSegmentList::SegmentNode& VFMemoryController::createDataSegment(ui64 size) {
  DataSegmentDescriptor descriptor{0, size};
  ui64 segment_size = sizeof(DataSegmentDescriptor) + descriptor.segment_size;
  ui64 descriptor_index = file.addSize(segment_size);
  file.write(descriptor_index, descriptor);
  file.write(data_segment_list.last().block.index, descriptor_index);
  return *data_segment_list.insertSegement({descriptor_index, segment_size}, descriptor);
}

void VFMemoryController::loadDataSegments() {
  DataSegmentDescriptor descriptor;
  file.read(sizeof(DBHeader) + node_segement_size + primitive_segment_size, descriptor);
  data_segment_list.first() = {nullptr, descriptor,
                              MemoryBlock{sizeof(DBHeader) + node_segement_size + primitive_segment_size,
                                          sizeof (DataSegmentDescriptor) + descriptor.segment_size}};
  while (descriptor.next_segment) {
    ui64 segment_index = descriptor.next_segment;
    file.read(segment_index, descriptor);
    data_segment_list.insertSegement(MemoryBlock{segment_index, sizeof (DataSegmentDescriptor) + descriptor.segment_size}, descriptor);
  }
}

NodeDescriptor VFMemoryController::getNodeDescriptor(ui64 index) {
  NodeDescriptor descriptor;
  file.read(node_segment_list[index/64].block.index +
            sizeof(NodeSegmentDescriptor) +
            (index%64)*sizeof(NodeDescriptor), descriptor);
  return descriptor;
}

ui64 VFMemoryController::setNodeDescriptor(ui64 index, NodeDescriptor descriptor) {
  NodeSegmentList::SegmentNode& node_segment = node_segment_list[index/64];
  ui64 node_index = index%64;
  if(!node_segment.descriptor.map.get(node_index)) {
    node_segment.descriptor.map.set(node_index, true);
    file.write(node_segment.block.index + offsetof(NodeSegmentDescriptor, map), node_segment.descriptor.map);
  }
  file.write(node_segment_list[index/64].block.index +
      sizeof(NodeSegmentDescriptor) +
      (node_index)*sizeof(NodeDescriptor), descriptor);
  return index;
}

ui64 VFMemoryController::setNodeDescriptor(NodeDescriptor descriptor) { return setNodeDescriptor(findFreeNodeDescriptor(), descriptor); }

void VFMemoryController::freeNodeDescriptor(ui64 index) {
  NodeSegmentList::SegmentNode& node_segment = node_segment_list[index/64];
  ui64 node_index = index%64;
  if(node_segment.descriptor.map.get(node_index)) {
    node_segment.descriptor.map.set(node_index, false);
    file.write(node_segment.block.index + offsetof(NodeSegmentDescriptor, map), node_segment.descriptor.map);
  }
}

ui64 VFMemoryController::findFreeNodeDescriptor() {
  ui64 node_index = 0;
  for(NodeSegmentList::SegmentNode node : node_segment_list) {
    for(BitPointer ptr = &node.descriptor.map; !ptr.isEnd(); ++ptr) {
      if(!*ptr) return node_index;
      ++node_index;
    }
  }
  createNodeSegment();
  return ++node_index;
}

ui64 VFMemoryController::getDataSegmentsSize() {
  ui64 size = 0;
  for(DataSegmentList::SegmentNode& node : data_segment_list) {
    size += node.block.size;
  }
  return size;
}

ui64 VFMemoryController::getNodeSegmentsSize() {
  ui64 size = 0;
  for(NodeSegmentList::SegmentNode& node : node_segment_list) {
    size += node.block.size;
  }
  return size;
}

ui64 VFMemoryController::getPrimitiveSegmentsSize() {
  ui64 size = 0;
  for(PrimitiveSegmentList::SegmentNode& node : primitive_segment_list) {
    size += node.block.size;
  }
  return size;
}

ui64 VFMemoryController::getDataSegmentsCount() {
  ui64 count = 0;
  for([[maybe_unused]] DataSegmentList::SegmentNode& node : data_segment_list) ++count;
  return count;
}

ui64 VFMemoryController::getNodeSegmentsCount() {
  ui64 count = 0;
  for([[maybe_unused]] NodeSegmentList::SegmentNode& node : node_segment_list) ++count;
  return count;
}

ui64 VFMemoryController::getPrimitiveSegmentsCount() {
  ui64 count = 0;
  for([[maybe_unused]] PrimitiveSegmentList::SegmentNode& node : primitive_segment_list) ++count;
  return count;
}

