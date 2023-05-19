#ifndef FILE_STORAGE_IMPLEMENTATION_HXX
#define FILE_STORAGE_IMPLEMENTATION_HXX

#include "types.hxx"

namespace binom::fs::priv {

using namespace type_alias;
// File structs
#pragma pack(push, 1)

template<typename T>
struct Page {
  ui64 page_number;
  ui64 next_page_pointer;

}

struct DataBaseHeader {
  char signature[] = "BinOMDB";

};


}

// TODO

#endif // FILE_STORAGE_IMPLEMENTATION_HXX
