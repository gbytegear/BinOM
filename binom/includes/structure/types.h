#ifndef BINOM_TYPES_H
#define BINOM_TYPES_H

#include "malloc.h"
#include <initializer_list>
#include <cstring>
#include <string>
#include "../ctypes.h"
#include "../mem.h"
#include "../byte_array.h"


//Arch dependebce
#if __SIZEOF_POINTER__ == 2
#define PTR_SZ 2
#elif __SIZEOF_POINTER__ == 4
#define PTR_SZ 4
#elif __SIZEOF_POINTER__ == 8
#define PTR_SZ 8
#else
#error "Unsupported architecture!"
#endif


namespace binom {
  enum class VarType : byte {
    end                 = 0x00,
    byte                = 0x01,
    word                = 0x02,
    dword               = 0x03,
    qword               = 0x04,
    byte_array          = 0x05,
    word_array          = 0x06,
    dword_array         = 0x07,
    qword_array         = 0x08,
    array               = 0x09,
    object              = 0x0A,

    invalid_type         = 0xFF
  };

  enum class ValType : byte {
      byte                = 0x00,
      word                = 0x01,
      dword               = 0x02,
      qword               = 0x03,
  };

  enum class VarTypeClass : byte {
    primitive           = 0x01,
    buffer_array        = 0x02,
    array               = 0x03,
    object              = 0x04,

    invalid_type        = 0xFF
  };

  inline VarTypeClass toTypeClass(VarType type) {
    switch(type) {
      case VarType::byte: case VarType::word: case VarType::dword: case VarType::qword:
        return VarTypeClass::primitive;
      case VarType::byte_array: case VarType::word_array: case VarType::dword_array: case VarType::qword_array:
        return VarTypeClass::buffer_array;
      case VarType::array:
        return VarTypeClass::array;
      case VarType::object:
        return VarTypeClass::object;
      default:
        return VarTypeClass::invalid_type;
    }
  }

  inline ValType toValueType(VarType type) {
      switch (type) {
          case VarType::byte: return ValType::byte;
          case VarType::word: return ValType::word;
          case VarType::dword: return ValType::dword;
          case VarType::qword: return ValType::qword;
          case VarType::byte_array: return ValType::byte;
          case VarType::word_array: return ValType::word;
          case VarType::dword_array: return ValType::dword;
          case VarType::qword_array: return ValType::qword;
          default: throw Exception(ErrCode::binom_invalid_type);
      }
  }

  inline VarType toVarType(ValType type) {
    switch (type) {
      case ValType::byte: return VarType::byte;
      case ValType::word: return VarType::word;
      case ValType::dword:return VarType::dword;
      case ValType::qword:return VarType::qword;
      default: return VarType::invalid_type;
    }
  }

  inline VarType toVarBufferType(ValType type) {
    switch (type) {
      case ValType::byte: return VarType::byte_array;
      case ValType::word: return VarType::word_array;
      case ValType::dword:return VarType::dword_array;
      case ValType::qword:return VarType::qword_array;
      default: return VarType::invalid_type;
    }
  }

  constexpr ui8 toSize(ValType type) {
    switch (type) {
      case ValType::byte: return 1;
      case ValType::word: return 2;
      case ValType::dword: return 4;
      case ValType::qword: return 8;
      default: return 0;
    }
  }

  inline const char* toTypeString(VarType type) {
    switch (type) {
    case binom::VarType::end: return "end";
    case binom::VarType::byte:  return "byte";
    case binom::VarType::word: return "word";
    case binom::VarType::dword: return "dword";
    case binom::VarType::qword:  return "qword";
    case binom::VarType::byte_array: return "byte_array";
    case binom::VarType::word_array: return "word_array";
    case binom::VarType::dword_array: return "dword_array";
    case binom::VarType::qword_array: return "qword_array";
    case binom::VarType::array: return "array";
    case binom::VarType::object:  return "object";
    default:
    case binom::VarType::invalid_type: return "invalid_type";
    }
  }

  inline const char* toTypeString(VarTypeClass type_class) {
    switch (type_class) {
    case binom::VarTypeClass::primitive: return "primitive";
    case binom::VarTypeClass::buffer_array: return "buffer_array";
    case binom::VarTypeClass::array: return "array";
    case binom::VarTypeClass::object: return "object";
    default:
    case binom::VarTypeClass::invalid_type: return "invalid_type";
    }
  }

  inline const char* toTypeString(ValType val_type) {
    switch (val_type) {
    case binom::ValType::byte: return "byte";
    case binom::ValType::word: return "word";
    case binom::ValType::dword: return "dword";
    case binom::ValType::qword: return "qword";
    default: return "invalid_type";
    }
  }

  // Basic
  class Variable;
  class Primitive;
  class BufferArray;
  class Array;

  // Helpers
  class ValueRef;
  struct NamedVariable;

  // Iterators
  class ValueIterator;
  typedef Variable* ArrayIterator;
  typedef NamedVariable* ObjectIterator;

  // Node Visitors
  class NodeVisitor;
  class DBNodeVisitor;

  // Initilizers
  typedef std::initializer_list<ui8> ui8arr;
  typedef std::initializer_list<i8> i8arr;
  typedef std::initializer_list<ui16> ui16arr;
  typedef std::initializer_list<i16> i16arr;
  typedef std::initializer_list<ui32> ui32arr;
  typedef std::initializer_list<i32> i32arr;
  typedef std::initializer_list<ui64> ui64arr;
  typedef std::initializer_list<i64> i64arr;
  typedef std::initializer_list<Variable> varr;
  typedef std::initializer_list<NamedVariable> vobj;
}

#endif
