#ifndef BYTEARRAY_H
#define BYTEARRAY_H

// Primitive Type
#include "byte.h"

// Memory Work
#include <malloc.h>
#include <new>
#include <cstring>

#include <cassert>
#include <tgmath.h>

#include <initializer_list>







////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<ui64 count>
class SByteArray {
public:

  typedef Byte* iterator;
  typedef const Byte* const_iterator;

#define ITERATOR_CLASS
#include "template_iterator.h"

private:
  Byte data_ptr[count];

  friend class ByteArray;
public:
  SByteArray(Byte value = 0) {for(auto& byte : (*this)) byte = value;}

  template<ui64 other_count>
  SByteArray(SByteArray<other_count>& other) {
    iterator other_it = other.begin();
    for(Byte& byte : (*this)) {
      new(&byte) Byte(*other_it);
      ++other_it;
    }
  }

  template<ui64 other_count>
  SByteArray(SByteArray<other_count>&& other) {
    iterator other_it = other.begin();
    for(Byte& byte : (*this)) {
      new(&byte) Byte(*other_it);
      ++other_it;
    }
  }

  SByteArray(std::initializer_list<Byte> bytes) {
    auto bytes_it = bytes.begin();
    for(iterator it = begin(); it != cend(); (++it)) {
      if(bytes_it != bytes.end()) {
        new(it) Byte(*bytes_it);
        ++bytes_it;
      }else new(it) Byte(0);
    }
  }

  template<ui64 arr_size>
  SByteArray(const Byte (&arr)[arr_size]) {
    const_iterator arr_it = arr;
    for(Byte& byte : (*this)) {
      new(&byte) Byte(*arr_it);
      ++arr_it;
    }
  }

  template<ui64 arr_size>
  SByteArray(const Byte (&&arr)[arr_size]) {
    iterator arr_it = arr;
    for(Byte& byte : (*this)) {
      new(&byte) Byte(*arr_it);
      ++arr_it;
    }
  }

  template<ui64 arr_size>
  SByteArray(const char (&charr)[arr_size]) {
    const char* arr_it = charr;
    for(Byte& byte : (*this)) {
      new(&byte) Byte(*arr_it);
      ++arr_it;
    }
  }

  template<ui64 arr_size>
  SByteArray(const wchar_t (&charr)[arr_size]) {
    const char* arr_it = reinterpret_cast<const char*>(charr);
    for(Byte& byte : (*this)) {
      new(&byte) Byte(*arr_it);
      ++arr_it;
    }
  }


  template<ui64 arr_size>
  static SByteArray make(const Byte (&&arr)[arr_size]) {
    return SByteArray<arr_size>(arr);
  }

  constexpr ui64 size() {return count;}

  Byte& operator[](ui64 index) {
    assert(index < count);
    return data_ptr[index];
  }

  template<typename Type>
  Type getAs(ui64 index, ui64 shift = 0) {
    assert(index * sizeof (Type) + shift < count);
    return reinterpret_cast<Type*>(data_ptr + shift)[index];
  }

  constexpr bool isEmpty() const noexcept {return count == 0;}

  // Iterators

  iterator begin() {return data_ptr;}
  iterator end() {return data_ptr + count;}

  const_iterator cbegin() {return data_ptr;}
  const_iterator cend() {return data_ptr + count;}

#define ITERATOR_FUNCS
#include "template_iterator.h"

  const char* toCharArray() {return reinterpret_cast<char*>(data_ptr);}
  const wchar_t* toWCharArray() {return reinterpret_cast<wchar_t*>(data_ptr);}
};

template<ui64 other_count>
SByteArray(SByteArray<other_count>& other) -> SByteArray<other_count>;

template<ui64 other_count>
SByteArray(SByteArray<other_count>&& other) -> SByteArray<other_count>;

template<ui64 arr_size>
SByteArray(const Byte (&arr)[arr_size]) -> SByteArray<arr_size>;

template<ui64 arr_size>
SByteArray(const Byte (&&arr)[arr_size]) -> SByteArray<arr_size>;

template<ui64 arr_size>
SByteArray(const char (&charr)[arr_size]) -> SByteArray<arr_size - 1>;

template<ui64 arr_size>
SByteArray(const wchar_t (&charr)[arr_size]) -> SByteArray<(arr_size - 1) * sizeof (wchar_t)>;













////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ByteArray {
public:

  typedef Byte* iterator;
  typedef const Byte* const_iterator;

#define ITERATOR_CLASS
#include "template_iterator.h"

private:
  ui64 count = 0;
  Byte* data_ptr = nullptr;

public:
  ByteArray() = default;
  ByteArray(ui64 size);
  ByteArray(ui64 size, const Byte value);
  ByteArray(ByteArray& other);
  ByteArray(ByteArray&& tmp);

  template<ui64 arr_size>
  ByteArray(const char (&arr)[arr_size]) : count(arr_size - 1), data_ptr(new Byte[arr_size - 1]()) {
    const char* it = arr;
    for(Byte& byte : (*this)) {
      new(&byte) Byte(*it);
      ++it;
    }
  }

  template<ui64 arr_size>
  ByteArray(const wchar_t (&arr)[arr_size]) : count((arr_size - 1) * sizeof (wchar_t)), data_ptr(new Byte[(arr_size - 1) * sizeof (wchar_t)]) {
    const char* it = reinterpret_cast<const char*>(arr);
    for(Byte& byte : (*this)) {
      new(&byte) Byte(*it);
      ++it;
    }
  }

  template<ui64 count>
  ByteArray(SByteArray<count>& other) : count(count), data_ptr(new Byte[count]) {memcpy(data_ptr, other.data_ptr, count);}
  template<ui64 count>
  ByteArray(SByteArray<count>&& other) : count(count), data_ptr(new Byte[count]) {memcpy(data_ptr, other.data_ptr, count);}

  ByteArray(std::initializer_list<Byte> byte_list);

  ~ByteArray();

  ui64 size() {return count;}

  Byte& operator[](ui64 index);

  template<typename Type>
  Type getAs(ui64 index, ui64 shift = 0) {
    assert(index * sizeof (Type) + shift < count);
    return reinterpret_cast<Type*>(data_ptr + shift)[index];
  }

  bool isEmpty() const noexcept;

  ByteArray& pushBack(const Byte byte) noexcept;
  ByteArray& pushBack(std::initializer_list<const Byte> bytes) noexcept;
  ByteArray& pushBack(ByteArray& other) noexcept;
  ByteArray& pushBack(ByteArray&& tmp) noexcept;

  template<ui64 size>
  ByteArray& pushBack(SByteArray<size>& other) {
    if(isEmpty()) {
      return (*this) = other;
    } else {
      data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count + other.size()));
      memcpy(data_ptr + count, other.data_ptr, other.size());
      count += other.size();
    }
    return *this;
  }

  template<ui64 size>
  ByteArray& pushBack(SByteArray<size>&& tmp) {
    if(isEmpty()) {
      return (*this) = tmp;
    } else {
      data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count + tmp.size()));
      memcpy(data_ptr + count, tmp.data_ptr, tmp.size());
      count += tmp.size();
    }
    return *this;
  }

  ByteArray& pushFront(const Byte byte) noexcept;
  ByteArray& pushFront(std::initializer_list<const Byte> bytes) noexcept;
  ByteArray& pushFront(ByteArray& other) noexcept;
  ByteArray& pushFront(ByteArray&& tmp) noexcept;

  ByteArray& insert(ui64 index, const Byte byte) noexcept;

  ByteArray& popBack(ui64 remove_count = 1);
  ByteArray& popFront(ui64 remove_count = 1);
  ByteArray& clear() noexcept;

  ByteArray& operator=(ByteArray& other);
  ByteArray& operator=(ByteArray&& tmp);

  template<ui64 size>
  ByteArray& operator=(SByteArray<size>& other) {
    count = other.size();
    data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count));
    memcpy(data_ptr, other.data_ptr, count);
    return *this;
  }

  template<ui64 size>
  ByteArray& operator=(SByteArray<size>&& tmp) {
    count = tmp.size();
    data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count));
    memcpy(data_ptr, tmp.data_ptr, count);
    return *this;
  }


  inline ByteArray& operator+=(Byte value) noexcept {return pushBack(value);}
  inline ByteArray& operator+=(std::initializer_list<const Byte> bytes) noexcept {return pushBack(bytes);}

  inline ByteArray& operator-=(ui64 count) noexcept {return popBack(count);}

  inline ByteArray& operator--() noexcept {return popFront();}
  inline ByteArray& operator--(int) noexcept {return popBack();}

  inline ByteArray operator+(Byte value) noexcept {return ByteArray(*this).pushBack(value);}
  inline ByteArray operator+(std::initializer_list<const Byte> bytes) noexcept {return ByteArray(*this).pushBack(bytes);}

  inline ByteArray operator-(ui64 count) noexcept {return ByteArray(*this).popBack(count);}

  // Iterators

  iterator begin() noexcept;
  iterator end() noexcept;

  const_iterator cbegin() noexcept;
  const_iterator cend() noexcept;

#define ITERATOR_FUNCS
#include "template_iterator.h"

  const char* toCharArray() {return reinterpret_cast<char*>(data_ptr);}
  const wchar_t* toWCharArray() {return reinterpret_cast<wchar_t*>(data_ptr);}

};

inline ByteArray& operator-=(int count, ByteArray& bytes) noexcept {return bytes.popFront(count);}
inline ByteArray& operator+=(Byte value, ByteArray& bytes) noexcept {return bytes.pushFront(value);}
inline ByteArray& operator+=(ByteArray&& byte_list, ByteArray& bytes) noexcept {return bytes.pushFront(byte_list);}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef STDCPPOUTPUT


std::ostream& operator << (std::ostream& os, ByteArray& bytes) {
  for(Byte& byte : bytes)
    os << byte << ' ';
  return os;
}

template<ui64 size>
std::ostream& operator << (std::ostream& os, SByteArray<size>& bytes) {
  for(Byte& byte : bytes)
    os << byte << ' ';
  return os;
}


#endif

#endif // BYTEARRAY_H
