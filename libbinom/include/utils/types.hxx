#ifndef TYPES_HXX
#define TYPES_HXX

#include "memctrl.hxx"
#include <initializer_list>
#include "heritable_initializer_list.hxx"

/// Binary Object Model
namespace binom {

using namespace type_alias;
using namespace memctrl;

/// BinOM Variable type codes
enum class VarType : ui8 {
  null                    = 0x01, ///< NULL
  boolean                 = 0x02, ///< Boolean value
  ui8                     = 0x03, ///< Unsigned 8-bit integer number
  si8                     = 0x04, ///< Signed 8-bit integer number
  ui16                    = 0x05, ///< Unsigned 16-bit integer number
  si16                    = 0x06, ///< Signed 16-bit integer number
  ui32                    = 0x07, ///< Unsigned 32-bit integer number
  si32                    = 0x08, ///< Signed 32-bit integer number
  f32                     = 0x09, ///< 32-bit number with floating point
  ui64                    = 0x0A, ///< Unsigned 64-bit integer number
  si64                    = 0x0B, ///< Signed 64-bit integer number
  f64                     = 0x0C, ///< 64-bit number with floating point
  bit_array               = 0x0D, ///< Array of boolean values
  ui8_array               = 0x0E, ///< Array of unsigned 8-bit integer numbers
  si8_array               = 0x0F, ///< Array of signed 8-bit integer numbers
  ui16_array              = 0x10, ///< Array of unsigned 16-bit integer numbers
  si16_array              = 0x11, ///< Array of signed 16-bit integer numbers
  ui32_array              = 0x12, ///< Array of unsigned 32-bit integer numbers
  si32_array              = 0x13, ///< Array of signed 32-bit integer numbers
  f32_array               = 0x14, ///< Array of 32-bit numbers with floating point
  ui64_array              = 0x15, ///< Array of unsigned 64-bit integer numbers
  si64_array              = 0x16, ///< Array of signed 64-bit integer numbers
  f64_array               = 0x17, ///< Array of 64-bit numbers with floating point
  array                   = 0x18, ///< Heterogeneous array
  list                    = 0x19, ///< Heterogeneous list
  less_map                = 0x1A, ///< Associative heterogeneous container with key-sorted by descending
  greater_map             = 0x1B, ///< Associative heterogeneous container with key-sorted by ascending

  separator               = 0x00, ///< End code
  invalid_type            = 0xFF  ///< Invalid type code
};

enum class VarTypeClass : ui8 {
  null                    = 0x01,
  number                  = 0x02,
  bit_array               = 0x03,
  buffer_array            = 0x04,
  array                   = 0x05,
  list                    = 0x06,
  map                     = 0x07,

  invalid_type            = int(VarType::invalid_type)
};

enum class VarBitWidth : ui8 {
  byte                    = sizeof (byte),
  word                    = sizeof (word),
  dword                   = sizeof (dword),
  qword                   = sizeof (qword),

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


inline VarTypeClass toTypeClass(VarType type) noexcept {
  switch (type) {
  case VarType::null:
  return VarTypeClass::null;

  case VarType::boolean:
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

  case VarType::bit_array:
  return VarTypeClass::bit_array;

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

  case VarType::list:
  return VarTypeClass::list;

  case VarType::less_map:
  case VarType::greater_map:
  return VarTypeClass::map;

  case VarType::separator:
  case VarType::invalid_type:
  default:
  return VarTypeClass::invalid_type;

  }
}

inline bool operator == (VarType type, VarTypeClass type_class) noexcept {return toTypeClass(type) == type_class;}
inline bool operator == (VarTypeClass type_class, VarType type) noexcept {return toTypeClass(type) == type_class;}
inline bool operator != (VarType type, VarTypeClass type_class) noexcept {return toTypeClass(type) != type_class;}
inline bool operator != (VarTypeClass type_class, VarType type) noexcept {return toTypeClass(type) != type_class;}


inline VarBitWidth toBitWidth(VarType type) noexcept {
  switch (type) {
  case VarType::boolean:
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

inline bool operator == (VarType type, VarBitWidth bit_width) noexcept {return toBitWidth(type) == bit_width;}
inline bool operator == (VarBitWidth bit_width, VarType type) noexcept {return toBitWidth(type) == bit_width;}
inline bool operator != (VarType type, VarBitWidth bit_width) noexcept {return toBitWidth(type) != bit_width;}
inline bool operator != (VarBitWidth bit_width, VarType type) noexcept {return toBitWidth(type) != bit_width;}

inline VarBitWidth toBitWidth(ValType type) noexcept {
  switch (type) {
  case ValType::boolean:
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

inline bool operator == (ValType val_type, VarBitWidth bit_width) noexcept {return toBitWidth(val_type) == bit_width;}
inline bool operator == (VarBitWidth bit_width, ValType val_type) noexcept {return toBitWidth(val_type) == bit_width;}
inline bool operator != (ValType val_type, VarBitWidth bit_width) noexcept {return toBitWidth(val_type) != bit_width;}
inline bool operator != (VarBitWidth bit_width, ValType val_type) noexcept {return toBitWidth(val_type) != bit_width;}

inline VarNumberType toNumberType(VarType type) noexcept {
  switch (type) {
  case VarType::bit_array:
  case VarType::boolean:
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

inline bool operator == (VarType type, VarNumberType number_type) noexcept {return toNumberType(type) == number_type;}
inline bool operator == (VarNumberType number_type, VarType type) noexcept {return toNumberType(type) == number_type;}
inline bool operator != (VarType type, VarNumberType number_type) noexcept {return toNumberType(type) != number_type;}
inline bool operator != (VarNumberType number_type, VarType type) noexcept {return toNumberType(type) != number_type;}

inline VarNumberType toNumberType(ValType type) noexcept {
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

inline bool operator == (ValType val_type, VarNumberType number_type) noexcept {return toNumberType(val_type) == number_type;}
inline bool operator == (VarNumberType number_type, ValType val_type) noexcept {return toNumberType(val_type) == number_type;}
inline bool operator != (ValType val_type, VarNumberType number_type) noexcept {return toNumberType(val_type) != number_type;}
inline bool operator != (VarNumberType number_type, ValType val_type) noexcept {return toNumberType(val_type) != number_type;}

inline VarSortType toSortType(VarType type) noexcept {
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
  case VarType::list:
  return VarSortType::unsorted;

  case VarType::less_map:
  return VarSortType::less;

  case VarType::greater_map:
  return VarSortType::greater;

  default:
  return VarSortType::invalid_type;
  }
}

inline bool operator==(VarSortType sort_type, VarType type) noexcept {return sort_type == toSortType(type);}
inline bool operator==(VarType type, VarSortType sort_type) noexcept {return toSortType(type) == sort_type;}
inline bool operator!=(VarSortType sort_type, VarType type) noexcept {return sort_type != toSortType(type);}
inline bool operator!=(VarType type, VarSortType sort_type) noexcept {return toSortType(type) != sort_type;}

inline ValType toValueType(VarType type) noexcept {
  switch (type) {
  case VarType::boolean:
  return ValType::boolean;

  case VarType::ui8:
  case VarType::ui8_array:
  return ValType::ui8;

  case VarType::si8:
  case VarType::si8_array:
  return ValType::si8;

  case VarType::ui16:
  case VarType::ui16_array:
  return ValType::ui16;

  case VarType::si16:
  case VarType::si16_array:
  return ValType::si16;

  case VarType::ui32:
  case VarType::ui32_array:
  return ValType::ui32;

  case VarType::si32:
  case VarType::si32_array:
  return ValType::si32;

  case VarType::f32:
  case VarType::f32_array:
  return ValType::f32;

  case VarType::ui64:
  case VarType::ui64_array:
  return ValType::ui64;

  case VarType::si64:
  case VarType::si64_array:
  return ValType::si64;

  case VarType::f64:
  case VarType::f64_array:
  return ValType::f64;

  default:
  return ValType::invalid_type;
  }
}

inline VarType toVarType(ValType type) {
  return VarType(type);
}

inline VarType toBufferVarType(ValType type) {
  return VarType(ui8(type) + (ui8(VarType::ui8_array) - ui8(VarType::ui8)));
}

inline bool operator == (ValType val_type, VarType type) noexcept {return toVarType(val_type) == type || toBufferVarType(val_type) == type;}
inline bool operator == (VarType type, ValType val_type) noexcept {return toVarType(val_type) == type || toBufferVarType(val_type) == type;}
inline bool operator != (ValType val_type, VarType type) noexcept {return toVarType(val_type) != type && toBufferVarType(val_type) != type;}
inline bool operator != (VarType type, ValType val_type) noexcept {return toVarType(val_type) != type && toBufferVarType(val_type) != type;}


class Variable;

class Number;
class BitArray;
class BufferArray;
class Array;
class List;
class Map;

namespace literals {
namespace priv {

struct ArrayLiteral : public HeritableInitializerList<const Variable> {using HeritableInitializerList::HeritableInitializerList;};
struct ListLiteral  : public HeritableInitializerList<const Variable> {using HeritableInitializerList::HeritableInitializerList;};
struct LessMapLiteral;
struct GreaterMapLiteral;

}

typedef std::initializer_list<const bool>       bitarr;
typedef std::initializer_list<const ui8>        ui8arr;
typedef std::initializer_list<const i8>         i8arr;
typedef std::initializer_list<const ui16>       ui16arr;
typedef std::initializer_list<const i16>        i16arr;
typedef std::initializer_list<const ui32>       ui32arr;
typedef std::initializer_list<const i32>        i32arr;
typedef std::initializer_list<const f32>        f32arr;
typedef std::initializer_list<const ui64>       ui64arr;
typedef std::initializer_list<const i64>        i64arr;
typedef std::initializer_list<const f64>        f64arr;
typedef priv::ArrayLiteral                      arr;
typedef priv::ListLiteral                       list;
typedef priv::LessMapLiteral                    lmap;
typedef priv::GreaterMapLiteral                 gmap;

}

}

#endif // TYPES_HXX
