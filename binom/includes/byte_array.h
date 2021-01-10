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

  byte& operator[](ui64 index) {
    if(index >= _length) throw SException(ErrCode::out_of_range);
    return array[index];
  }

  byte& pushBack(byte b) {
    ui64 last_length = _length;
    array = tryRealloc<byte>(array, _length + 1);
    return array[last_length] = b;
  }

  iterator begin() {return array;}
  iterator end() {return array + _length;}

  const_iterator cbegin() {return array;}
  const_iterator cend() {return array + _length;}

};

}


#endif // BYTE_ARRAY_H
