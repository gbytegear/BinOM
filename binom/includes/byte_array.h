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
  ByteArray(const void* buffer, ui64 size) : _length(size), array(tryMalloc<byte>(size)) {memcpy(array, buffer, size);}
  ByteArray(const ByteArray& other) : _length(other._length), array(tryMalloc<byte>(_length)) {memcpy(array, other.begin(), _length);}
  ByteArray(const ByteArray&& other) : _length(other._length), array(other.array) {}
  ByteArray(ui64 size) : _length(size), array(tryMalloc<byte>(size)) {}
  ByteArray(std::initializer_list<const ByteArray> arrays) {
    for(const ByteArray& byte_array : arrays) {
      _length += byte_array._length;
    }
    array = tryMalloc<byte>(_length);
    iterator it = begin();
    for(const ByteArray& byte_array : arrays) {
      memcpy(it, byte_array.begin(), byte_array._length);
      it += byte_array._length;
    }
  }

  ui64 length() const {return _length;}

  template<typename Type>
  ui64 length() const {return _length/sizeof(Type);}

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

  template<typename Type>
  ByteArray& pushBack(const Type& value) {return pushBack(&value, sizeof (Type));}

  template<typename Type>
  ByteArray& pushFront(const Type& value) {return pushFront(&value, sizeof (Type));}

  ByteArray& pushBack(const char* c_str) {return pushBack (c_str, strlen (c_str) + 1);}
  ByteArray& pushFront(const char* c_str) {return pushFront(c_str, strlen (c_str) + 1);}

  ByteArray& pushBack(const ByteArray& byte_array) {return pushBack (byte_array.array, byte_array._length);}
  ByteArray& pushFront(const ByteArray& byte_array) {return pushFront (byte_array.array, byte_array._length);}

  ByteArray& pushBack(const ByteArray&& byte_array) {return pushBack (byte_array.array, byte_array._length);}
  ByteArray& pushFront(const ByteArray&& byte_array) {return pushFront (byte_array.array, byte_array._length);}


  ByteArray takeBack(ui64 size) {
    if(size > _length) throw SException(ErrCode::any);
    ByteArray _new(size);
    memcpy(_new.begin(), end()-size, size);
    _length -= size;
    array = tryRealloc<byte>(array, _length);
    return _new;
  }


  bool isEqual(const ByteArray& other) {
    if(length() != other.length()) return false;
    iterator o_it = other.begin();
    for(ui8 it : *this) {
      if(it != *o_it)
        return false;
      ++o_it;
    }
    return true;
  }

  ByteArray takeFront(ui64 size) {
    if(size > _length) throw SException(ErrCode::any);
    ByteArray _new(size);
    memcpy(_new.begin(), begin(), size);
    _length -= size;
    memmove(begin(), begin() + size, _length);
    array = tryRealloc<byte>(array, _length);
    return _new;
  }

  template<typename Type>
  Type takeBack() {
    if(sizeof (Type) > _length) throw SException(ErrCode::any);
    Type _new;
    memcpy(&_new, end()-sizeof(Type), sizeof(Type));
    _length -= sizeof(Type);
    return _new;
  }

  template<typename Type>
  Type takeFront() {
    if(sizeof (Type) > _length) throw SException(ErrCode::any);
    Type _new;
    memcpy(&_new, begin(), sizeof (Type));
    _length -= sizeof (Type);
    memmove(begin(), begin() + sizeof (Type), _length);
    array = tryRealloc<byte>(array, _length);
    return _new;
  }

  ByteArray& operator+=(byte b) {return pushBack (b);}
  ByteArray& operator+=(const ByteArray& byte_array) {return pushBack (byte_array);}
  ByteArray& operator+=(const ByteArray&& byte_array) {return pushBack (byte_array);}
  ByteArray& operator+=(const char* c_str) {return pushBack (c_str);}
  template<typename Type>
  ByteArray& operator+=(const Type& data) {return pushBack(data);}

  byte& get(ui64 index) {return array[index];}
  template<typename Type>
  Type& get(ui64 index, ui64 shift = 0) {return reinterpret_cast<Type*>(array + shift)[index];}

  byte& first() {return *array;}
  byte& last() {return array[_length - 1];}

  template<typename Type>
  Type& first() {return *reinterpret_cast<Type*>(array);}
  template<typename Type>
  Type& last() {return *reinterpret_cast<Type*>(array + _length - sizeof (Type));}

  iterator begin() const {return array;}
  iterator end() const {return array + _length;}

  template<typename Type>
  Type* begin() const {return reinterpret_cast<Type*>(array);}
  template<typename Type>
  Type* end() const {return reinterpret_cast<Type*>(array) + length<Type>();}

  const_iterator cbegin() const {return array;}
  const_iterator cend() const {return array + _length;}

};

}


#endif // BYTE_ARRAY_H
