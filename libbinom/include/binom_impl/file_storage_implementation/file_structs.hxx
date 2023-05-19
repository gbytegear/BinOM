#ifndef FILE_STRUCTS_HXX
#define FILE_STRUCTS_HXX

#include "../types.hxx"

namespace binom::file_structs {

using namespace type_alias;

// File structs
#pragma pack(push, 1)

constexpr ui64 nullindex = 0xFFFFFFFFFFFFFFFFull;

enum class NodeIndex : ui64;

struct NodeDescriptor {
  VarType type = VarType::null;
  ui64 link_counter = 1;
  union Data {
    bool  bool_val;
    ui8   ui8_val;
    ui16  ui16_val;
    ui32  ui32_val;
    ui64  ui64_val;
    i8    i8_val;
    i16   i16_val;
    i32   i32_val;
    i64   i64_val;
    f32   f32_val;
    f64   f64_val;

    struct ComplexTypeDescriptor {
      ui64 count = 0;
      ui64 index = nullindex;
    } complex_type_descriptor = {};

  } data;
};

struct MemoryBlockDescriptor {
  ui64 size = 0;
  ui64 index = nullindex;
  ui64 next = nullindex;
  ui64 prev = nullindex;
};

struct Page {
  ui64 next = nullindex;
};



struct DataBaseHeader {
  const char file_signature[] = "BinOMDB";
  NodeDescriptor root;
  ui64 node_page = nullindex;
  ui64 memory_block_page = nullindex;
  ui64 byte_page = nullindex;
  ui64 word_page = nullindex;
  ui64 dword_page = nullindex;
  ui64 qword_page = nullindex;

};



#pragma pack(pop)

}

#endif
