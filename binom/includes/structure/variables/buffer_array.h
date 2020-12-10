#ifndef BUFFER_ARRAY_H
#define BUFFER_ARRAY_H

#include "../types.h"
#include "value.h"

namespace binom {
class BufferArray {
  union types {
      void* ptr;
      VarType* type;
      byte* bytes;
      types(void* ptr) : ptr(ptr) {}
  } data;

  inline ui64& length() const {return *reinterpret_cast<ui64*>(data.bytes + 1);}

  ui64 msize() const;


  void mch(size_t new_size);
  void* madd(size_t add_size);
  void msub(size_t sub_size);
  void* maddto(void* to, size_t size);
  void destroy();
  void* clone() const;

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

  BufferArray(BufferArray& other);
  BufferArray(BufferArray&& other);

  ~BufferArray() {destroy();}

  inline ui64 getMemberCount() const {return *reinterpret_cast<ui64*>(data.bytes + 1);}
  inline ui8 getMemberSize() const {
    switch (*data.type) {
      case VarType::byte_array: return 1;
      case VarType::word_array: return 2;
      case VarType::dword_array: return 4;
      case VarType::qword_array: return 8;
      default: throw SException(ErrCode::binom_invalid_type);
    }
  }

  inline Value getValue(ui64 index) const {
    if(index >= getMemberCount()) throw SException(ErrCode::binom_out_of_range, "Out of buffer array range!");
    return begin()[index];
  }

  Value pushBack(const ui64 value);
  Value pushBack(const i64 value);
  Value pushBack(const f64 value);
  iterator pushBack(const BufferArray& other);
  Value pushFront(const ui64 value);
  Value pushFront(const i64 value);
  Value pushFront(const f64 value);
  iterator pushFront(const BufferArray& other);
  Value insert(const ui64 index, const ui64 value);
  Value insert(const ui64 index, const i64 value);
  Value insert(const ui64 index, const f64 value);
  iterator insert(const ui64 index, const BufferArray& other);

  inline Value operator[](ui64 index) const {return getValue(index);}
  inline BufferArray& operator+=(const ui64 value) {pushBack(value); return *this;}
  inline BufferArray& operator+=(const i64 value) {pushBack(value); return *this;}
  inline BufferArray& operator+=(const f64 value) {pushBack(value); return *this;}
  inline BufferArray& operator+=(const BufferArray& other) {pushBack(other); return *this;}

  BufferArray& operator=(const BufferArray& other);

  ValueIterator begin() const;
  ValueIterator end() const;
  const ValueIterator cbegin() const;
  const ValueIterator cend() const;

  std::string toString();
};



}

std::ostream& operator<<(std::ostream& os, binom::BufferArray& buffer);

#endif
