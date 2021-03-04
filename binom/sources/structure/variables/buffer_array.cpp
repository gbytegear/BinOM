#include "binom/includes/structure/variables/variable.h"
#include <cmath>

using namespace binom;


ui64 BufferArray::msize() const {
  switch (*data.type) {
    case VarType::byte_array: return 9 + length();
    case VarType::word_array: return 9 + length()*2;
    case VarType::dword_array: return 9 + length()*4;
    case VarType::qword_array: return 9 + length()*8;
    default: throw SException(ErrCode::binom_invalid_type);
  }
}

void BufferArray::mch(size_t new_size) {data.ptr = tryRealloc(data.ptr, new_size);}

void* BufferArray::madd(size_t add_size) {
  size_t shift = msize();
  mch(shift + add_size);
  return data.bytes + shift;
}

void BufferArray::msub(size_t sub_size) {
  mch(msize() - sub_size);
}

void* BufferArray::maddto(void* to, size_t size) {
  size_t pos = reinterpret_cast<byte*>(to) - data.bytes;
  size_t old_size = msize();
  madd(size);
  memmove(data.bytes + pos + size, data.bytes + pos, old_size - pos);
  return data.bytes + pos;
}

void BufferArray::msubfrom(void* from, size_t size) {
  if(from < data.ptr) throw SException(ErrCode::binom_out_of_range);
  size_t old_size = msize();
  memmove(from, reinterpret_cast<byte*>(from) + size,
          old_size - (reinterpret_cast<byte*>(from) - data.bytes) - size);
  mch(old_size - size);
}

void BufferArray::destroy() {
  if(!data.ptr) return;
  tryFree(data.ptr);
  data.ptr = nullptr;
}

void* BufferArray::clone() const {
  size_t size = msize();
  void* ptr = tryMalloc(size);
  memcpy(ptr, data.ptr, size);
  return ptr;
}

ByteArray BufferArray::toChainNumber(ui64 number) {
  auto shiftBits = [](ui8 prev, ui8 next, ui8 shift)->ui8{return (next << shift) + (prev >> (8 - shift));};
  auto get7Bit = [](ui8 val)->ui8 {return (val > 127)? val - 128 : val;};
  ui8* number_bytes = reinterpret_cast<ui8*>(&number);
  ByteArray bytes((number == 0) ? 1 : static_cast<ui64>(floor (log10 (number) / log10 (128))) + 1);
  for(ui8 i = 0; i < bytes.length(); ++i) {
    bytes[i] = get7Bit(shiftBits(number_bytes[i - 1], number_bytes[i], i));
    if(i == bytes.length() - 1)
      bytes[i] = (bytes[i] > 127) ? bytes[i] - 128 : bytes[i];
    else
      bytes[i] |= 128;
  }
  return bytes;
}

ui64 BufferArray::fromChainNumber(ByteArray::iterator& it) {
  auto get7Bit = [](ui8 val)->ui8 {return (val > 127)? val - 128 : val;};
  ui64 number = 0;
  ui8 i = 0;
  for(; *it > 127; (++i, ++it)) {
    number += static_cast<ui64>( get7Bit(*it) * pow(128,i));
  }
  number += static_cast<ui64>(get7Bit(*it) * pow(128,i));
  ++it;
  return number;
}

BufferArray::BufferArray(VarType type) : data(nullptr){
  if(type < VarType::byte_array || type > VarType::qword_array) throw SException(ErrCode::binom_invalid_type);
  data.ptr = tryMalloc (9);
  *data.type = type;
  length() = 0;
}

BufferArray::BufferArray(const char* str) : data(tryMalloc(9 + strlen(str))) {
  data.type[0] = VarType::byte_array;
  ui64 size = strlen(str);
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;
  memcpy(reinterpret_cast<char*>(data.bytes + 9), str, size);
}

BufferArray::BufferArray(const std::string str) : data(tryMalloc(9 + str.length())) {
  data.type[0] = VarType::byte_array;
  ui64 size = str.length();
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;
  memcpy(reinterpret_cast<char*>(data.bytes + 9), str.c_str(), size);
}

BufferArray::BufferArray(const wchar_t* wstr) : data(tryMalloc(9 + wcslen(wstr))) {

}




BufferArray::BufferArray(size_t size, ui8 value) : data(tryMalloc(9 + size)) {
    data.type[0] = VarType::byte_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;
    memset(data.bytes + 9, value, size);
}

BufferArray::BufferArray(size_t size, ui16 value) : data(tryMalloc(9 + size*2)) {
    data.type[0] = VarType::word_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;

    size_t i = 0;
    for(ui16* it = reinterpret_cast<ui16*>(data.bytes + 9); i < size; (++it, ++i) )
        *it = value;
}

BufferArray::BufferArray(size_t size, ui32 value) : data(tryMalloc(9 + size*4)) {
    data.type[0] = VarType::word_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;

    size_t i = 0;
    for(ui32* it = reinterpret_cast<ui32*>(data.bytes + 9); i < size; (++it, ++i) )
        *it = value;
}

BufferArray::BufferArray(size_t size, ui64 value) : data(tryMalloc(9 + size*8)) {
    data.type[0] = VarType::word_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;

    size_t i = 0;
    for(ui64* it = reinterpret_cast<ui64*>(data.bytes + 9); i < size; (++it, ++i) )
        *it = value;

}




BufferArray::BufferArray(size_t size, i8 value) : data(tryMalloc(9 + size)) {
    data.type[0] = VarType::byte_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;
    memset(data.bytes + 9, value, size);
}

BufferArray::BufferArray(size_t size, i16 value) : data(tryMalloc(9 + size*2)) {
    data.type[0] = VarType::word_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;

    size_t i = 0;
    for(i16* it = reinterpret_cast<i16*>(data.bytes + 9); i < size; (++it, ++i) )
        *it = value;

}

BufferArray::BufferArray(size_t size, i32 value) : data(tryMalloc(9 + size*4)) {
    data.type[0] = VarType::word_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;

    size_t i = 0;
    for(i32* it = reinterpret_cast<i32*>(data.bytes + 9); i < size; (++it, ++i) )
        *it = value;

}

BufferArray::BufferArray(size_t size, i64 value) : data(tryMalloc(9 + size*8)) {
    data.type[0] = VarType::word_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;

    size_t i = 0;
    for(i64* it = reinterpret_cast<i64*>(data.bytes + 9); i < size; (++it, ++i) )
        *it = value;

}




BufferArray::BufferArray(ui8* values, size_t size) : data(tryMalloc(9 + size)) {
    data.type[0] = VarType::byte_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;
    memcpy(data.bytes + 9, values, size);
}

BufferArray::BufferArray(ui16* values, size_t size) : data(tryMalloc(9 + size*2)) {
    data.type[0] = VarType::word_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;

    ui16* v_it = values;
    for(ui64* it = reinterpret_cast<ui64*>(data.bytes + 9); ui64(v_it - values) < size; (++it, ++v_it) )
        *it = *v_it;

}

BufferArray::BufferArray(ui32* values, size_t size) : data(tryMalloc(9 + size*4)) {
    data.type[0] = VarType::dword_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;

    ui32* v_it = values;
    for(ui32* it = reinterpret_cast<ui32*>(data.bytes + 9); ui64(v_it - values) < size; (++it, ++v_it) )
        *it = *v_it;

}

BufferArray::BufferArray(ui64* values, size_t size) : data(tryMalloc(9 + size*8)) {
    data.type[0] = VarType::qword_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;

    ui64* v_it = values;
    for(ui64* it = reinterpret_cast<ui64*>(data.bytes + 9); ui64(v_it - values) < size; (++it, ++v_it) )
        *it = *v_it;

}




BufferArray::BufferArray(i8* values, size_t size) : data(tryMalloc(9 + size)) {
    data.type[0] = VarType::byte_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;
    memcpy(data.bytes + 9, values, size);
}

BufferArray::BufferArray(i16* values, size_t size) : data(tryMalloc(9 + size*2)) {
    data.type[0] = VarType::word_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;

    i16* v_it = values;
    for(i64* it = reinterpret_cast<i64*>(data.bytes + 9); ui64(v_it - values) < size; (++it, ++v_it) )
        *it = *v_it;

}

BufferArray::BufferArray(i32* values, size_t size) : data(tryMalloc(9 + size*4)) {
    data.type[0] = VarType::dword_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;

    i32* v_it = values;
    for(i32* it = reinterpret_cast<i32*>(data.bytes + 9); ui64(v_it - values) < size; (++it, ++v_it) )
        *it = *v_it;

}

BufferArray::BufferArray(i64* values, size_t size) : data(tryMalloc(9 + size*8)) {
    data.type[0] = VarType::qword_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = size;

    i64* v_it = values;
    for(i64* it = reinterpret_cast<i64*>(data.bytes + 9); ui64(v_it - values) < size; (++it, ++v_it) )
        *it = *v_it;

}





BufferArray::BufferArray(ui8arr array) : data(tryMalloc(9 + array.size())) {
    data.type[0] = VarType::byte_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
    ui8* it = data.bytes + 9;
    for(ui8 value : array) {
        *it = value;
        ++it;
    }
}

BufferArray::BufferArray(ui16arr array) : data(tryMalloc(9 + array.size()*2)) {
    data.type[0] = VarType::word_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
    ui16* it = reinterpret_cast<ui16*>(data.bytes + 9);
    for(ui16 value : array) {
        *it = value;
        ++it;
    }
}

BufferArray::BufferArray(ui32arr array) : data(tryMalloc(9 + array.size()*4)) {
    data.type[0] = VarType::dword_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
    ui32* it = reinterpret_cast<ui32*>(data.bytes + 9);
    for(ui32 value : array) {
        *it = value;
        ++it;
    }
}

BufferArray::BufferArray(ui64arr array) : data(tryMalloc(9 + array.size()*8)) {
    data.type[0] = VarType::qword_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
    ui64* it = reinterpret_cast<ui64*>(data.bytes + 9);
    for(ui64 value : array) {
        *it = value;
        ++it;
    }
}





BufferArray::BufferArray(i8arr array) : data(tryMalloc(9 + array.size())) {
    data.type[0] = VarType::byte_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
    i16* it = reinterpret_cast<i16*>(data.bytes + 9);
    for(i8 value : array) {
        *it = value;
        ++it;
    }
}

BufferArray::BufferArray(i16arr array) : data(tryMalloc(9 + array.size()*2)) {
    data.type[0] = VarType::word_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
    i16* it = reinterpret_cast<i16*>(data.bytes + 9);
    for(i16 value : array) {
        *it = value;
        ++it;
    }
}

BufferArray::BufferArray(i32arr array) : data(tryMalloc(9 + array.size()*4)) {
    data.type[0] = VarType::dword_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
    i32* it = reinterpret_cast<i32*>(data.bytes + 9);
    for(i32 value : array) {
        *it = value;
        ++it;
    }
}

BufferArray::BufferArray(i64arr array) : data(tryMalloc(9 + array.size()*8)) {
    data.type[0] = VarType::qword_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
    i64* it = reinterpret_cast<i64*>(data.bytes + 9);
    for(i64 value : array) {
        *it = value;
        ++it;
    }
}

BufferArray::BufferArray(ByteArray arr) : data(tryMalloc (9 + arr.length())) {
  data.type[0] = VarType::byte_array;
  length() = arr.length();
  memcpy(data.bytes + 9, arr.begin(), length());
}

BufferArray::BufferArray(BufferArray& other) : data(other.clone()) {}
BufferArray::BufferArray(BufferArray&& other) : data(other.data.ptr) {other.data.ptr = nullptr;}

ByteArray BufferArray::serialize() const {
  ByteArray bytes(1);
  bytes[0] = byte(*data.type);
  bytes += toChainNumber (length ());
  bytes.pushBack(data.bytes + 9, msize() - 9);
  return bytes;
}

BufferArray BufferArray::deserialize(binom::ByteArray::iterator& it) {
  VarType type = VarType(*it);
  if(type < VarType::byte_array || type > VarType::qword_array) throw SException(ErrCode::binom_invalid_type);
  ++it;
  ui64 length = fromChainNumber(it);
  void* buffer = tryMalloc(9 + length*getMemberSize(type));
  BufferArray buffer_array(buffer);
  *buffer_array.data.type = type;
  buffer_array.length() = length;
  memcpy(buffer_array.data.bytes + 9, it, length*getMemberSize(type));
  it += length*getMemberSize(type);
  return buffer_array;
}

ValueRef BufferArray::pushBack(ui64 value) {
    ValueRef val(*data.type, madd(getMemberSize()));
    val.setUnsigned(value);
    ++length();
    return val;
}

ValueRef BufferArray::pushBack(i64 value) {
    ValueRef val(*data.type, madd(getMemberSize()));
    val.setSigned(value);
    ++length();
    return val;
}

ValueRef BufferArray::pushBack(f64 value) {
    ValueRef val(*data.type, madd(getMemberSize()));
    val.setFloat(value);
    ++length();
    return val;
}

BufferArray::iterator BufferArray::pushBack(const BufferArray& other) {
    ui64 shift = getMemberCount();
    ui8 member_size = getMemberSize();
    madd(other.getMemberCount() * member_size);
    iterator it(*data.type, data.bytes + 9 + shift*member_size), ret(it);
    for(ValueRef val : other) {
        *it << val;
        ++it;
    }
    length() += other.length();
    return ret;
}

ValueRef BufferArray::pushFront(ui64 value) {
    ValueRef val(*data.type, maddto(data.bytes + 9, getMemberSize()));
    val.setUnsigned(value);
    ++length();
    return val;
}

ValueRef BufferArray::pushFront(i64 value) {
    ValueRef val(*data.type, maddto(data.bytes + 9, getMemberSize()));
    val.setSigned(value);
    ++length();
    return val;
}

ValueRef BufferArray::pushFront(f64 value) {
    ValueRef val(*data.type, maddto(data.bytes + 9, getMemberSize()));
    val.setFloat(value);
    ++length();
    return val;
}

BufferArray::iterator BufferArray::pushFront(const BufferArray& other) {
    iterator it(*data.type, maddto(data.bytes + 9, other.getMemberCount() * getMemberSize()));
    for(ValueRef val : other) {
        *it << val;
        ++it;
    }
    length() += other.length();
    return begin();
}

ValueRef BufferArray::insert(const ui64 index, const ui64 value) {
    if(index > getMemberCount()) throw SException(ErrCode::binom_out_of_range, "Out of buffer array range!");
    ui8 member_size = getMemberSize();
    ValueRef val(*data.type, maddto(data.bytes + 9 + index*member_size, member_size));
    val.setUnsigned(value);
    ++length();
    return val;
}

ValueRef BufferArray::insert(const ui64 index, const i64 value) {
    if(index > getMemberCount()) throw SException(ErrCode::binom_out_of_range, "Out of buffer array range!");
    ui8 member_size = getMemberSize();
    ValueRef val(*data.type, maddto(data.bytes + 9 + index*member_size, member_size));
    val.setSigned(value);
    ++length();
    return val;
}

ValueRef BufferArray::insert(const ui64 index, const f64 value) {
    if(index > getMemberCount()) throw SException(ErrCode::binom_out_of_range, "Out of buffer array range!");
    ui8 member_size = getMemberSize();
    ValueRef val(*data.type, maddto(data.bytes + 9 + index*member_size, member_size));
    val.setFloat(value);
    ++length();
    return val;
}

BufferArray::iterator BufferArray::insert(const ui64 index, const BufferArray& other) {
    if(index > getMemberCount()) throw SException(ErrCode::binom_out_of_range, "Out of buffer array range!");
    ui8 member_size = getMemberSize();
    iterator it(*data.type, maddto(data.bytes + 9 + index*member_size, other.getMemberCount() * member_size)), ret(it);
    for(const ValueRef &val : other) {
        *it << val;
        ++it;
    }
    length() += other.length();
    return ret;
}

void BufferArray::popBack(const ui64 n) {
  if(n > getMemberCount()) throw SException(ErrCode::binom_out_of_range);
  size_t member_size = getMemberSize();
  msubfrom(data.bytes + msize() - member_size*n, member_size*n);
  length() -= n;
}

void BufferArray::popFront(const ui64 n) {
  if(n > getMemberCount()) throw SException(ErrCode::binom_out_of_range);
  msubfrom(data.bytes + 9, getMemberSize()*n);
  length() -= n;
}

void BufferArray::remove(const ui64 index, const ui64 n) {
  if(index + n >= getMemberCount()) throw SException(ErrCode::binom_out_of_range);
  size_t member_size = getMemberSize();
  msubfrom(data.bytes + 9 + index*member_size, member_size*n);
  length() -= n;
}

BufferArray BufferArray::subarr(const ui64 index, const ui64 n) {
  if(index + n >= getMemberCount()) throw SException(ErrCode::binom_out_of_range);
  ui8 member_size = getMemberSize();
  byte* ptr = tryMalloc<byte>(9 + n*member_size);
  *reinterpret_cast<VarType*>(ptr) = *data.type;
  *reinterpret_cast<ui64*>(ptr + 1) = n;
  memcpy(ptr + 9, data.bytes + 9 + index*member_size, n*member_size);
  return ptr;
}

void BufferArray::clear() {mch(9);length() = 0;}


BufferArray& BufferArray::operator=(BufferArray other) {
  destroy();
  data.ptr = other.data.ptr;
  other.data.ptr = nullptr;
  return *this;
}

bool BufferArray::operator==(const BufferArray& other) const {
  if(*data.type != *other.data.type || length() != other.length()) return false;
  ValueIterator this_it = begin();
  ValueIterator other_it = other.begin();
  const ValueIterator this_end_it = end();
  for(;this_it != this_end_it;(++this_it, ++other_it))
    if(this_it->asUnsigned() != other_it->asUnsigned()) return false;
  return true;
}

bool BufferArray::operator>(const BufferArray& other) const {
  {
    ui64 this_size = length(),
        other_size = other.length();
    if(this_size > other_size) return true;
    else if(this_size < other_size) return false;
  }

  ValueIterator this_it = begin();
  ValueIterator other_it = other.begin();
  const ValueIterator this_end_it = end();
  for(;this_it != this_end_it;(++this_it, ++other_it))
    if(this_it->asUnsigned() > other_it->asUnsigned()) return true;
    else if(this_it->asUnsigned() < other_it->asUnsigned()) return false;
  return false;
}

bool BufferArray::operator<(const BufferArray& other) const {
  {
    ui64 this_size = length(),
        other_size = other.length();
    if(this_size < other_size) return true;
    else if(this_size > other_size) return false;
  }

  ValueIterator this_it = begin();
  ValueIterator other_it = other.begin();
  const ValueIterator this_end_it = end();
  for(;this_it != this_end_it;(++this_it, ++other_it))
    if(this_it->asUnsigned() < other_it->asUnsigned()) return true;
    else if(this_it->asUnsigned() > other_it->asUnsigned()) return false;
  return false;
}

bool BufferArray::operator>=(const BufferArray& other) const {
  {
    ui64 this_size = length(),
        other_size = other.length();
    if(this_size > other_size) return true;
    else if(this_size < other_size) return false;
  }

  ValueIterator this_it = begin();
  ValueIterator other_it = other.begin();
  const ValueIterator this_end_it = end();
  for(;this_it != this_end_it;(++this_it, ++other_it))
    if(this_it->asUnsigned() > other_it->asUnsigned()) return true;
    else if(this_it->asUnsigned() < other_it->asUnsigned()) return false;
  return true;
}

bool BufferArray::operator<=(const BufferArray& other) const {
  {
    ui64 this_size = length(),
        other_size = other.length();
    if(this_size < other_size) return true;
    else if(this_size > other_size) return false;
  }

  ValueIterator this_it = begin();
  ValueIterator other_it = other.begin();
  const ValueIterator this_end_it = end();
  for(;this_it != this_end_it;(++this_it, ++other_it))
    if(this_it->asUnsigned() < other_it->asUnsigned()) return true;
    else if(this_it->asUnsigned() > other_it->asUnsigned()) return false;
  return true;
}

BufferArray::iterator BufferArray::begin() const {return ValueIterator(*data.type, data.bytes + 9);}
BufferArray::iterator BufferArray::end() const {return ValueIterator(*data.type, data.bytes + msize());}

BufferArray::const_iterator BufferArray::cbegin() const {return ValueIterator(*data.type, data.bytes + 9);}
BufferArray::const_iterator BufferArray::cend() const {return ValueIterator(*data.type, data.bytes + msize());}

std::string BufferArray::toString() const {
  std::string str;
  str.reserve(length());
  for(const ValueRef &val : *this)
    str += char(val.asSigned());
  return str;
}

std::wstring BufferArray::toWString() const {
  std::wstring wstr;
  wstr.reserve (length());
  for(const ValueRef &val : *this)
    wstr += wchar_t(val.asUnsigned());
  return wstr;
}

ByteArray BufferArray::toByteArray() const {
  ui64 size = msize() - 9;
  ByteArray array(size);
  memcpy(array.begin(), data.bytes + 9, size);
  return array;
}

#define BUFFER_AS_STRING

std::ostream& operator<<(std::ostream& os, const binom::BufferArray& buffer) {
#ifdef BUFFER_AS_STRING
  if(buffer.getType() == VarType::byte_array)
    os << buffer.toString();
  else for(const binom::ValueRef &val : buffer)
    os << val << ' ';
#else
  for(const binom::ValueRef &val : buffer)
    os << val << ' ';
#endif

  return os;
}

const binom::BufferArray operator "" _buffer(const char* c_str) {return c_str;}
