#ifndef BUFFER_ARRAY_H
#define BUFFER_ARRAY_H

#include "../types.h"
#include "value_ptr.h"

namespace binom {
class BufferArray {
  union types {
      void* ptr;
      VarType* type;
      byte* bytes;
      types(void* ptr) : ptr(ptr) {}
  } data;

  ui64 msize() const {
    switch (*data.type) {
      case VarType::byte: return 9 + getMemberCount();
      case VarType::word: return 9 + getMemberCount()*2;
      case VarType::dword: return 9 + getMemberCount()*4;
      case VarType::qword: return 9 + getMemberCount()*8;
      default: throw SException(ErrCode::binom_invalid_type);
    }
  }

  void destroy() {free(data.ptr);data.ptr = nullptr;}

  friend class Variable;
    
public:

  typedef ValueIterator iterator;
  typedef const ValueIterator const_iterator;

  BufferArray(const char* str);
  BufferArray(const std::string str);

  BufferArray(size_t size, ui8 value);
  BufferArray(size_t size, ui16 value);
  BufferArray(size_t size, ui32 value);
  BufferArray(size_t size, ui64 value);
  BufferArray(size_t size, i8 value);
  BufferArray(size_t size, i16 value);
  BufferArray(size_t size, i32 value);
  BufferArray(size_t size, i64 value);

  BufferArray(ui8* values, size_t size);
  BufferArray(ui16* values, size_t size);
  BufferArray(ui32* values, size_t size);
  BufferArray(ui64* values, size_t size);
  BufferArray(i8* values, size_t size);
  BufferArray(i16* values, size_t size);
  BufferArray(i32* values, size_t size);
  BufferArray(i64* values, size_t size);

  BufferArray(ui8arr array);
  BufferArray(ui16arr array);
  BufferArray(ui32arr array);
  BufferArray(ui64arr array);

  BufferArray(i8arr array);
  BufferArray(i16arr array);
  BufferArray(i32arr array);
  BufferArray(i64arr array);

  ~BufferArray() {destroy();}


  inline ui64 getMemberCount() const {return *reinterpret_cast<ui64*>(data.bytes + 1);}

  inline ValuePtr getValue(ui64 index) const {
    if(index >= getMemberCount()) throw SException(ErrCode::binom_out_of_range, "Out of buffer array range!");
    return begin()[index];
  }

  inline ValuePtr operator[](ui64 index) const {return getValue(index);}

  inline ValueIterator begin() const {return ValueIterator(*data.type, data.bytes + 9);}
  inline ValueIterator end() const {return ValueIterator(*data.type, data.bytes + msize());}
  inline const ValueIterator cbegin() const {return ValueIterator(*data.type, data.bytes + 9);}
  inline const ValueIterator cend() const {return ValueIterator(*data.type, data.bytes + msize());}

};
}

#endif
