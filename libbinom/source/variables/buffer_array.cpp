#include "libbinom/include/variables/variable.h"
#include <cmath>

using namespace binom;

void BufferArray::destroyImpl() {
  if(!data.ptr) return;
  tryFree(data.ptr);
  data.ptr = nullptr;
}

void* BufferArray::cloneImpl() const {
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
  if(type < VarType::byte_array || type > VarType::qword_array) throw Exception(ErrCode::binom_invalid_type);
  data.ptr = tryMalloc (9);
  *data.type = type;
  length() = 0;
}

BufferArray::BufferArray(VarType type, size_t count) : data(nullptr){
  if(type < VarType::byte_array || type > VarType::qword_array) throw Exception(ErrCode::binom_invalid_type);
  size_t size = toSize(toValueType(type)) * count;
  data.ptr = tryMalloc(9 + size);
  *data.type = type;
  length() = count;
  memset(data.bytes + 9, 0, size);
}

BufferArray::BufferArray(ValType type) : BufferArray(toVarBufferType(type)) {}
BufferArray::BufferArray(ValType type, size_t count) : BufferArray(toVarBufferType(type), count) {}

BufferArray::BufferArray(const std::string_view str) : data(tryMalloc(9 + str.length())) {
  data.type[0] = VarType::byte_array;
  ui64 size = str.length();
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;
  memcpy(reinterpret_cast<char*>(data.bytes + 9), str.data(), size);
}

BufferArray::BufferArray(const std::wstring_view wstr) : data(tryMalloc(9 + wstr.length() * sizeof (std::wstring_view::value_type))) {
  data.type[0] = VarType::word_array;
  ui64 size = wstr.length() * sizeof (std::wstring_view::value_type);
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;
  memcpy(reinterpret_cast<char*>(data.bytes + 9), wstr.data(), size);
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




BufferArray::BufferArray(ui8* values, size_t count) : data(tryMalloc(9 + count)) {
    data.type[0] = VarType::byte_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = count;
    memcpy(data.bytes + 9, values, count);
}

BufferArray::BufferArray(ui16* values, size_t count) : data(tryMalloc(9 + count*2)) {
    data.type[0] = VarType::word_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = count;

    ui16* v_it = values;
    for(ui64* it = reinterpret_cast<ui64*>(data.bytes + 9); ui64(v_it - values) < count; (++it, ++v_it) )
        *it = *v_it;

}

BufferArray::BufferArray(ui32* values, size_t count) : data(tryMalloc(9 + count*4)) {
    data.type[0] = VarType::dword_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = count;

    ui32* v_it = values;
    for(ui32* it = reinterpret_cast<ui32*>(data.bytes + 9); ui64(v_it - values) < count; (++it, ++v_it) )
        *it = *v_it;

}

BufferArray::BufferArray(ui64* values, size_t count) : data(tryMalloc(9 + count*8)) {
    data.type[0] = VarType::qword_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = count;

    ui64* v_it = values;
    for(ui64* it = reinterpret_cast<ui64*>(data.bytes + 9); ui64(v_it - values) < count; (++it, ++v_it) )
        *it = *v_it;

}




BufferArray::BufferArray(i8* values, size_t count) : data(tryMalloc(9 + count)) {
    data.type[0] = VarType::byte_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = count;
    memcpy(data.bytes + 9, values, count);
}

BufferArray::BufferArray(i16* values, size_t count) : data(tryMalloc(9 + count*2)) {
    data.type[0] = VarType::word_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = count;
    memcpy(data.bytes + 9, values, count*2);
}

BufferArray::BufferArray(i32* values, size_t count) : data(tryMalloc(9 + count*4)) {
    data.type[0] = VarType::dword_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = count;
    memcpy(data.bytes + 9, values, count*4);

}

BufferArray::BufferArray(i64* values, size_t count) : data(tryMalloc(9 + count*8)) {
    data.type[0] = VarType::qword_array;
    *reinterpret_cast<ui64*>(data.bytes + 1) = count;
    memcpy(data.bytes + 9, values, count*2);
}

BufferArray::BufferArray(binom::ValType type, const void* ptr, size_t count) : data(tryMalloc(9 + toSize(type)*count)) {
  data.type[0] = toVarBufferType(type);
  *reinterpret_cast<ui64*>(data.bytes + 1) = count;
  memcpy(data.bytes + 9, ptr, count*toSize(type));
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

BufferArray::BufferArray(const BufferArray& other) : data(other.clone()) {}
BufferArray::BufferArray(BufferArray&& other) : data(other.data.ptr) {other.data.ptr = nullptr;}

BufferArray::BufferArray(Primitive primitive) : data(tryMalloc(9 + toSize(primitive.getValType()))) {
  switch (primitive.getValType()) {
    case binom::ValType::byte:
      data.type[0] = VarType::byte_array;
      data.bytes[9] = primitive.getValue().asUi8();
    break;
    case binom::ValType::word:
      data.type[0] = VarType::word_array;
      *reinterpret_cast<ui16*>(data.bytes + 9) = primitive.getValue().asUi16();
    break;
    case binom::ValType::dword:
      data.type[0] = VarType::dword_array;
      *reinterpret_cast<ui32*>(data.bytes + 9) = primitive.getValue().asUi32();
    break;
    case binom::ValType::qword:
      data.type[0] = VarType::qword_array;
      *reinterpret_cast<ui64*>(data.bytes + 9) = primitive.getValue().asUi64();
    break;
    default: throw Exception(ErrCode::binom_invalid_type);
  }
  length() = 1;
}

bool BufferArray::isPrintable() const {
  if(getType() != binom::VarType::byte_array)
    return false;
  for(const auto &value_ref : *this)
    if(!isprint(char(value_ref.asUi8()))) return false;
  return true;
}

ByteArray BufferArray::serialize() const {
  ByteArray bytes(1);
  bytes[0] = byte(*data.type);
  bytes += toChainNumber (length ());
  bytes.pushBack(data.bytes + 9, msize() - 9);
  return bytes;
}

BufferArray BufferArray::deserialize(binom::ByteArray::iterator& it) {
  VarType type = VarType(*it);
  if(type < VarType::byte_array || type > VarType::qword_array) throw Exception(ErrCode::binom_invalid_type);
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

ui64 BufferArray::serializedSize(ByteArray::iterator it) {
  ByteArray::iterator old_it = it;
  VarType type = VarType(*it);
  if(type < VarType::byte_array || type > VarType::qword_array) throw Exception(ErrCode::binom_invalid_type);
  ++it;
  it += fromChainNumber(it) * getMemberSize(type);
  return it - old_it;
}

void* BufferArray::getDataPointer() const {return data.bytes + 9;}
ui64 BufferArray::getDataSize() const {return getMemberCount() * getMemberSize();}

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
    if(index > getMemberCount()) throw Exception(ErrCode::binom_out_of_range, "Out of buffer array range!");
    ui8 member_size = getMemberSize();
    ValueRef val(*data.type, maddto(data.bytes + 9 + index*member_size, member_size));
    val.setUnsigned(value);
    ++length();
    return val;
}

ValueRef BufferArray::insert(const ui64 index, const i64 value) {
    if(index > getMemberCount()) throw Exception(ErrCode::binom_out_of_range, "Out of buffer array range!");
    ui8 member_size = getMemberSize();
    ValueRef val(*data.type, maddto(data.bytes + 9 + index*member_size, member_size));
    val.setSigned(value);
    ++length();
    return val;
}

ValueRef BufferArray::insert(const ui64 index, const f64 value) {
    if(index > getMemberCount()) throw Exception(ErrCode::binom_out_of_range, "Out of buffer array range!");
    ui8 member_size = getMemberSize();
    ValueRef val(*data.type, maddto(data.bytes + 9 + index*member_size, member_size));
    val.setFloat(value);
    ++length();
    return val;
}

BufferArray::iterator BufferArray::insert(const ui64 index, const BufferArray& other) {
    if(index > getMemberCount()) throw Exception(ErrCode::binom_out_of_range, "Out of buffer array range!");
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
  if(n > getMemberCount()) throw Exception(ErrCode::binom_out_of_range);
  size_t member_size = getMemberSize();
  msubfrom(data.bytes + msize() - member_size*n, member_size*n);
  length() -= n;
}

void BufferArray::popFront(const ui64 n) {
  if(n > getMemberCount()) throw Exception(ErrCode::binom_out_of_range);
  msubfrom(data.bytes + 9, getMemberSize()*n);
  length() -= n;
}

void BufferArray::remove(binom::ui64 index, binom::ui64 n) {
  if(index + n > getMemberCount())
    n = getMemberCount() - index;
  if(!n) return;
  size_t member_size = getMemberSize();
  msubfrom(data.bytes + 9 + index*member_size, member_size*n);
  length() -= n;
}

BufferArray BufferArray::subarr(binom::ui64 index, binom::ui64 n) {
  if(index + n > getMemberCount())
    n = getMemberCount() - index;
  if(!n) return BufferArray(getType());
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

bool BufferArray::operator==(binom::BufferArray other) const {
  if(getType() != other.getType()) return false;
  if(*data.type != *other.data.type || length() != other.length()) return false;
  ValueIterator this_it = begin();
  ValueIterator other_it = other.begin();
  const ValueIterator this_end_it = end();
  for(;this_it != this_end_it;(++this_it, ++other_it))
    if(this_it->asUnsigned() != other_it->asUnsigned()) return false;
  return true;
}

bool BufferArray::operator>(binom::BufferArray other) const {
  if(getType() > other.getType()) return true;
  if(getType() < other.getType()) return false;
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

bool BufferArray::operator<(binom::BufferArray other) const {
  if(getType() < other.getType()) return true;
  if(getType() > other.getType()) return false;
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

bool BufferArray::operator>=(BufferArray other) const {
  if(getType() > other.getType()) return true;
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

bool BufferArray::operator<=(binom::BufferArray other) const {
  if(getType() < other.getType()) return true;
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

i8 BufferArray::getCompare(BufferArray other) const {
  if(getType() > other.getType()) return 1;
  elif(getType() < other.getType()) return -1;
  if(length() > other.length()) return 1;
  elif(length() < other.length()) return -1;
  ValueIterator it = other.begin();
  for(const ValueRef &val : *this) {
    if(val > *it) return 1;
    elif(val < *it) return -1;
    ++it;
  }
  return 0;
}

BufferArray::iterator BufferArray::findMemory(BufferArray data) const {
  ui64 f_size = getDataSize(), s_size = data.getDataSize();
  if(f_size < s_size) return end();
  f_size -= s_size-1;
  const ui8* f_mem_ptr = reinterpret_cast<ui8*>(getDataPointer());
  const ui8* s_mem_ptr = reinterpret_cast<ui8*>(data.getDataPointer());
  ui8* f_it = const_cast<ui8*>(f_mem_ptr);
  for(;f_size;(--f_size, ++f_it))
    if(ismemeq(f_it, s_mem_ptr, s_size))
      return ValueIterator(getType(), this->data.bytes + 9 + static_cast<ui64>(std::floor((f_it - f_mem_ptr)/getDataSize())));
  return end();
}

BufferArray::iterator BufferArray::findValue(BufferArray data) const {
  ui64 f_count = getMemberCount(), s_count = data.getMemberCount();
  if(f_count < s_count) return end();
  iterator f_start = begin(),
           f_it = f_start,
           f_end = begin() + (f_count - s_count + 1),
           s_it = data.begin();
  for(;f_it != f_end; (++f_it, ++s_it))
    if(f_it == s_it) {
      bool is_equal = true;
      ui64 cmp_cout = s_count;
      for(auto cmp_f_it = f_it+1, cmp_s_it = s_it+1;cmp_cout;(++f_it, ++s_it, --cmp_cout)) {
        if(cmp_f_it == cmp_s_it) continue;
        else {is_equal = false; break;}
      }
      if(is_equal) return f_it;
    } else continue;
  return end();
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

const binom::BufferArray operator ""_vbfr(const char* c_str, std::size_t) {return c_str;}
