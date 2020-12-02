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
    matrix              = 0x0B,
    table               = 0x0C,

    invlid_type         = 0xFF
  };

  enum class VarTypeClass : byte {
    primitive           = 0x01,
    buffer_array        = 0x02,
    array               = 0x03,
    object              = 0x04,
    matrix              = 0x05,
    table               = 0x06,

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
      case VarType::matrix:
        return VarTypeClass::matrix;
      case VarType::table:
        return VarTypeClass::table;
      default:
        return VarTypeClass::invalid_type;
    }
  }

  class Variable;
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
