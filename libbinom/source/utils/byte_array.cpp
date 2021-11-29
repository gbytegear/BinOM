#include "libbinom/include/utils/byte_array.h"

using namespace binom;

ByteArray::ByteArray(const void* buffer, ui64 size) : _length(size), array(tryMalloc<byte>(size)) {memcpy(array, buffer, size);}

ByteArray::ByteArray(const ByteArray& other) : _length(other._length), array(tryMalloc<byte>(_length)) {memcpy(array, other.begin(), _length);}

ByteArray::ByteArray(ByteArray&& other) : _length(other._length), array(other.array) {other.array = nullptr;}

ByteArray::ByteArray(ui64 size) : _length(size), array(tryMalloc<byte>(size)) {}

ByteArray::ByteArray(std::initializer_list<const ByteArray> arrays) {
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

ByteArray::~ByteArray() {
  if(!array) return;
  free(array);
  array = nullptr;
  _length = 0;
}

bool ByteArray::isEmpty() const {return !_length;}

ui64 ByteArray::length() const {return _length;}

byte& ByteArray::operator[](ui64 index) {
  if(index >= _length) throw Exception(ErrCode::out_of_range);
  return array[index];
}


ByteArray& ByteArray::pushBack(byte b) {
  array = tryRealloc<byte>(array, ++_length);
  array[_length - 1] = b;
  return *this;
}

ByteArray& ByteArray::pushFront(byte b) {
  array = tryRealloc<byte>(array, ++_length);
  memmove (array + 1, array, _length - 1);
  *array = b;
  return *this;
}

ByteArray& ByteArray::pushBack(const void* buffer, ui64 size) {
  ui64 last_length = _length;
  array = tryRealloc<byte>(array, _length += size);
  memcpy (array + last_length, buffer, size);
  return *this;
}

ByteArray& ByteArray::pushFront(const void* buffer, ui64 size) {
  array = tryRealloc<byte>(array, _length += size);
  memmove (array + size, array, _length - size);
  memcpy (array, buffer, size);
  return *this;
}

ByteArray& ByteArray::pushBack(const char* c_str) {return pushBack (c_str, strlen (c_str) + 1);}

ByteArray& ByteArray::pushFront(const char* c_str) {return pushFront(c_str, strlen (c_str) + 1);}

ByteArray& ByteArray::pushBack(const ByteArray& byte_array) {return pushBack (byte_array.array, byte_array._length);}

ByteArray& ByteArray::pushFront(const ByteArray& byte_array) {return pushFront (byte_array.array, byte_array._length);}

ByteArray& ByteArray::pushBack(const ByteArray&& byte_array) {return pushBack (byte_array.array, byte_array._length);}

ByteArray& ByteArray::pushFront(const ByteArray&& byte_array) {return pushFront (byte_array.array, byte_array._length);}

ByteArray& ByteArray::insert(ui64 index, byte b) {
  if(index > _length) throw Exception(ErrCode::out_of_range);
  array = tryRealloc<byte>(array, ++_length);
  memmove(array + index + 1, array + index, _length - index - 1);
  array[index] = b;
  return *this;
}

ByteArray& ByteArray::insert(ui64 index, const void* buffer, ui64 size) {
  if(index > _length) throw Exception(ErrCode::out_of_range);
  array = tryRealloc<byte>(array, _length += size);
  memmove(array + index + size, array + index, _length - index - size);
  memcpy(array + index, buffer, size);
  return *this;
}

ByteArray& ByteArray::insert(ui64 index, const ByteArray& byte_array) { return insert(index, byte_array.array, byte_array._length); }
ByteArray& ByteArray::insert(ui64 index, const ByteArray&& byte_array) { return insert(index, byte_array.array, byte_array._length); }

ByteArray& ByteArray::remove(ui64 index, ui64 size) {
  if(!index && size == _length) {
    _length = 0;
    free(array);
    array = nullptr;
    return *this;
  }
  if(index + size > _length) throw Exception(ErrCode::out_of_range);
  memmove(array + index, array + index + size, (_length -= size) - index);
  array = tryRealloc<byte>(array, _length);
  return *this;
}

ByteArray ByteArray::takeBack(ui64 size) {
  if(size > _length) throw Exception(ErrCode::any);
  ByteArray _new(size);
  memcpy(_new.begin(), end()-size, size);
  _length -= size;
  array = tryRealloc<byte>(array, _length);
  return _new;
}

bool ByteArray::isEqual(const ByteArray& other) {
  if(length() != other.length()) return false;
  iterator o_it = other.begin();
  for(ui8 it : *this) {
    if(it != *o_it)
      return false;
    ++o_it;
  }
  return true;
}

void ByteArray::reset(ui64 new_length) {
  _length = new_length;
  array = tryRealloc<byte>(array, _length);
  memset(array, 0, _length);
}

void ByteArray::resize(ui64 new_length) {
  _length = new_length;
  array = tryRealloc<byte>(array, _length);
}

ByteArray::iterator ByteArray::addSize(ui64 add) {
  ui64 old_length = _length;
  array = tryRealloc<byte>(array, _length += add);
  iterator it = array + old_length;
  memset(it, 0, add);
  return it;
}

ByteArray::iterator ByteArray::addSizeFront(ui64 add) {
  ui64 old_length = _length;
  addSize(add);
  memmove(array + add, array, old_length);
  memset(array, 0, add);
  return array;
}

ByteArray::iterator ByteArray::addSizeTo(ui64 to, ui64 add) {
  ui64 old_length = _length;
  array = tryRealloc<byte>(array, _length += add);
  iterator it = array + to;
  memmove(it + add, it, old_length - add);
  memset(it, 0, add);
  return it;
}

void ByteArray::subSize(ui64 sub) {
  array = tryRealloc<byte>(array, _length -= sub);
}

ByteArray ByteArray::takeFront(ui64 size) {
  if(size > _length) throw Exception(ErrCode::any);
  ByteArray _new(size);
  memcpy(_new.begin(), begin(), size);
  _length -= size;
  memmove(begin(), begin() + size, _length);
  array = tryRealloc<byte>(array, _length);
  return _new;
}

ByteArray ByteArray::takeFrom(ui64 index, ui64 size) {
  if(index + size > _length) throw Exception(ErrCode::any);
  ByteArray _new(size);
  memcpy(_new.begin(), begin() + index, size);
  _length -= size;
  memmove(begin() + index, begin() + index + size, _length - index);
  array = tryRealloc<byte>(array, _length);
  return _new;
}

ui64 ByteArray::pointerToIndex(void* pos) {
  if(pos < array || pos > array + _length)
    throw Exception(ErrCode::out_of_range);
  return ui64(reinterpret_cast<byte*>(pos) - array);
}

ByteArray& ByteArray::operator+=(byte b) {return pushBack(b);}
ByteArray& ByteArray::operator+=(ByteArray byte_array) {return pushBack (std::move(byte_array));}
ByteArray& ByteArray::operator+=(const char* c_str) {return pushBack(c_str);}

ByteArray ByteArray::operator+(byte b) {return ByteArray(*this).pushBack(b);}
ByteArray ByteArray::operator+(ByteArray byte_array) {return ByteArray(*this).pushBack (std::move(byte_array));}
ByteArray ByteArray::operator+(const char* c_str) {return ByteArray(*this).pushBack(c_str);}

byte& ByteArray::get(ui64 index) {return array[index];}

byte& ByteArray::set(ui64 index, byte value) {return get(index) = value;}

ByteArray::iterator ByteArray::set(ui64 index, ByteArray data) {
  if(ui64 needed_length = index + _length; needed_length < data._length)
    array = tryRealloc<byte>(array, _length = needed_length);
  iterator ret = begin() + index;
  for(iterator this_it = ret, other_it = data.begin();
      other_it != data.end();
      (++this_it, ++other_it))
    *this_it = *other_it;
  return ret;
}

byte& ByteArray::first() {return *array;}

byte& ByteArray::last() {return array[_length - 1];}

ByteArray::iterator ByteArray::begin() const {return array;}

ByteArray::iterator ByteArray::end() const {return array + _length;}

ByteArray::const_iterator ByteArray::cbegin() const {return array;}

ByteArray::const_iterator ByteArray::cend() const {return array + _length;}

void* ByteArray::unfree() {
  void* ptr = array;
  array = nullptr;
  _length = 0;
  return ptr;
}

void ByteArray::split(ui64 second_start, ByteArray& first, ByteArray& second) {
  first.reset(second_start);
  memcpy(first.array, array, second_start);
  second.reset(_length - second_start);
  memcpy(second.array, array + second_start, second._length);
}

std::string ByteArray::toStdString() {return std::string(reinterpret_cast<char*>(array), _length);}

ByteArray& ByteArray::operator=(ByteArray other) {
  if(array) free(array);
  _length = other._length;
  array = other.array;
  other.array = nullptr;
  return *this;
}
