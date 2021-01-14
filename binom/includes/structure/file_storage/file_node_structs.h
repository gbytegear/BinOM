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
  ui64 val = 0;

  bool get(ui8 index) {return bits[index/8].get(index%8);}

};


// Structure headers

struct DBHeader {

};

struct NodeSegmentDescriptor {
  ui64 next_segment = 0;
  BitMap map;
};

struct NodeDescriptor {
  VarType type = VarType::end;
  ui32 element_count = 0;
  ui64 data_index = 0;
};

struct DataSegmentDescriptor {
  ui64 next_segment = 0;
};



// Node data

struct ArrayElement {
  ui64 node_index;
};

struct ObjectElement { // Think about it!!!
  ui64 node_index;
  ui64 name_index;
};

#pragma pack(pop)

}


#endif
