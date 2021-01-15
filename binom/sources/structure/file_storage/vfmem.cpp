#include "binom/includes/structure/file_storage/vfmem.h"

using namespace binom;

void VFMemory::init() {
  if(file.isEmpty ()) {

    // [DBHeader] [NodeSegmentDescriptor] [node_segment_size byte] [DBHeader::data_segment_size byte]
    //                                                             [PrimitiveSegementDescriptor]

    ui64 file_size = sizeof (DBHeader) + sizeof (NodeSegmentDescriptor) + node_segment_size + header.data_segment_size;

    file.resize(file_size);
    file.write (0, header);
    file.write (sizeof(DBHeader), node_segments.descriptor);
    file.write (sizeof(DBHeader) + sizeof(NodeSegmentDescriptor) + node_segment_size, primitive_segments.descriptor);

    free_mem_chain.mem_block.pos = sizeof(DBHeader) +
                                   sizeof(NodeSegmentDescriptor) +
                                   node_segment_size +
                                   sizeof (PrimitiveSegementDescriptor) +
                                   primitive_segment_size;

    free_mem_chain.mem_block.size = file_size - free_mem_chain.mem_block.pos;

  } else {

    file.read(0, header);

    if(strcmp(header.version.program_name, "BinOM"))
      throw SException(ErrCode::binomdb_invalid_file);
    elif(header.version.number.major != current_version.major ||
         header.version.number.minor != current_version.minor)
      throw SException(ErrCode::binomdb_invalid_storage_version);

    file.read(sizeof(DBHeader), node_segments.descriptor);
    file.read(sizeof(DBHeader) + sizeof(NodeSegmentDescriptor) + node_segment_size, primitive_segments.descriptor);

    if(node_segments.descriptor.next_segment)
      while (last_node_segment->descriptor.next_segment) {
        NodeSegmentList* new_segment_descriptor = new NodeSegmentList;
        new_segment_descriptor->header_pos = last_node_segment->descriptor.next_segment;
        file.read (new_segment_descriptor->header_pos, new_segment_descriptor->descriptor);
        new_segment_descriptor->next = nullptr;
        last_node_segment = last_node_segment->next = new_segment_descriptor;
      }

    if(primitive_segments.descriptor.next_segment)
      while(last_primitive_segment->descriptor.next_segment) {
        PrimitiveSegmentList* new_segment_descriptor = new PrimitiveSegmentList;
        new_segment_descriptor->header_pos = last_primitive_segment->descriptor.next_segment;
        file.read (new_segment_descriptor->header_pos, new_segment_descriptor->descriptor);
        new_segment_descriptor->next = nullptr;
        last_primitive_segment = last_primitive_segment->next = new_segment_descriptor;
      }

  }
}

void VFMemory::findFree() {
  ui64 file_size = file.size();
  NodeBuffer buffer;
  file.read(sizeof(DBHeader), buffer);
}
