#ifndef TYPES_HPP
#define TYPES_HPP

#include "memctrl.hxx"
#include <initializer_list>

/// Binary Object Model
namespace binom {

using namespace type_alias;
using namespace memctrl;

enum class LinkType : ui8 {
  soft_link = 0x00,
  hard_link = 0x01,

  invalid_link = 0xFF
};

enum class VarType : ui8 {
  null                    = 0x01,
  boolean                 = 0x02,
  ui8                     = 0x03,
  si8                     = 0x04,
  ui16                    = 0x05,
  si16                    = 0x06,
  ui32                    = 0x07,
  si32                    = 0x08,
  f32                     = 0x09,
  ui64                    = 0x0A,
  si64                    = 0x0B,
  f64                     = 0x0C,
  ui8_array               = 0x0D,
  si8_array               = 0x0E,
  ui16_array              = 0x0F,
  si16_array              = 0x10,
  ui32_array              = 0x11,
  si32_array              = 0x12,
  f32_array               = 0x13,
  ui64_array              = 0x14,
  si64_array              = 0x15,
  f64_array               = 0x16,
  array                   = 0x17,
  less_map                = 0x18,
  greater_map             = 0x19,

  separator               = 0x00,
  invalid_type            = 0xFF
};

enum class VarTypeClass : ui8 {
  null                    = 0x01,
  number                  = 0x02,
  buffer_array            = 0x03,
  array                   = 0x04,
  map                     = 0x05,

  invalid_type            = int(VarType::invalid_type)
};

enum class VarBitWidth : ui8 {
  byte                    = 0x01,
  word                    = 0x02,
  dword                   = 0x04,
  qword                   = 0x08,

  invalid_type            = int(VarType::invalid_type)
};

enum class VarNumberType : ui8 {
  unsigned_integer        = 0x00,
  signed_integer          = 0x01,
  float_point             = 0x02,

  invalid_type            = int(VarType::invalid_type)
};

enum class VarSortType : ui8 {
  unsorted                = 0x00,
  less                    = 0x01,
  greater                 = 0x02,

  invalid_type            = int(VarType::invalid_type)
};

enum class ValType : ui8 {
  boolean                 = int(VarType::boolean),
  ui8                     = int(VarType::ui8),
  si8                     = int(VarType::si8),
  ui16                    = int(VarType::ui16),
  si16                    = int(VarType::si16),
  ui32                    = int(VarType::ui32),
  si32                    = int(VarType::si32),
  f32                     = int(VarType::f32),
  ui64                    = int(VarType::ui64),
  si64                    = int(VarType::si64),
  f64                     = int(VarType::f64),

  invalid_type            = int(VarType::invalid_type)
};


inline VarTypeClass getTypeClass(VarType type) noexcept {
  switch (type) {
    case VarType::null:
    return VarTypeClass::null;

    case VarType::ui8:
    case VarType::si8:
    case VarType::ui16:
    case VarType::si16:
    case VarType::ui32:
    case VarType::si32:
    case VarType::f32:
    case VarType::ui64:
    case VarType::si64:
    case VarType::f64:
    return VarTypeClass::number;

    case VarType::ui8_array:
    case VarType::si8_array:
    case VarType::ui16_array:
    case VarType::si16_array:
    case VarType::ui32_array:
    case VarType::si32_array:
    case VarType::f32_array:
    case VarType::ui64_array:
    case VarType::si64_array:
    case VarType::f64_array:
    return VarTypeClass::buffer_array;

    case VarType::array:
    return VarTypeClass::array;

    case VarType::less_map:
    case VarType::greater_map:
    return VarTypeClass::map;

    case VarType::separator:
    case VarType::invalid_type:
    default:
    return VarTypeClass::invalid_type;

  }
}


inline VarBitWidth getBitWidth(VarType type) noexcept {
  switch (type) {
    case VarType::ui8:
    case VarType::si8:
    case VarType::ui8_array:
    case VarType::si8_array:
    return VarBitWidth::byte;

    case VarType::ui16:
    case VarType::si16:
    case VarType::ui16_array:
    case VarType::si16_array:
    return VarBitWidth::word;

    case VarType::ui32:
    case VarType::si32:
    case VarType::f32:
    case VarType::ui32_array:
    case VarType::si32_array:
    case VarType::f32_array:
    return VarBitWidth::dword;

    case VarType::ui64:
    case VarType::si64:
    case VarType::f64:
    case VarType::ui64_array:
    case VarType::si64_array:
    case VarType::f64_array:
    return VarBitWidth::qword;

    default: return VarBitWidth::invalid_type;
  }
}

inline VarBitWidth getBitWidth(ValType type) noexcept {
  switch (type) {
    case ValType::ui8:
    case ValType::si8:
    return VarBitWidth::byte;

    case ValType::ui16:
    case ValType::si16:
    return VarBitWidth::word;

    case ValType::ui32:
    case ValType::si32:
    case ValType::f32:
    return VarBitWidth::dword;

    case ValType::ui64:
    case ValType::si64:
    case ValType::f64:
    return VarBitWidth::qword;

    default: return VarBitWidth::invalid_type;
  }
}


inline VarNumberType getNumberType(VarType type) noexcept {
  switch (type) {
    case VarType::ui8_array:
    case VarType::ui8:
    case VarType::ui16_array:
    case VarType::ui16:
    case VarType::ui32_array:
    case VarType::ui32:
    case VarType::ui64_array:
    case VarType::ui64:
    return VarNumberType::unsigned_integer;

    case VarType::si8_array:
    case VarType::si8:
    case VarType::si16_array:
    case VarType::si16:
    case VarType::si32_array:
    case VarType::si32:
    case VarType::si64_array:
    case VarType::si64:
    return VarNumberType::signed_integer;

    case VarType::f32_array:
    case VarType::f32:
    case VarType::f64_array:
    case VarType::f64:
    return VarNumberType::float_point;

    default: return VarNumberType::invalid_type;
  }
}

inline VarNumberType getNumberType(ValType type) noexcept {
  switch (type) {
    case ValType::boolean:
    case ValType::ui8:
    case ValType::ui16:
    case ValType::ui32:
    case ValType::ui64:
    return VarNumberType::unsigned_integer;

    case ValType::si8:
    case ValType::si16:
    case ValType::si32:
    case ValType::si64:
    return VarNumberType::signed_integer;

    case ValType::f32:
    case ValType::f64:
    return VarNumberType::float_point;

    default: return VarNumberType::invalid_type;
  }
}


inline VarSortType getSortType(VarType type) noexcept {
  switch (type) {
    case VarType::ui8_array:
    case VarType::si8_array:
    case VarType::ui16_array:
    case VarType::si16_array:
    case VarType::ui32_array:
    case VarType::si32_array:
    case VarType::f32_array:
    case VarType::ui64_array:
    case VarType::si64_array:
    case VarType::f64_array:
    case VarType::array:
    return VarSortType::unsorted;

    case VarType::less_map:
    return VarSortType::less;

    case VarType::greater_map:
    return VarSortType::greater;

    default:
    return VarSortType::invalid_type;
  }
}

inline ValType getValueType(VarType type) noexcept {
  switch (type) {
    case binom::VarType::boolean:
    return ValType::boolean;

    case binom::VarType::ui8:
    case binom::VarType::ui8_array:
    return ValType::ui8;

    case binom::VarType::si8:
    case binom::VarType::si8_array:
    return ValType::si8;

    case binom::VarType::ui16:
    case binom::VarType::ui16_array:
    return ValType::ui16;

    case binom::VarType::si16:
    case binom::VarType::si16_array:
    return ValType::si16;

    case binom::VarType::ui32:
    case binom::VarType::ui32_array:
    return ValType::ui32;

    case binom::VarType::si32:
    case binom::VarType::si32_array:
    return ValType::si32;

    case binom::VarType::f32:
    case binom::VarType::f32_array:
    return ValType::f32;

    case binom::VarType::ui64:
    case binom::VarType::ui64_array:
    return ValType::ui64;

    case binom::VarType::si64:
    case binom::VarType::si64_array:
    return ValType::si64;

    case binom::VarType::f64:
    case binom::VarType::f64_array:
    return ValType::f64;

    default:
    return ValType::invalid_type;
  }
}


inline bool operator==(VarTypeClass type_class, VarType type) noexcept {return type_class == getTypeClass(type);}
inline bool operator==(VarType type, VarTypeClass type_class) noexcept {return getTypeClass(type) == type_class;}
inline bool operator!=(VarTypeClass type_class, VarType type) noexcept {return type_class != getTypeClass(type);}
inline bool operator!=(VarType type, VarTypeClass type_class) noexcept {return getTypeClass(type) != type_class;}

inline bool operator==(VarBitWidth bit_width, VarType type) noexcept {return bit_width == getBitWidth(type);}
inline bool operator==(VarType type, VarBitWidth bit_width) noexcept {return getBitWidth(type) == bit_width;}
inline bool operator!=(VarBitWidth bit_width, VarType type) noexcept {return bit_width != getBitWidth(type);}
inline bool operator!=(VarType type, VarBitWidth bit_width) noexcept {return getBitWidth(type) != bit_width;}

inline bool operator==(VarNumberType number_type, VarType type) noexcept {return number_type == getNumberType(type);}
inline bool operator==(VarType type, VarNumberType number_type) noexcept {return getNumberType(type) == number_type;}
inline bool operator!=(VarNumberType number_type, VarType type) noexcept {return number_type != getNumberType(type);}
inline bool operator!=(VarType type, VarNumberType number_type) noexcept {return getNumberType(type) != number_type;}

inline bool operator==(VarSortType sort_type, VarType type) noexcept {return sort_type == getSortType(type);}
inline bool operator==(VarType type, VarSortType sort_type) noexcept {return getSortType(type) == sort_type;}
inline bool operator!=(VarSortType sort_type, VarType type) noexcept {return sort_type != getSortType(type);}
inline bool operator!=(VarType type, VarSortType sort_type) noexcept {return getSortType(type) != sort_type;}


class Variable;

class Number;
class BufferArray;
class Array;
class Map;
class MultiMap;

struct MapLiteral;
struct MultiMapLiteral;

namespace literals {

typedef std::initializer_list<const ui8>        ui8arr;
typedef std::initializer_list<const i8>         i8arr;
typedef std::initializer_list<const ui16>       ui16arr;
typedef std::initializer_list<const i16>        i16arr;
typedef std::initializer_list<const ui32>       ui32arr;
typedef std::initializer_list<const i32>        i32arr;
typedef std::initializer_list<const ui64>       ui64arr;
typedef std::initializer_list<const i64>        i64arr;
typedef std::initializer_list<const Variable>   varr;
typedef MapLiteral                              map;

}

}

#endif // TYPES_HPP
