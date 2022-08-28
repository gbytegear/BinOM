#ifndef TYPES_HXX
#define TYPES_HXX

#include "../utils/memctrl.hxx"
#include "../utils/heritable_initializer_list.hxx"

#include <type_traits>
#include <initializer_list>

/// Binary Object Model
namespace binom {

using namespace type_alias;
using namespace memctrl;

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
  singly_linked_list      = 0x19, ///< Heterogeneous singly linked list
  doubly_linked_list      = 0x1A, ///< Heterogeneous doubly linked list
  map                     = 0x1B, ///< Associative heterogeneous container with key-sorted
  multimap                = 0x1C, ///< Associative heterogeneous container with key-sorted
  table                   = 0x1D, ///< Multiple key-sorted associative heterogeneous container

// Compile time defined C-like data types

  char_t                  = std::is_signed_v<char> ? VarType::si8 : VarType::ui8,
  uchar_t                 = VarType::ui8,
  schar_t                 = VarType::si8,

  int_t                   = sizeof (int) == 2 ? VarType::si16
                          : sizeof (int) == 4 ? VarType::si32
                          : sizeof (int) == 8 ? VarType::si64
                          : VarType::invalid_type,
  uint_t                  = sizeof (unsigned int) == 2 ? VarType::ui16
                          : sizeof (unsigned int) == 4 ? VarType::ui32
                          : sizeof (unsigned int) == 8 ? VarType::ui64
                          : VarType::invalid_type,

  short_t                 = sizeof (short) == 2 ? VarType::si16
                          : sizeof (short) == 4 ? VarType::si32
                          : sizeof (short) == 8 ? VarType::si64
                          : VarType::invalid_type,
  ushort_t                = sizeof (unsigned short) == 2 ? VarType::ui16
                          : sizeof (unsigned short) == 4 ? VarType::ui32
                          : sizeof (unsigned short) == 8 ? VarType::ui64
                          : VarType::invalid_type,

  long_t                  = sizeof (long) == 2 ? VarType::si16
                          : sizeof (long) == 4 ? VarType::si32
                          : sizeof (long) == 8 ? VarType::si64
                          : VarType::invalid_type,
  ulong_t                 = sizeof (unsigned long) == 2 ? VarType::ui16
                          : sizeof (unsigned long) == 4 ? VarType::ui32
                          : sizeof (unsigned long) == 8 ? VarType::ui64
                          : VarType::invalid_type,

  long_long_t             = sizeof (long long) == 2 ? VarType::si16
                          : sizeof (long long) == 4 ? VarType::si32
                          : sizeof (long long) == 8 ? VarType::si64
                          : VarType::invalid_type,
  ulong_long_t            = sizeof (unsigned long long) == 2 ? VarType::ui16
                          : sizeof (unsigned long long) == 4 ? VarType::ui32
                          : sizeof (unsigned long long) == 8 ? VarType::ui64
                          : VarType::invalid_type,
  float_t                 = VarType::f32,
  long_float_t            = VarType::f64,
  long_long_float_t       = sizeof (double) == sizeof (long double) ? VarType::f64
                            : VarType::invalid_type, // Unsupported in BinOM storages, architecture-depends implementation!

  char_array              = int(int(VarType::char_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  uchar_array             = int(int(VarType::uchar_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  schar_array             = int(int(VarType::schar_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  int_array               = int(int(VarType::int_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  uint_array              = int(int(VarType::uint_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  short_array             = int(int(VarType::short_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  ushort_array            = int(int(VarType::ushort_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  long_array              = int(int(VarType::long_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  ulong_array             = int(int(VarType::ulong_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  long_long_array         = int(int(VarType::long_long_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  ulong_long_array        = int(int(VarType::ulong_long_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  float_array             = int(int(VarType::float_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  long_float_array        = int(int(VarType::long_float_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  long_long_float_array   = sizeof (double) == sizeof (long double) ? VarType::f64_array
                          : VarType::invalid_type, // Unsupported in BinOM storages, architecture-depends implementation!

  list                    = doubly_linked_list
};

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

  char_t                  = std::is_signed_v<char> ? VarKeyType::si8 : VarKeyType::ui8,
  uchar_t                 = VarKeyType::ui8,
  schar_t                 = VarKeyType::si8,

  int_t                   = sizeof (int) == 2 ? VarKeyType::si16
                          : sizeof (int) == 4 ? VarKeyType::si32
                          : sizeof (int) == 8 ? VarKeyType::si64
                          : VarKeyType::invalid_type,
  uint_t                  = sizeof (unsigned int) == 2 ? VarKeyType::ui16
                          : sizeof (unsigned int) == 4 ? VarKeyType::ui32
                          : sizeof (unsigned int) == 8 ? VarKeyType::ui64
                          : VarKeyType::invalid_type,

  short_t                 = sizeof (short) == 2 ? VarKeyType::si16
                          : sizeof (short) == 4 ? VarKeyType::si32
                          : sizeof (short) == 8 ? VarKeyType::si64
                          : VarKeyType::invalid_type,
  ushort_t                = sizeof (unsigned short) == 2 ? VarKeyType::ui16
                          : sizeof (unsigned short) == 4 ? VarKeyType::ui32
                          : sizeof (unsigned short) == 8 ? VarKeyType::ui64
                          : VarKeyType::invalid_type,

  long_t                  = sizeof (long) == 2 ? VarKeyType::si16
                          : sizeof (long) == 4 ? VarKeyType::si32
                          : sizeof (long) == 8 ? VarKeyType::si64
                          : VarKeyType::invalid_type,
  ulong_t                 = sizeof (unsigned long) == 2 ? VarKeyType::ui16
                          : sizeof (unsigned long) == 4 ? VarKeyType::ui32
                          : sizeof (unsigned long) == 8 ? VarKeyType::ui64
                          : VarKeyType::invalid_type,

  long_long_t             = sizeof (long long) == 2 ? VarKeyType::si16
                          : sizeof (long long) == 4 ? VarKeyType::si32
                          : sizeof (long long) == 8 ? VarKeyType::si64
                          : VarKeyType::invalid_type,
  ulong_long_t            = sizeof (unsigned long long) == 2 ? VarKeyType::ui16
                          : sizeof (unsigned long long) == 4 ? VarKeyType::ui32
                          : sizeof (unsigned long long) == 8 ? VarKeyType::ui64
                          : VarKeyType::invalid_type,
  float_t                 = VarKeyType::f32,
  long_float_t            = VarKeyType::f64,
  long_long_float_t       = sizeof (double) == sizeof (long double) ? VarKeyType::f64
                            : VarKeyType::invalid_type, // Unsupported in BinOM storages, architecture-depends implementation!

  char_array              = int(int(VarType::char_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  uchar_array             = int(int(VarType::uchar_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  schar_array             = int(int(VarType::schar_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  int_array               = int(int(VarType::int_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  uint_array              = int(int(VarType::uint_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  short_array             = int(int(VarType::short_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  ushort_array            = int(int(VarType::ushort_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  long_array              = int(int(VarType::long_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  ulong_array             = int(int(VarType::ulong_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  long_long_array         = int(int(VarType::long_long_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  ulong_long_array        = int(int(VarType::ulong_long_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  float_array             = int(int(VarType::float_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  long_float_array        = int(int(VarType::long_float_t) + (int(VarType::ui8_array) - int(VarType::ui8))),
  long_long_float_array   = sizeof (double) == sizeof (long double) ? VarKeyType::f64_array
                          : VarKeyType::invalid_type, // Unsupported in BinOM storages, architecture-depends implementation!
};

enum class VarTypeClass : ui8 {
  null                    = 0x01,
  number                  = 0x02,
  bit_array               = 0x03,
  buffer_array            = 0x04,
  array                   = 0x05,
  singly_linked_list      = 0x06,
  doubly_linked_list      = 0x07,
  map                     = 0x08,
  multimap                = 0x09,
  table                   = 0x0A,

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

// Compile time defined C++ data types

  char_t                  = std::is_signed_v<char> ? ValType::si8 : ValType::ui8,
  uchar_t                 = ValType::ui8,
  schar_t                 = ValType::si8,

  int_t                   = sizeof (int) == 2 ? ValType::si16
                          : sizeof (int) == 4 ? ValType::si32
                          : sizeof (int) == 8 ? ValType::si64
                          : ValType::invalid_type,
  uint_t                  = sizeof (unsigned int) == 2 ? ValType::ui16
                          : sizeof (unsigned int) == 4 ? ValType::ui32
                          : sizeof (unsigned int) == 8 ? ValType::ui64
                          : ValType::invalid_type,

  short_t                 = sizeof (short) == 2 ? ValType::si16
                          : sizeof (short) == 4 ? ValType::si32
                          : sizeof (short) == 8 ? ValType::si64
                          : ValType::invalid_type,
  ushort_t                = sizeof (unsigned short) == 2 ? ValType::ui16
                          : sizeof (unsigned short) == 4 ? ValType::ui32
                          : sizeof (unsigned short) == 8 ? ValType::ui64
                          : ValType::invalid_type,

  long_t                  = sizeof (long) == 2 ? ValType::si16
                          : sizeof (long) == 4 ? ValType::si32
                          : sizeof (long) == 8 ? ValType::si64
                          : ValType::invalid_type,
  ulong_t                 = sizeof (unsigned long) == 2 ? ValType::ui16
                          : sizeof (unsigned long) == 4 ? ValType::ui32
                          : sizeof (unsigned long) == 8 ? ValType::ui64
                          : ValType::invalid_type,

  long_long_t             = sizeof (long long) == 2 ? ValType::si16
                          : sizeof (long long) == 4 ? ValType::si32
                          : sizeof (long long) == 8 ? ValType::si64
                          : ValType::invalid_type,
  ulong_long_t            = sizeof (unsigned long long) == 2 ? ValType::ui16
                          : sizeof (unsigned long long) == 4 ? ValType::ui32
                          : sizeof (unsigned long long) == 8 ? ValType::ui64
                          : ValType::invalid_type,
  float_t                 = ValType::f32,
  long_float_t            = ValType::f64,
  long_long_float_t       = sizeof (double) == sizeof (long double) ? ValType::f64
                            : ValType::invalid_type, // Unsupported in BinOM storages, architecture-depends implementation!
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

  case VarType::array: return VarTypeClass::array;

  case VarType::singly_linked_list: return VarTypeClass::singly_linked_list;

  case VarType::doubly_linked_list: return VarTypeClass::doubly_linked_list;

  case VarType::map: return VarTypeClass::map;

  case VarType::multimap: return VarTypeClass::multimap;

  case VarType::table: return VarTypeClass::table;

  case VarType::separator:
  case VarType::invalid_type:
  default:
  return VarTypeClass::invalid_type;

  }
}

inline VarTypeClass toTypeClass(VarKeyType type) noexcept {
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

inline bool operator == (VarType type, VarTypeClass type_class) noexcept {return toTypeClass(type) == type_class;}
inline bool operator == (VarTypeClass type_class, VarType type) noexcept {return toTypeClass(type) == type_class;}
inline bool operator != (VarType type, VarTypeClass type_class) noexcept {return toTypeClass(type) != type_class;}
inline bool operator != (VarTypeClass type_class, VarType type) noexcept {return toTypeClass(type) != type_class;}

inline VarKeyType toKeyType(VarType type) noexcept {
  if(type <= VarType::f64_array) return VarKeyType(type);
  else return VarKeyType::invalid_type;
}

inline VarKeyType toKeyType(ValType type) noexcept {
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

inline VarType toVarType(VarKeyType key_type) noexcept {return VarType(key_type);}

inline bool operator == (VarType type, VarKeyType key_type) noexcept {return toKeyType(type) == key_type;}
inline bool operator == (VarKeyType key_type, VarType type) noexcept {return toKeyType(type) == key_type;}
inline bool operator != (VarType type, VarKeyType key_type) noexcept {return toKeyType(type) != key_type;}
inline bool operator != (VarKeyType key_type, VarType type) noexcept {return toKeyType(type) != key_type;}

inline bool operator == (ValType type, VarKeyType key_type) noexcept {return toKeyType(type) == key_type;}
inline bool operator == (VarKeyType key_type, ValType type) noexcept {return toKeyType(type) == key_type;}
inline bool operator != (ValType type, VarKeyType key_type) noexcept {return toKeyType(type) != key_type;}
inline bool operator != (VarKeyType key_type, ValType type) noexcept {return toKeyType(type) != key_type;}

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

inline ValType toValueType(VarType type) noexcept {
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

inline ValType toValueType(VarKeyType type) noexcept {
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

namespace priv { // BinOM containers implementation

class BitArrayImplementation;
class BufferArrayImplementation;
class ArrayImplementation;
class SinglyLinkedListImplementation;
class DoublyLinkedListImplementation;
class MapImplementation;
class MultiMapImplementation;
class TableImplementation;
class KeyValueImplementation;

class AVLNode;
class AVLTree;

}

class Variable;

class Number;
class BitArray;
class BufferArray;
class Array;
class SinglyLinkedList;
class DoublyLinkedList;
class Map;
class MultiMap;
class Table;
class KeyValue;

class NamedVariable;

namespace literals {
namespace priv {

struct ArrayLiteral : public heritable_initializer_list::HeritableInitializerList<const Variable> {using HeritableInitializerList::HeritableInitializerList;};
struct SinglyLinkedListLiteral  : public heritable_initializer_list::HeritableInitializerList<const Variable> {using HeritableInitializerList::HeritableInitializerList;};
struct DoublyLinkedListLiteral  : public heritable_initializer_list::HeritableInitializerList<const Variable> {using HeritableInitializerList::HeritableInitializerList;};
struct MapLiteral : public heritable_initializer_list::HeritableInitializerList<const NamedVariable> {using HeritableInitializerList::HeritableInitializerList;};
struct MultiMapLiteral : public heritable_initializer_list::HeritableInitializerList<const NamedVariable> {using HeritableInitializerList::HeritableInitializerList;};
struct TableLiteral;

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
typedef priv::SinglyLinkedListLiteral           sllist;
typedef priv::DoublyLinkedListLiteral           dllist;
typedef priv::MapLiteral                        map;
typedef priv::MultiMapLiteral                   multimap;
typedef priv::TableLiteral                      table;

}

}

#endif // TYPES_HXX
