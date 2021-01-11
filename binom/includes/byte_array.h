#ifndef BYTE_ARRAY_H
#define BYTE_ARRAY_H

#include "ctypes.h"
#include "mem.h"
#include <cstring>

namespace binom {

class ByteArray {
  ui64 _length = 0;
  byte* array = nullptr;
public:

  typedef byte* iterator;
  typedef const byte* const_iterator;

  ByteArray() = default;
  ByteArray(void* buffer, ui64 size) : _length(size), array(tryMalloc<byte>(size)) {memcpy(array, buffer, size);}
  ByteArray(ui64 size) : _length(size), array(tryMalloc<byte>(size)) {}

  ui64 length() {return _length;}

  byte& operator[](ui64 index) {
    if(index >= _length) throw SException(ErrCode::out_of_range);
    return array[index];
  }

  ByteArray& pushBack(byte b) {
    array = tryRealloc<byte>(array, ++_length);
    array[_length - 1] = b;
    return *this;
  }

  ByteArray& pushFront(byte b) {
    array = tryRealloc<byte>(array, ++_length);
    memmove (array + 1, array, _length - 1);
    *array = b;
    return *this;
  }

  ByteArray& pushBack(const void* buffer, ui64 size) {
    ui64 last_length = _length;
    array = tryRealloc<byte>(array, _length += size);
    memcpy (array + last_length, buffer, size);
    return *this;
  }

  ByteArray& pushFront(const void* buffer, ui64 size) {
    array = tryRealloc<byte>(array, _length += size);
    memmove (array + size, array, _length - size);
    memcpy (array, buffer, size);
    return *this;
  }

  ByteArray& pushBack(const char* c_str) {return pushBack (c_str, strlen (c_str) + 1);}
  ByteArray& pushFront(const char* c_str) {return pushFront(c_str, strlen (c_str) + 1);}

  ByteArray& pushBack(const ByteArray& byte_array) {return pushBack (byte_array.array, byte_array._length);}
  ByteArray& pushFront(const ByteArray& byte_array) {return pushFront (byte_array.array, byte_array._length);}

  ByteArray& pushBack(const ByteArray&& byte_array) {return pushBack (byte_array.array, byte_array._length);}
  ByteArray& pushFront(const ByteArray&& byte_array) {return pushFront (byte_array.array, byte_array._length);}

  ByteArray& operator+=(byte b) {return pushBack (b);}
  ByteArray& operator+=(ByteArray& byte_array) {return pushBack (byte_array);}
  ByteArray& operator+=(ByteArray&& byte_array) {return pushBack (byte_array);}
  ByteArray& operator+=(const char* c_str) {return pushBack (c_str);}

  iterator begin() const {return array;}
  iterator end() const {return array + _length;}

  const_iterator cbegin() const {return array;}
  const_iterator cend() const {return array + _length;}

};

}


#endif // BYTE_ARRAY_H
