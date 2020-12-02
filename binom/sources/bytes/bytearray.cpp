#include "bytearray.h"

ByteArray::ByteArray(ui64 size) : count(size), data_ptr(new Byte[size]) {}

ByteArray::ByteArray(ui64 size, const Byte value) : count(size), data_ptr(new Byte[size]) {
  for(auto& byte : (*this)) byte = value;
}

ByteArray::ByteArray(ByteArray& other) : count(other.count), data_ptr(new Byte[count]) {memcpy(data_ptr, other.data_ptr, count);}

ByteArray::ByteArray(ByteArray&& tmp) : count(tmp.count), data_ptr(tmp.data_ptr) {tmp.data_ptr = nullptr;}

ByteArray::ByteArray(std::initializer_list<Byte> byte_list) : count(byte_list.size()), data_ptr(new Byte[count]) {
  iterator ba_it = begin();
  std::initializer_list<Byte>::iterator l_it = byte_list.begin();
  for(; ba_it != cend(); (++ba_it, ++l_it) )
    new(ba_it) Byte(*l_it);
}

ByteArray::~ByteArray() {if(data_ptr)delete [] data_ptr;}

Byte& ByteArray::operator[](ui64 index) {
  assert(index < count);
  return data_ptr[index];
}

bool ByteArray::isEmpty() const noexcept {return data_ptr == nullptr || count == 0;}

ByteArray& ByteArray::pushBack(const Byte byte) noexcept {
  if(isEmpty()) {
    ++count;
    data_ptr = new Byte(byte);
  } else {
    ++count;
    data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count));
    new(data_ptr + count - 1) Byte(byte);
  }
  return *this;
}

ByteArray& ByteArray::pushBack(std::initializer_list<const Byte> bytes) noexcept {
  if(isEmpty()) {
    count = bytes.size();
    data_ptr = new Byte[count]();
    iterator it = data_ptr;
    for(const Byte &byte : bytes) {
      assert(it != cend());
      new(it) Byte(byte);
      ++it;
    }
  } else {
    data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count + bytes.size()));
    iterator it = data_ptr + count;
    count += bytes.size();
    for(const Byte &byte : bytes) {
      assert(it != cend());
      new(it) Byte(byte);
      ++it;
    }
  }

  return *this;
}

ByteArray& ByteArray::pushBack(ByteArray& other) noexcept {
  if(isEmpty()) {
    return (*this) = other;
  } else {
    data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count + other.count));
    memcpy(data_ptr + count, other.data_ptr, other.count);
    count += other.count;
  }
  return *this;
}

ByteArray& ByteArray::pushFront(const Byte byte) noexcept {
  if(isEmpty()) {
    ++count;
    data_ptr = new Byte(byte);
  } else {
    ++count;
    data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count));
    memmove(data_ptr + 1, data_ptr, count - 1);
    new(data_ptr) Byte(byte);
  }

  return *this;
}

ByteArray& ByteArray::pushFront(std::initializer_list<const Byte> bytes) noexcept {
  if(isEmpty()) {
    count = bytes.size();
    data_ptr = new Byte[count]();
    iterator it = data_ptr;
    for(const Byte &byte : bytes) {
      assert(it != cend());
      new(it) Byte(byte);
      ++it;
    }
  } else {
    data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count + bytes.size()));
    memmove(data_ptr + bytes.size(), data_ptr, count);
    iterator it = data_ptr;
    count += bytes.size();
    for(const Byte &byte : bytes) {
      assert(it != data_ptr + bytes.size());
      new(it) Byte(byte);
      ++it;
    }
  }

  return *this;
}

ByteArray& ByteArray::pushFront(ByteArray& other) noexcept {
  if(isEmpty()) {
    return (*this) = other;
  } else {
    data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count + other.count));
    memmove(data_ptr + other.count, data_ptr, count);
    memcpy(data_ptr, other.data_ptr, other.count);
    count += other.count;
  }
  return *this;
}

ByteArray& ByteArray::insert(ui64 index, const Byte byte) noexcept {
  if(isEmpty()) {
    ++count;
    data_ptr = new Byte(byte);
  } else if(index >= count) {
    ++count;
    data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count));
    new(data_ptr + count - 1) Byte(byte);
  } else {
    ++count;
    data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count));
    memmove(data_ptr + index + 1, data_ptr + index, count - 1);
    new(data_ptr + index) Byte(byte);
  }

  return *this;
}

ByteArray& ByteArray::popBack(ui64 remove_count) {
  if(!isEmpty()) {
    count -= remove_count;
    data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count));
  }
  return *this;
}

ByteArray& ByteArray::popFront(ui64 remove_count) {
  if(!isEmpty()) {
    count -= remove_count;
    memmove(data_ptr, data_ptr + remove_count, count);
    data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count));
  }
  return *this;
}

ByteArray& ByteArray::clear() noexcept {
  count = 0;
  if(data_ptr) {
    delete [] data_ptr;
    data_ptr = nullptr;
  }
  return *this;
}

ByteArray& ByteArray::operator=(ByteArray& other) {
  count = other.count;
  data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count));
  memcpy(data_ptr, other.data_ptr, count);
  return *this;
}

ByteArray& ByteArray::operator=(ByteArray&& tmp) {
  count = tmp.count;
  data_ptr = reinterpret_cast<Byte*>(realloc(data_ptr, count));
  memcpy(data_ptr, tmp.data_ptr, count);
  return *this;
}

ByteArray::iterator ByteArray::begin() noexcept {return data_ptr;}
ByteArray::iterator ByteArray::end() noexcept {return data_ptr + count;}

ByteArray::const_iterator ByteArray::cbegin() noexcept {return data_ptr;}
ByteArray::const_iterator ByteArray::cend() noexcept {return data_ptr + count;}

