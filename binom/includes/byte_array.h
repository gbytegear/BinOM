#ifndef BYTE_ARRAY_H
#define BYTE_ARRAY_H

#include "ctypes.h"
#include "mem.h"
#include <cstring>

namespace binom {

class ByteArray {
  ui64 _length = 0;
  byte* array = nullptr;

  friend class SharedByteArray;

public:

  typedef byte* iterator;
  typedef const byte* const_iterator;

  ByteArray() = default;
  ByteArray(const void* buffer, ui64 size);
  ByteArray(const ByteArray& other);
  ByteArray(ByteArray&& other);
  ByteArray(ui64 size);
  ByteArray(std::initializer_list<const ByteArray> arrays);

  ~ByteArray();

  bool isEmpty() const;

  ui64 length() const;

  template<typename Type>
  ui64 length() const {return _length/sizeof(Type);}

  byte& operator[](ui64 index);

  ByteArray& pushBack(byte b);
  ByteArray& pushFront(byte b);
  ByteArray& pushBack(const void* buffer, ui64 size);
  ByteArray& pushFront(const void* buffer, ui64 size);

  template<typename Type>
  ByteArray& pushBack(const Type& value) {return pushBack(&value, sizeof (Type));}

  template<typename Type>
  ByteArray& pushFront(const Type& value) {return pushFront(&value, sizeof (Type));}

  ByteArray& pushBack(const char* c_str);
  ByteArray& pushFront(const char* c_str);

  ByteArray& pushBack(const ByteArray& byte_array);
  ByteArray& pushFront(const ByteArray& byte_array);

  ByteArray& pushBack(const ByteArray&& byte_array);
  ByteArray& pushFront(const ByteArray&& byte_array);


  ByteArray takeBack(ui64 size);


  bool isEqual(const ByteArray& other);

  ByteArray takeFront(ui64 size);

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

  ByteArray& operator+=(byte b);
  ByteArray& operator+=(const ByteArray& byte_array);
  ByteArray& operator+=(const ByteArray&& byte_array);
  ByteArray& operator+=(const char* c_str);
  template<typename Type>
  ByteArray& operator+=(const Type& data) {return pushBack(data);}

  byte& get(ui64 index);
  template<typename Type>
  Type& get(ui64 index, ui64 shift = 0) {return reinterpret_cast<Type*>(array + shift)[index];}

  byte& set(ui64 index, byte value);

  template<typename Type>
  Type& set(ui64 index, ui64 shift, Type value) {
    return get<Type>(index, shift) = value;
  }
  iterator set(ui64 index, ByteArray data);

  byte& first();
  byte& last();

  template<typename Type>
  Type& first() {return *reinterpret_cast<Type*>(array);}
  template<typename Type>
  Type& last() {return *reinterpret_cast<Type*>(array + _length - sizeof (Type));}

  iterator begin() const;
  iterator end() const;

  template<typename Type>
  Type* begin(ui64 shift = 0) const {return reinterpret_cast<Type*>(array + shift);}
  template<typename Type>
  Type* end() const {return reinterpret_cast<Type*>(array) + length<Type>();}

  const_iterator cbegin() const;
  const_iterator cend() const;

  void* unfree();
};


}


#endif // BYTE_ARRAY_H
