#ifndef BUFFER_ARRAY_H
#define BUFFER_ARRAY_H

#include "../types.h"

namespace binom {
class BufferArray {
  byte* ptr = nullptr;
  friend class Variable;
    
public:
  BufferArray(const char* str);

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

  template<size_t array_size>
  BufferArray(const ui8 (&array)[array_size]);
  template<size_t array_size>
  BufferArray(const ui16 (&array)[array_size]);
  template<size_t array_size>
  BufferArray(const ui32 (&array)[array_size]);
  template<size_t array_size>
  BufferArray(const ui64 (&array)[array_size]);
  template<size_t array_size>
  BufferArray(const i8 (&array)[array_size]);
  template<size_t array_size>
  BufferArray(const i16 (&array)[array_size]);
  template<size_t array_size>
  BufferArray(const i32 (&array)[array_size]);
  template<size_t array_size>
  BufferArray(const i64 (&array)[array_size]);
  template<size_t array_size>
  BufferArray(const f32 (&array)[array_size]);
  template<size_t array_size>
  BufferArray(const f64 (&array)[array_size]);
};
}

#endif
