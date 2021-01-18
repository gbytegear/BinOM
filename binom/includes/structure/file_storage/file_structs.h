#ifndef FILE_NODE_STRUCTS_H
#define FILE_NODE_STRUCTS_H
#include "../types.h"


namespace binom {

#pragma pack(push, 1)

/*
 * [DBHeader]
 * [NodeSegment] {[NodeSegmentDescriptor], NodeDescriptor 1, NodeDescriptor 2, ... , NodeDescriptor n}
 * [DataSegment] {[DataSegmentDescriptor],...}
 * [NodeSegment] {[NodeSegmentDescriptor], NodeDescriptor n+1, NodeDescriptor n+2, ... , NodeDescriptor n+n}
 * [DataSegment] {[DataSegmentDescriptor],...}
 * ...
 *
*/

union BitMap {
  ui64 val = 0;
  struct Bits {
    bool n0:1;
    bool n1:1;
    bool n2:1;
    bool n3:1;
    bool n4:1;
    bool n5:1;
    bool n6:1;
    bool n7:1;

    bool get(ui8 index) {
      switch (index) {
      case 0: return n0;
      case 1: return n1;
      case 2: return n2;
      case 3: return n3;
      case 4: return n4;
      case 5: return n5;
      case 6: return n6;
      case 7: return n7;
      }
      return false;
    }

    bool set(ui8 index, bool value) {
      switch (index) {
      case 0: return n0 = value;
      case 1: return n1 = value;
      case 2: return n2 = value;
      case 3: return n3 = value;
      case 4: return n4 = value;
      case 5: return n5 = value;
      case 6: return n6 = value;
      case 7: return n7 = value;
      }
      return false;
    }

  } bits[8];

  bool get(ui8 index) {return bits[index/8].get(index%8);}
  bool set(ui8 index, bool value) {return bits[index/8].set(index%8, value);}

};

struct VersionNumber {
  enum Type : ui8 { alpha, beta, RC, RC2, final };
  const ui16 major = 0;
  const ui32 minor = 1;
  Type type;
};

constexpr VersionNumber current_version {0, 1, VersionNumber::alpha};


// Structure headersW
struct DBHeader {
  struct Version {
    const char program_name[6] = "BinOM";
    VersionNumber number = current_version;
  };
  Version version;
  ui32 data_segment_size = 4096;
};

//! Node Segmnet size = 64*sizeof(NodeDescriptor) + sizeof(NodeSegmentDescriptor)
struct NodeSegmentDescriptor {
  ui64 next_segment = 0;
  BitMap map;
};

struct NodeDescriptor {
  VarType type = VarType::end;
  ui32 element_count = 0;
  ui64 data_index = 0;
};


//! Primitive Segment size = 64 + sizeof(PrimitiveSegementDescriptor)
struct PrimitiveSegmentDescriptor {
  ui64 next_segment = 0;
  BitMap map;
};

struct DataSegmentDescriptor {
  ui64 next_segment = 0;
  ui64 segment_size = 4096;
};



// Node data

struct ArrayDescriptor {
  ui64 element_count;
};

struct ObjectDescriptor {
  ui64 name_length_block_size;
  ui64 name_block_size;
  ui64 element_count;
};

struct AOElementDescriptor {
  ui64 node_index;
};

struct BufferDescriptor {
  ui64 element_count;
};

#pragma pack(pop)

}


#endif
