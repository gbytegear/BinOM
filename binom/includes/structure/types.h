#ifndef BINOM_TYPES_H
#define BINOM_TYPES_H

#include "malloc.h"
#include <initializer_list>
#include <cstring>
#include <string>
#include "../ctypes.h"
#include "../trymem.h"


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
    bool_false          = 0x01,
    bool_true           = 0x02,
    byte                = 0x03,
    word                = 0x04,
    dword               = 0x05,
    qword               = 0x06,
    byte_array          = 0x07,
    word_array          = 0x08,
    dword_array         = 0x09,
    qword_array         = 0x0A,
    array               = 0x0B,
    object              = 0x0C,
    matrix              = 0x0D,
    table               = 0x0E,

    invlid_type         = 0xFF
  };

  enum class VarTypeClass : byte {
    boolean             = 0x00,
    primitive           = 0x01,
    buffer_array        = 0x02,
    array               = 0x03,
    object              = 0x04,
    matrix              = 0x05,
    table               = 0x06,

    invalid_type        = 0xFF
  };

  enum class VarTemplateType : byte {
    end                 = 0x00,
    boolean             = 0x01,
    byte                = 0x02,
    word                = 0x03,
    dword               = 0x04,
    qword               = 0x05,
    byte_array          = 0x06,
    word_array          = 0x07,
    dword_array         = 0x08,
    qword_array         = 0x09,
    array               = 0x0A,
    object              = 0x0B,
    matrix              = 0x0C,
    table               = 0x0D
  };

  inline VarTypeClass toTypeClass(VarType type) {
    switch(type) {
      case VarType::bool_false: case VarType::bool_true:
        return VarTypeClass::boolean;
      case VarType::byte: case VarType::word: case VarType::dword: case VarType::qword:
        return VarTypeClass::primitive;
      case VarType::byte_array: case VarType::word_array: case VarType::dword_array: case VarType::qword_array:
        return VarTypeClass::buffer_array;
      case VarType::array:
        return VarTypeClass::array;
      case VarType::object:
        return VarTypeClass::object;
      case VarType::matrix:
        return VarTypeClass::matrix;
      case VarType::table:
        return VarTypeClass::table;
      default:
        return VarTypeClass::invalid_type;
    }
  }

  class Variable;
  class Boolean;
  class Primitive;
  class BufferArray;
  class Array;
  class Matrix;
  class Table;

  struct NamedVariable;
  class MatrixRowIterator;
  class TableRowIterator;

  // 
  typedef std::initializer_list<ui8> ui8arr;
  typedef std::initializer_list<i8> i8arr;
  typedef std::initializer_list<ui16> ui16arr;
  typedef std::initializer_list<i16> i16arr;
  typedef std::initializer_list<ui32> ui32arr;
  typedef std::initializer_list<i32> i32arr;
  typedef std::initializer_list<ui64> ui64arr;
  typedef std::initializer_list<i64> i64arr;
  struct mtrx;
  struct tbl;

  typedef std::initializer_list<Variable> varr;
  typedef std::initializer_list<NamedVariable> obj;


}

//#include "variables/variable.h"
//#include "variables/boolean.h"
//#include "variables/primitive.h"
//#include "variables/buffer_array.h"
//#include "variables/array.h"
//#include "variables/object.h"
//#include "variables/matrix.h"
//#include "variables/table.h"

#endif
