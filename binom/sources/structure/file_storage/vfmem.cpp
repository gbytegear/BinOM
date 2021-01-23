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
    loadDataMemory ();
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
  data_memory_map.addMemory(data_segment_list.first().block.size);
  while (descriptor.next_segment) {
    ui64 segment_index = descriptor.next_segment;
    file.read(segment_index, descriptor);
    data_segment_list.insertSegement(MemoryBlock{segment_index, sizeof (DataSegmentDescriptor) + descriptor.segment_size}, descriptor);
    data_memory_map.addMemory(data_segment_list.last().block.size);
  }
}

void VFMemoryController::loadDataMemory() {
  for(NodeSegmentList::SegmentNode& segment : node_segment_list) {
    NodeDescriptor descriptor;
    {ui8 i = 0; for(BitPointer ptr(&segment.descriptor.map); !ptr.isEnd(); (++ptr,++i))
      if(*ptr) {
        file.read(segment.block.index + sizeof(NodeSegmentDescriptor) + i*sizeof(NodeDescriptor), descriptor);
        if(VarTypeClass type_class = toTypeClass(descriptor.type); type_class == VarTypeClass::primitive || type_class == VarTypeClass::invalid_type)
          continue;
        data_memory_map.allocDataBlock (descriptor.data_index, descriptor.block_size);
      }
    }
  }
}










IndexedNodeDescriptor VFMemoryController::getNodeDescriptor(ui64 index) {
  NodeDescriptor descriptor;
  file.read(node_segment_list[index/64].block.index +
            sizeof(NodeSegmentDescriptor) +
            (index%64)*sizeof(NodeDescriptor), descriptor);
  return {descriptor, index};
}

ui64 VFMemoryController::setNodeDescriptor(ui64 index, NodeDescriptor descriptor) {
  NodeSegmentList::SegmentNode& node_segment = node_segment_list[index/64];
  ui64 node_index = index%64;
  if(!node_segment.descriptor.map.get(node_index)) {
    node_segment.descriptor.map.set(node_index, true);
    file.write(node_segment.block.index + offsetof(NodeSegmentDescriptor, map), node_segment.descriptor.map);
  }
  file.write(node_segment.block.index +
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

ui64 VFMemoryController::getNodeCount() {
  ui64 count = 0;
  for(NodeSegmentList::SegmentNode& node : node_segment_list)
    for(BitPointer ptr(&node.descriptor.map) ; !ptr.isEnd(); ++ptr)
      count += *ptr;
  return count;
}

ui64 VFMemoryController::getPrimitiveSegmentsCount() {
  ui64 count = 0;
  for([[maybe_unused]] PrimitiveSegmentList::SegmentNode& node : primitive_segment_list) ++count;
  return count;
}










template ui8 VFMemoryController::getPrimitive<ui8>(ui64);
template ui16 VFMemoryController::getPrimitive<ui16>(ui64);
template ui32 VFMemoryController::getPrimitive<ui32>(ui64);
template ui64 VFMemoryController::getPrimitive<ui64>(ui64);
template i8 VFMemoryController::getPrimitive<i8>(ui64);
template i16 VFMemoryController::getPrimitive<i16>(ui64);
template i32 VFMemoryController::getPrimitive<i32>(ui64);
template i64 VFMemoryController::getPrimitive<i64>(ui64);
template f32 VFMemoryController::getPrimitive<f32>(ui64);
template f64 VFMemoryController::getPrimitive<f64>(ui64);
template<typename Type>
Type VFMemoryController::getPrimitive(ui64 index) {
  Type value;
  file.read(primitive_segment_list[index/64].block.index +
            sizeof(PrimitiveSegmentDescriptor) + index%64, value);
  return value;
}

template ui64 VFMemoryController::setPrimitive<ui8>(ui64,ui8);
template ui64 VFMemoryController::setPrimitive<ui16>(ui64,ui16);
template ui64 VFMemoryController::setPrimitive<ui32>(ui64,ui32);
template ui64 VFMemoryController::setPrimitive<ui64>(ui64,ui64);
template ui64 VFMemoryController::setPrimitive<i8>(ui64,i8);
template ui64 VFMemoryController::setPrimitive<i16>(ui64,i16);
template ui64 VFMemoryController::setPrimitive<i32>(ui64,i32);
template ui64 VFMemoryController::setPrimitive<i64>(ui64,i64);
template ui64 VFMemoryController::setPrimitive<f32>(ui64,f32);
template ui64 VFMemoryController::setPrimitive<f64>(ui64,f64);
template<typename Type>
ui64 VFMemoryController::setPrimitive(ui64 index, Type value) {
  PrimitiveSegmentList::SegmentNode& primitive_segment = primitive_segment_list[index/64];
  ui64 byte_index = index%64;

  for(ui8 i = byte_index; i < byte_index + sizeof(Type); ++i)
    primitive_segment.descriptor.map.set(i, true);
  file.write(primitive_segment.block.index + offsetof(PrimitiveSegmentDescriptor, map), primitive_segment.descriptor.map);

  file.write(primitive_segment.block.index +
      sizeof(PrimitiveSegmentDescriptor) + index%64, value);
  return index;
}

template ui64 VFMemoryController::setPrimitive<ui8>(ui8);
template ui64 VFMemoryController::setPrimitive<ui16>(ui16);
template ui64 VFMemoryController::setPrimitive<ui32>(ui32);
template ui64 VFMemoryController::setPrimitive<ui64>(ui64);
template ui64 VFMemoryController::setPrimitive<i8>(i8);
template ui64 VFMemoryController::setPrimitive<i16>(i16);
template ui64 VFMemoryController::setPrimitive<i32>(i32);
template ui64 VFMemoryController::setPrimitive<i64>(i64);
template ui64 VFMemoryController::setPrimitive<f32>(f32);
template ui64 VFMemoryController::setPrimitive<f64>(f64);
template<typename Type>
ui64 VFMemoryController::setPrimitive(Type value) {return setPrimitive<Type>(findFreePrimitive<Type>(), value);}

template void VFMemoryController::freePrimitive<ui8>(ui64);
template void VFMemoryController::freePrimitive<ui16>(ui64);
template void VFMemoryController::freePrimitive<ui32>(ui64);
template void VFMemoryController::freePrimitive<ui64>(ui64);
template void VFMemoryController::freePrimitive<i8>(ui64);
template void VFMemoryController::freePrimitive<i16>(ui64);
template void VFMemoryController::freePrimitive<i32>(ui64);
template void VFMemoryController::freePrimitive<i64>(ui64);
template void VFMemoryController::freePrimitive<f32>(ui64);
template void VFMemoryController::freePrimitive<f64>(ui64);
template<typename Type>
void VFMemoryController::freePrimitive(ui64 index) {
  PrimitiveSegmentList::SegmentNode& primitive_segment = primitive_segment_list[index/64];
  ui64 byte_index = index%64;
  for(ui8 i = byte_index; i < byte_index + sizeof(Type); ++i)
    primitive_segment.descriptor.map.set(i, false);
  file.write(primitive_segment.block.index + offsetof(PrimitiveSegmentDescriptor, map), primitive_segment.descriptor.map);
}

template ui64 VFMemoryController::findFreePrimitive<ui8>();
template ui64 VFMemoryController::findFreePrimitive<ui16>();
template ui64 VFMemoryController::findFreePrimitive<ui32>();
template ui64 VFMemoryController::findFreePrimitive<ui64>();
template ui64 VFMemoryController::findFreePrimitive<i8>();
template ui64 VFMemoryController::findFreePrimitive<i16>();
template ui64 VFMemoryController::findFreePrimitive<i32>();
template ui64 VFMemoryController::findFreePrimitive<i64>();
template<typename Type>
ui64 VFMemoryController::findFreePrimitive() {
  ui64 byte_index = 0;
  for(PrimitiveSegmentList::SegmentNode segment : primitive_segment_list)
    for(BitPointer ptr = &segment.descriptor.map; !ptr.isEnd(); ++ptr) {
      ui8 i = sizeof(Type);
      for(;!*ptr && i > 0 && !ptr.isEnd();(--i,++ptr)); // Test bits
      if(i == 0) return byte_index;
      else byte_index += sizeof (Type) - i;
    }
  createPrimitiveSegment();
  return ++byte_index;
}







ByteArray VFMemoryController::getData(ui64 index, ui64 size) {
  ByteArray data(size);
  DataSegmentList::SegmentIterator s_it = &data_segment_list[index/header.data_segment_size];
  ui64 data_index = index%header.data_segment_size;
  if(ui64 size_in_segment = s_it->block.size - sizeof(DataSegmentDescriptor); s_it->block.size - data_index >= size) // Read from continuous block
    file.read (s_it->block.index + sizeof(DataSegmentDescriptor) + data_index, data);

  else { // Read from segmented block
    ByteArray::iterator b_it = file.read (s_it->block.index +  + sizeof(DataSegmentDescriptor) + data_index, data, size_in_segment);
    size -= size_in_segment;
    while (size) {
      ++s_it;
      if(s_it == nullptr) throw SException(ErrCode::binomdb_out_of_segment);
      if(size > s_it->block.size) {
        b_it = file.read (s_it->block.index + sizeof(DataSegmentDescriptor), data, s_it->block.size - sizeof(DataSegmentDescriptor));
        size -= s_it->block.size - sizeof(DataSegmentDescriptor);
      } else {
         file.read(s_it->block.index  + sizeof(DataSegmentDescriptor), data, size);
         break;
      }
    }
  }

  return data;
}

ByteArray VFMemoryController::getData(ui64 index) {
  MemoryBlock mblock = data_memory_map.findDataBlock(index);
  if(mblock.index == 0 && mblock.size == 0) throw SException(ErrCode::binomdb_block_isnt_exist);
  return getData(mblock.index, mblock.size);
}

ByteArray VFMemoryController::getData(NodeDescriptor descriptor) {return getData(descriptor.data_index, descriptor.block_size);}

ui64 VFMemoryController::setData(ui64 index, ByteArray data) {
  DataSegmentList::SegmentIterator s_it = &data_segment_list[index/header.data_segment_size];
  ui64 size = data.length();
  ui64 data_index = index%header.data_segment_size;
  if(ui64 size_in_segment = s_it->block.size - sizeof(DataSegmentDescriptor); s_it->block.size - data_index >= size) // Read from continuous block
    file.write (s_it->block.index + sizeof(DataSegmentDescriptor) + data_index, data);

  else { // Read from segmented block
    ByteArray::iterator b_it = file.read (s_it->block.index +  + sizeof(DataSegmentDescriptor) + data_index, data, size_in_segment);
    size -= size_in_segment;
    while (size) {
      ++s_it;
      if(s_it == nullptr) throw SException(ErrCode::binomdb_out_of_segment);
      if(size > s_it->block.size) {
        b_it = file.write (s_it->block.index + sizeof(DataSegmentDescriptor), data, s_it->block.size - sizeof(DataSegmentDescriptor));
        size -= s_it->block.size - sizeof(DataSegmentDescriptor);
      } else {
        file.write(s_it->block.index  + sizeof(DataSegmentDescriptor), data, size);
        break;
      }
    }
  }

  return index;
}

ui64 VFMemoryController::setData(ByteArray data) {
  MemoryBlock mblock = data_memory_map.allocDataBlock (data.length());
  if(mblock.index == 0 && mblock.size == 0) throw SException(ErrCode::binomdb_block_isnt_exist);
  return setData(mblock.index, data);
}

void VFMemoryController::freeData(ui64 index) {data_memory_map.freeBlock(index);}








// DataMemoryBlocks

void VFMemoryController::DataMemoryBlocks::split(VFMemoryController::DataMemoryBlocks::DataBlock& data_block, ui64 size) {
  if(data_block.block.size == size) {
    return;
  }

  DataBlock* new_data_block = new DataBlock{
      false,
      {data_block.block.index + size,
       data_block.block.size - size},
      data_block.next, &data_block};

  data_block.block.size = size;
  if(data_block.next) data_block.next->prev = new_data_block;
  else last_block = new_data_block;
  data_block.next = new_data_block;
}

MemoryBlock VFMemoryController::DataMemoryBlocks::alloc(VFMemoryController::DataMemoryBlocks::DataBlock& data_block, ui64 size) {
  split (data_block, size);
  data_block.is_used = true;
  return data_block.block;
}

void VFMemoryController::DataMemoryBlocks::free(VFMemoryController::DataMemoryBlocks::DataBlock& data_block) {
  while(!data_block.prev->is_used) {
    data_block.prev->is_used = true;
    DataBlock* prev_block = data_block.prev;
    data_block.prev = data_block.prev->prev;
    if(data_block.prev)data_block.prev->next = &data_block;
    data_block.block = {prev_block->block.index,
                        data_block.block.size + prev_block->block.size};
    delete prev_block;
    if(!data_block.prev) break;
  }

  while(!data_block.next->is_used) {
    data_block.next->is_used = true;
    DataBlock* next_block = data_block.next;
    data_block.next = data_block.next->next;
    if(data_block.next) data_block.next->prev = &data_block;
    data_block.block.size += next_block->block.size;
    delete next_block;
    if(!data_block.next) {
      last_block = &data_block;
      break;
    }
  }

  data_block.is_used = false;
}

void VFMemoryController::DataMemoryBlocks::addMemory(ui64 size) {
  if(!last_block->is_used) {
    last_block->block.size += size;
  } else {
    last_block = last_block->next = new DataBlock{false,
    {last_block->block.index + last_block->block.size, size},
        nullptr,
        last_block};
  }
}

MemoryBlock VFMemoryController::DataMemoryBlocks::allocDataBlock(ui64 size) {
  for(DataBlock& data : *this) {
    if(data.is_used || data.block.size < size) continue;
    return alloc(data, size);
  }
  return {0,0};
}

MemoryBlock VFMemoryController::DataMemoryBlocks::allocDataBlock(ui64 index, ui64 size) {
  for(DataBlock& data : *this) {
    if(data.block.index < index && data.block.index + data.block.size > index) {
      split (data, index - data.block.index);
      return alloc (*data.next, size);
    } else if(data.block.index == index) return alloc (data, size);
  }
  return {0,0};
}

MemoryBlock VFMemoryController::DataMemoryBlocks::findDataBlock(ui64 index) {
  for(DataBlock& data: *this)
    if(data.block.index == index) return data.block;
  return {0,0};
}

void VFMemoryController::DataMemoryBlocks::freeBlock(ui64 index) {
  for(DataBlock& data : *this)
    if(data.block.index == index) return free(data);
}

VFMemoryController::DataMemoryBlocks::iterator VFMemoryController::DataMemoryBlocks::begin() {return &start_block;}
VFMemoryController::DataMemoryBlocks::iterator VFMemoryController::DataMemoryBlocks::end() {return nullptr;}
