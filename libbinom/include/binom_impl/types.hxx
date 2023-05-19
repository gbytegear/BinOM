#ifndef TYPES_HXX
#define TYPES_HXX

#include "../utils/err.hxx"
#include "../utils/heritable_initializer_list.hxx"
#include "../utils/extended_type_traits.hxx"

#include <initializer_list>
#include <utility>

/**
 * @brief Binary Object Model namespace
 */
namespace binom {

using namespace type_alias;
using namespace binom::err;

//! Table column index type
enum class IndexType {
  unique_index,
  multi_index
};

//! Map or MultiMap field type
enum class FieldType : ui8 {
  local   = 0x00,
  indexed = 0x01,
  empty   = 0xFF
};

namespace priv {

// Shared resource implementation
class Link;
class SharedResource;
class ResourceData;
class FileResourceIndex;

// BinOM containers implementation
class BitArrayImplementation;
class BufferArrayImplementation;
class ArrayImplementation;
class ListImplementation;
class MapImplementation;
class MultiMapImplementation;
class TableImplementation;
class KeyValueImplementation;

}

namespace index {

class Index;
struct IndexComparator;
class Field;
struct MapComparator;
class Iterator;
class ConstIterator;
class ReverseIterator;
class ConstReverseIterator;

}

// BinOM generic variables
class Variable;
class KeyValue;

// BinOM containers
class Number;
class BitArray;
class BufferArray;
class Array;
class List;
class Map;
class MultiMap;
class Table;

// Container support types
class FieldInit;
class FieldRef;

namespace conditions {

class ConditionQuery;
class ConditionExpression;

}

namespace literals {
namespace priv {

struct ArrayLiteral             : public heritable_initializer_list::HeritableInitializerList<const Variable>      {using HeritableInitializerList::HeritableInitializerList;};
struct ListLiteral              : public heritable_initializer_list::HeritableInitializerList<const Variable>      {using HeritableInitializerList::HeritableInitializerList;};
struct MapLiteral               : public heritable_initializer_list::HeritableInitializerList<const FieldInit> {using HeritableInitializerList::HeritableInitializerList;};
struct MultiMapLiteral          : public heritable_initializer_list::HeritableInitializerList<const FieldInit> {using HeritableInitializerList::HeritableInitializerList;};

struct TableRowLiteral {
  enum LiteralType {
    map,
    multimap
  } type;

  union RowData {
    MapLiteral map_init_list;
    MultiMapLiteral multimap_init_list;
  } data;

  TableRowLiteral(MapLiteral map_literal)
    : type(LiteralType::map), data{.map_init_list{map_literal}} {}

  TableRowLiteral(MultiMapLiteral multi_map_literal)
    : type(LiteralType::multimap), data{.multimap_init_list{multi_map_literal}} {}
};

struct TableLiteral {
  std::initializer_list<std::pair<KeyValue, IndexType>> header;
  std::initializer_list<TableRowLiteral> row_list = {};
  std::initializer_list<binom::conditions::ConditionQuery> constrait_list = {};
};

}

// BinOM Container literals
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
typedef priv::MapLiteral                        map;
typedef priv::MultiMapLiteral                   multimap;
typedef priv::TableLiteral                      table;

}

#define getIntType(Enum, Type) \
std::is_signed_v<Type> ? ( \
    sizeof (Type) == 1 ? Enum::si8 \
  : sizeof (Type) == 2 ? Enum::si16 \
  : sizeof (Type) == 4 ? Enum::si32 \
  : sizeof (Type) == 8 ? Enum::si64 \
  : Enum::invalid_type \
) : ( \
    sizeof (Type) == 1 ? Enum::ui8 \
  : sizeof (Type) == 2 ? Enum::ui16 \
  : sizeof (Type) == 4 ? Enum::ui32 \
  : sizeof (Type) == 8 ? Enum::ui64 \
  : Enum::invalid_type \
)

#define getIntArrType(Enum, Type) \
std::is_signed_v<Type> ? ( \
    sizeof (Type) == 1 ? Enum::si8_array \
  : sizeof (Type) == 2 ? Enum::si16_array \
  : sizeof (Type) == 4 ? Enum::si32_array \
  : sizeof (Type) == 8 ? Enum::si64_array \
  : Enum::invalid_type \
) : ( \
    sizeof (Type) == 1 ? Enum::ui8_array \
  : sizeof (Type) == 2 ? Enum::ui16_array \
  : sizeof (Type) == 4 ? Enum::ui32_array \
  : sizeof (Type) == 8 ? Enum::ui64_array \
  : Enum::invalid_type \
)

/// BinOM Variable type codes
enum class VarType : ui8 {
  separator               = 0x00, ///< End code
  invalid_type            = 0xFF, ///< Invalid type code

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
  list                    = 0x19, ///< Heterogeneous doubly linked list
  map                     = 0x1A, ///< Associative heterogeneous container with key-sorted
  multimap                = 0x1B, ///< Associative heterogeneous container with key-sorted
  table                   = 0x1C, ///< Multiple key-sorted associative heterogeneous container

// Compile-time defined C-like data types

  char_t                  = getIntType(VarType, char),
  uchar_t                 = VarType::ui8,
  schar_t                 = VarType::si8,

  char8                   = getIntType(VarType, char8_t),
  char16                  = getIntType(VarType, char16_t),
  char32                  = getIntType(VarType, char32_t),
  wchar                   = getIntType(VarType, wchar_t),

  int_t                   = getIntType(VarType, int),
  uint_t                  = getIntType(VarType, unsigned int),

  short_t                 = getIntType(VarType, short),
  ushort_t                = getIntType(VarType, unsigned short),

  long_t                  = getIntType(VarType, long),
  ulong_t                 = getIntType(VarType, unsigned long),

  long_long_t             = getIntType(VarType, long long),
  ulong_long_t            = getIntType(VarType, unsigned long long),
  float_t                 = VarType::f32,
  long_float_t            = VarType::f64,
  long_long_float_t       = sizeof (double) == sizeof (long double) ? VarType::f64 : VarType::invalid_type,

  char_array              = getIntArrType(VarType, char),
  uchar_array             = getIntArrType(VarType, unsigned char),
  schar_array             = getIntArrType(VarType, signed char),
  int_array               = getIntArrType(VarType, int),
  uint_array              = getIntArrType(VarType, unsigned int),
  short_array             = getIntArrType(VarType, short),
  ushort_array            = getIntArrType(VarType, unsigned short),
  long_array              = getIntArrType(VarType, long),
  ulong_array             = getIntArrType(VarType, unsigned long),
  long_long_array         = getIntArrType(VarType, long long),
  ulong_long_array        = getIntArrType(VarType, unsigned long long),
  float_array             = int(int(VarType::float_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  long_float_array        = int(int(VarType::long_float_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  long_long_float_array   = sizeof (double) == sizeof (long double) ? VarType::f64_array : VarType::invalid_type,
};

//! BinOM KeyValue type
enum class VarKeyType : ui8 {
  separator               = int(VarType::separator),    ///< End code
  invalid_type            = int(VarType::invalid_type), ///< Invalid type code

  null                    = int(VarType::null),       ///< NULL
  boolean                 = int(VarType::boolean),    ///< Boolean value
  ui8                     = int(VarType::ui8),        ///< Unsigned 8-bit integer number
  si8                     = int(VarType::si8),        ///< Signed 8-bit integer number
  ui16                    = int(VarType::ui16),       ///< Unsigned 16-bit integer number
  si16                    = int(VarType::si16),       ///< Signed 16-bit integer number
  ui32                    = int(VarType::ui32),       ///< Unsigned 32-bit integer number
  si32                    = int(VarType::si32),       ///< Signed 32-bit integer number
  f32                     = int(VarType::f32),        ///< 32-bit number with floating point
  ui64                    = int(VarType::ui64),       ///< Unsigned 64-bit integer number
  si64                    = int(VarType::si64),       ///< Signed 64-bit integer number
  f64                     = int(VarType::f64),        ///< 64-bit number with floating point
  bit_array               = int(VarType::bit_array),  ///< Array of boolean values
  ui8_array               = int(VarType::ui8_array),  ///< Array of unsigned 8-bit integer numbers
  si8_array               = int(VarType::si8_array),  ///< Array of signed 8-bit integer numbers
  ui16_array              = int(VarType::ui16_array), ///< Array of unsigned 16-bit integer numbers
  si16_array              = int(VarType::si16_array), ///< Array of signed 16-bit integer numbers
  ui32_array              = int(VarType::ui32_array), ///< Array of unsigned 32-bit integer numbers
  si32_array              = int(VarType::si32_array), ///< Array of signed 32-bit integer numbers
  f32_array               = int(VarType::f32_array),  ///< Array of 32-bit numbers with floating point
  ui64_array              = int(VarType::ui64_array), ///< Array of unsigned 64-bit integer numbers
  si64_array              = int(VarType::si64_array), ///< Array of signed 64-bit integer numbers
  f64_array               = int(VarType::f64_array),  ///< Array of 64-bit numbers with floating point

// Compile time defined C++ data types

  char_t                  = getIntType(VarKeyType, char),
  uchar_t                 = VarKeyType::ui8,
  schar_t                 = VarKeyType::si8,

  char8                   = getIntType(VarKeyType, char8_t),
  char16                  = getIntType(VarKeyType, char16_t),
  char32                  = getIntType(VarKeyType, char32_t),
  wchar                   = getIntType(VarKeyType, wchar_t),

  int_t                   = getIntType(VarKeyType, int),
  uint_t                  = getIntType(VarKeyType, unsigned int),

  short_t                 = getIntType(VarKeyType, short),
  ushort_t                = getIntType(VarKeyType, unsigned short),

  long_t                  = getIntType(VarKeyType, long),
  ulong_t                 = getIntType(VarKeyType, unsigned long),

  long_long_t             = getIntType(VarKeyType, long long),
  ulong_long_t            = getIntType(VarKeyType, unsigned long long),
  float_t                 = VarKeyType::f32,
  long_float_t            = VarKeyType::f64,
  long_long_float_t       = sizeof (double) == sizeof (long double) ? VarKeyType::f64 : VarKeyType::invalid_type,

  char_array              = getIntArrType(VarKeyType, char),
  uchar_array             = getIntArrType(VarKeyType, unsigned char),
  schar_array             = getIntArrType(VarKeyType, signed char),
  int_array               = getIntArrType(VarKeyType, int),
  uint_array              = getIntArrType(VarKeyType, unsigned int),
  short_array             = getIntArrType(VarKeyType, short),
  ushort_array            = getIntArrType(VarKeyType, unsigned short),
  long_array              = getIntArrType(VarKeyType, long),
  ulong_array             = getIntArrType(VarKeyType, unsigned long),
  long_long_array         = getIntArrType(VarKeyType, long long),
  ulong_long_array        = getIntArrType(VarKeyType, unsigned long long),
  float_array             = int(int(VarType::float_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  long_float_array        = int(int(VarType::long_float_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  long_long_float_array   = sizeof (double) == sizeof (long double) ? VarKeyType::f64_array : VarKeyType::invalid_type
};

//! BinOM Variable type class
enum class VarTypeClass : ui8 {
  null                    = 0x01,
  number                  = 0x02,
  bit_array               = 0x03,
  buffer_array            = 0x04,
  array                   = 0x05,
  list                    = 0x06,
  map                     = 0x07,
  multimap                = 0x08,
  table                   = 0x09,

  invalid_type            = int(VarType::invalid_type)
};

//! BinOM Number/BufferArray bit width
enum class VarBitWidth : ui8 {
  byte                    = sizeof (byte),
  word                    = sizeof (word),
  dword                   = sizeof (dword),
  qword                   = sizeof (qword),

  invalid_type            = int(VarType::invalid_type)
};

//! BinOM Number/BufferArray number type
enum class VarNumberType : ui8 {
  unsigned_integer        = 0x00,
  signed_integer          = 0x01,
  float_point             = 0x02,

  invalid_type            = int(VarType::invalid_type)
};

//! BinOM Number/BufferArray value type
enum class ValType : ui8 {
  invalid_type            = int(VarType::invalid_type),
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

// Compile-time defined C++ data types

  char_t                  = getIntType(ValType, char),
  uchar_t                 = ValType::ui8,
  schar_t                 = ValType::si8,

  char8                   = getIntType(ValType, char8_t),
  char16                  = getIntType(ValType, char16_t),
  char32                  = getIntType(ValType, char32_t),
  wchar                   = getIntType(ValType, wchar_t),

  int_t                   = getIntType(ValType, int),
  uint_t                  = getIntType(ValType, unsigned int),

  short_t                 = getIntType(ValType, short),
  ushort_t                = getIntType(ValType, unsigned short),

  long_t                  = getIntType(ValType, long),
  ulong_t                 = getIntType(ValType, unsigned long),

  long_long_t             = getIntType(ValType, long long),
  ulong_long_t            = getIntType(ValType, unsigned long long),
  float_t                 = ValType::f32,
  long_float_t            = ValType::f64,
  long_long_float_t       = sizeof (double) == sizeof (long double) ? ValType::f64 : ValType::invalid_type
};

template<typename T>
requires std::is_arithmetic_v<T> &&
(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8)
constexpr VarType to_number_type = []() consteval {
  switch (sizeof (T)) {
  case 1: return std::is_signed_v<T> ? VarType::si8 : VarType::ui8;
  case 2: return std::is_signed_v<T> ? VarType::si16 : VarType::ui16;
  case 4: return std::is_signed_v<T> ? std::is_floating_point_v<T> ? VarType::f32 : VarType::si32 : VarType::ui32;
  case 8: return std::is_signed_v<T> ? std::is_floating_point_v<T> ? VarType::f64 : VarType::si64 : VarType::ui64;
  }
}();

template<typename T>
requires std::is_arithmetic_v<T> &&
(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8)
constexpr VarType to_buffer_array_type = []() consteval {
  switch (sizeof (T)) {
  case 1: return std::is_signed_v<T> ? VarType::si8_array : VarType::ui8_array;
  case 2: return std::is_signed_v<T> ? VarType::si16_array : VarType::ui16_array;
  case 4: return std::is_signed_v<T> ? std::is_floating_point_v<T> ? VarType::f32_array : VarType::si32_array : VarType::ui32_array;
  case 8: return std::is_signed_v<T> ? std::is_floating_point_v<T> ? VarType::f64_array : VarType::si64_array : VarType::ui64_array;
  }
}();

#undef getIntType
#undef getIntArrType

constexpr inline VarTypeClass toTypeClass(VarType type) noexcept {
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

  case VarType::array: return VarTypeClass::array;

  case VarType::list: return VarTypeClass::list;

  case VarType::map: return VarTypeClass::map;

  case VarType::multimap: return VarTypeClass::multimap;

  case VarType::table: return VarTypeClass::table;

  case VarType::separator:
  case VarType::invalid_type:
  default:
  return VarTypeClass::invalid_type;

  }
}

constexpr inline VarTypeClass toTypeClass(VarKeyType type) noexcept {
  switch (type) {
  case VarKeyType::null: return VarTypeClass::null;
  case VarKeyType::boolean:
  case VarKeyType::ui8:
  case VarKeyType::si8:
  case VarKeyType::ui16:
  case VarKeyType::si16:
  case VarKeyType::ui32:
  case VarKeyType::si32:
  case VarKeyType::f32:
  case VarKeyType::ui64:
  case VarKeyType::si64:
  case VarKeyType::f64:
  return VarTypeClass::number;

  case VarKeyType::bit_array:
  return VarTypeClass::bit_array;

  case VarKeyType::ui8_array:
  case VarKeyType::si8_array:
  case VarKeyType::ui16_array:
  case VarKeyType::si16_array:
  case VarKeyType::ui32_array:
  case VarKeyType::si32_array:
  case VarKeyType::f32_array:
  case VarKeyType::ui64_array:
  case VarKeyType::si64_array:
  case VarKeyType::f64_array:
  return VarTypeClass::buffer_array;

  case VarKeyType::separator:
  case VarKeyType::invalid_type:
  default:
  return VarTypeClass::invalid_type;
  }
}

constexpr inline bool operator == (VarType type, VarTypeClass type_class) noexcept {return toTypeClass(type) == type_class;}
constexpr inline bool operator == (VarTypeClass type_class, VarType type) noexcept {return toTypeClass(type) == type_class;}
constexpr inline bool operator != (VarType type, VarTypeClass type_class) noexcept {return toTypeClass(type) != type_class;}
constexpr inline bool operator != (VarTypeClass type_class, VarType type) noexcept {return toTypeClass(type) != type_class;}

constexpr inline VarKeyType toKeyType(VarType type) noexcept {
  if(type <= VarType::f64_array) return VarKeyType(type);
  else return VarKeyType::invalid_type;
}

constexpr inline VarKeyType toKeyType(ValType type) noexcept {
  switch (type) {
  case ValType::boolean:      return VarKeyType::boolean;
  case ValType::ui8:          return VarKeyType::ui8;
  case ValType::si8:          return VarKeyType::si8;
  case ValType::ui16:         return VarKeyType::ui16;
  case ValType::si16:         return VarKeyType::si16;
  case ValType::ui32:         return VarKeyType::ui32;
  case ValType::si32:         return VarKeyType::si32;
  case ValType::f32:          return VarKeyType::f32;
  case ValType::ui64:         return VarKeyType::ui64;
  case ValType::si64:         return VarKeyType::si64;
  case ValType::f64:          return VarKeyType::f64;
  default:
  case ValType::invalid_type: return VarKeyType::invalid_type;
  }
}

constexpr inline VarKeyType toKeyBufferType(ValType type) noexcept {
  switch (type) {
  case ValType::ui8:          return VarKeyType::ui8_array;
  case ValType::si8:          return VarKeyType::si8_array;
  case ValType::ui16:         return VarKeyType::ui16_array;
  case ValType::si16:         return VarKeyType::si16_array;
  case ValType::ui32:         return VarKeyType::ui32_array;
  case ValType::si32:         return VarKeyType::si32_array;
  case ValType::f32:          return VarKeyType::f32_array;
  case ValType::ui64:         return VarKeyType::ui64_array;
  case ValType::si64:         return VarKeyType::si64_array;
  case ValType::f64:          return VarKeyType::f64_array;
  default:
  case ValType::invalid_type: return VarKeyType::invalid_type;
  }
}

constexpr inline VarType toVarType(VarKeyType key_type) noexcept {return VarType(key_type);}

constexpr inline bool operator == (VarType type, VarKeyType key_type) noexcept {return toKeyType(type) == key_type;}
constexpr inline bool operator == (VarKeyType key_type, VarType type) noexcept {return toKeyType(type) == key_type;}
constexpr inline bool operator != (VarType type, VarKeyType key_type) noexcept {return toKeyType(type) != key_type;}
constexpr inline bool operator != (VarKeyType key_type, VarType type) noexcept {return toKeyType(type) != key_type;}

constexpr inline bool operator == (ValType type, VarKeyType key_type) noexcept {return toKeyType(type) == key_type;}
constexpr inline bool operator == (VarKeyType key_type, ValType type) noexcept {return toKeyType(type) == key_type;}
constexpr inline bool operator != (ValType type, VarKeyType key_type) noexcept {return toKeyType(type) != key_type;}
constexpr inline bool operator != (VarKeyType key_type, ValType type) noexcept {return toKeyType(type) != key_type;}

constexpr inline VarBitWidth toBitWidth(VarType type) noexcept {
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

constexpr inline bool operator == (VarType type, VarBitWidth bit_width) noexcept {return toBitWidth(type) == bit_width;}
constexpr inline bool operator == (VarBitWidth bit_width, VarType type) noexcept {return toBitWidth(type) == bit_width;}
constexpr inline bool operator != (VarType type, VarBitWidth bit_width) noexcept {return toBitWidth(type) != bit_width;}
constexpr inline bool operator != (VarBitWidth bit_width, VarType type) noexcept {return toBitWidth(type) != bit_width;}

constexpr inline VarBitWidth toBitWidth(ValType type) noexcept {
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

constexpr inline bool operator == (ValType val_type, VarBitWidth bit_width) noexcept {return toBitWidth(val_type) == bit_width;}
constexpr inline bool operator == (VarBitWidth bit_width, ValType val_type) noexcept {return toBitWidth(val_type) == bit_width;}
constexpr inline bool operator != (ValType val_type, VarBitWidth bit_width) noexcept {return toBitWidth(val_type) != bit_width;}
constexpr inline bool operator != (VarBitWidth bit_width, ValType val_type) noexcept {return toBitWidth(val_type) != bit_width;}

constexpr inline VarNumberType toNumberType(VarType type) noexcept {
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

constexpr inline bool operator == (VarType type, VarNumberType number_type) noexcept {return toNumberType(type) == number_type;}
constexpr inline bool operator == (VarNumberType number_type, VarType type) noexcept {return toNumberType(type) == number_type;}
constexpr inline bool operator != (VarType type, VarNumberType number_type) noexcept {return toNumberType(type) != number_type;}
constexpr inline bool operator != (VarNumberType number_type, VarType type) noexcept {return toNumberType(type) != number_type;}

constexpr inline VarNumberType toNumberType(ValType type) noexcept {
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

constexpr inline bool operator == (ValType val_type, VarNumberType number_type) noexcept {return toNumberType(val_type) == number_type;}
constexpr inline bool operator == (VarNumberType number_type, ValType val_type) noexcept {return toNumberType(val_type) == number_type;}
constexpr inline bool operator != (ValType val_type, VarNumberType number_type) noexcept {return toNumberType(val_type) != number_type;}
constexpr inline bool operator != (VarNumberType number_type, ValType val_type) noexcept {return toNumberType(val_type) != number_type;}

constexpr inline ValType toValueType(VarType type) noexcept {
  switch (type) {
  case VarType::boolean:
  case VarType::bit_array:
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

constexpr inline ValType toValueType(VarKeyType type) noexcept {
  switch (type) {
  case VarKeyType::boolean:
  case VarKeyType::bit_array:
  return ValType::boolean;

  case VarKeyType::ui8:
  case VarKeyType::ui8_array:
  return ValType::ui8;

  case VarKeyType::si8:
  case VarKeyType::si8_array:
  return ValType::si8;

  case VarKeyType::ui16:
  case VarKeyType::ui16_array:
  return ValType::ui16;

  case VarKeyType::si16:
  case VarKeyType::si16_array:
  return ValType::si16;

  case VarKeyType::ui32:
  case VarKeyType::ui32_array:
  return ValType::ui32;

  case VarKeyType::si32:
  case VarKeyType::si32_array:
  return ValType::si32;

  case VarKeyType::f32:
  case VarKeyType::f32_array:
  return ValType::f32;

  case VarKeyType::ui64:
  case VarKeyType::ui64_array:
  return ValType::ui64;

  case VarKeyType::si64:
  case VarKeyType::si64_array:
  return ValType::si64;

  case VarKeyType::f64:
  case VarKeyType::f64_array:
  return ValType::f64;

  default:
  return ValType::invalid_type;
  }
}

constexpr inline VarType toVarType(ValType type) {
  return VarType(type);
}

constexpr inline VarType toBufferVarType(ValType type) {
  return VarType(ui8(type) + (ui8(VarType::ui8_array) - ui8(VarType::ui8)));
}

constexpr inline bool operator == (ValType val_type, VarType type) noexcept {return toVarType(val_type) == type || toBufferVarType(val_type) == type;}
constexpr inline bool operator == (VarType type, ValType val_type) noexcept {return toVarType(val_type) == type || toBufferVarType(val_type) == type;}
constexpr inline bool operator != (ValType val_type, VarType type) noexcept {return toVarType(val_type) != type && toBufferVarType(val_type) != type;}
constexpr inline bool operator != (VarType type, ValType val_type) noexcept {return toVarType(val_type) != type && toBufferVarType(val_type) != type;}


}

#endif // TYPES_HXX
