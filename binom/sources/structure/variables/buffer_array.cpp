#include "binom/includes/structure/variables/variable.h"

using namespace binom;


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
  for(ui8 value : array)
    *it = value;
}

BufferArray::BufferArray(ui16arr array) : data(tryMalloc(9 + array.size()*2)) {
  data.type[0] = VarType::word_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  ui16* it = reinterpret_cast<ui16*>(data.bytes + 9);
  for(ui16 value : array)
    *it = value;
}

BufferArray::BufferArray(ui32arr array) : data(tryMalloc(9 + array.size()*4)) {
  data.type[0] = VarType::dword_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  ui32* it = reinterpret_cast<ui32*>(data.bytes + 9);
  for(ui32 value : array)
    *it = value;
}

BufferArray::BufferArray(ui64arr array) : data(tryMalloc(9 + array.size()*8)) {
  data.type[0] = VarType::qword_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  ui64* it = reinterpret_cast<ui64*>(data.bytes + 9);
  for(ui64 value : array)
    *it = value;
}





BufferArray::BufferArray(i8arr array) : data(tryMalloc(9 + array.size())) {
  data.type[0] = VarType::byte_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  i16* it = reinterpret_cast<i16*>(data.bytes + 9);
  for(i8 value : array)
    *it = value;
}

BufferArray::BufferArray(i16arr array) : data(tryMalloc(9 + array.size()*2)) {
  data.type[0] = VarType::word_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  i16* it = reinterpret_cast<i16*>(data.bytes + 9);
  for(i16 value : array)
    *it = value;
}

BufferArray::BufferArray(i32arr array) : data(tryMalloc(9 + array.size()*4)) {
  data.type[0] = VarType::dword_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  i32* it = reinterpret_cast<i32*>(data.bytes + 9);
  for(i32 value : array)
    *it = value;
}

BufferArray::BufferArray(i64arr array) : data(tryMalloc(9 + array.size()*8)) {
  data.type[0] = VarType::qword_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  i64* it = reinterpret_cast<i64*>(data.bytes + 9);
  for(i64 value : array)
    *it = value;
}

ValuePtr BufferArray::pushBack(const ui64 value) {
  ValuePtr val(*data.type, madd(getMemberSize()));
  val.setUnsigned(value);
  return val;
}

ValuePtr BufferArray::pushBack(const i64 value) {
  ValuePtr val(*data.type, madd(getMemberSize()));
  val.setSigned(value);
  return val;
}

ValuePtr BufferArray::pushBack(const f64 value) {
  ValuePtr val(*data.type, madd(getMemberSize()));
  val.setFloat(value);
  return val;
}

BufferArray::iterator BufferArray::pushBack(const BufferArray& other) {
  ui64 shift = getMemberCount();
  madd(other.getMemberCount() * getMemberSize());
  iterator it(*data.type, data.bytes + 9 + shift), ret(it);
  for(ValuePtr val : other) {
    *it << val;
    ++it;
  }
  return ret;
}

ValuePtr BufferArray::pushFront(const ui64 value) {
  ValuePtr val(*data.type, maddto(data.bytes + 9, getMemberSize()));
  val.setUnsigned(value);
  return val;
}

ValuePtr BufferArray::pushFront(const i64 value) {
  ValuePtr val(*data.type, maddto(data.bytes + 9, getMemberSize()));
  val.setSigned(value);
  return val;
}

ValuePtr BufferArray::pushFront(const f64 value) {
  ValuePtr val(*data.type, maddto(data.bytes + 9, getMemberSize()));
  val.setFloat(value);
  return val;
}

BufferArray::iterator BufferArray::pushFront(const BufferArray& other) {
  iterator it(*data.type, maddto(data.bytes + 9, other.getMemberCount() * getMemberSize()));
  for(ValuePtr val : other) {
    *it << val;
    ++it;
  }
  return begin();
}

ValuePtr BufferArray::insert(const ui64 index, const ui64 value) {
  if(index > getMemberCount()) throw SException(ErrCode::binom_out_of_range, "Out of buffer array range!");
  ValuePtr val(*data.type, maddto(data.bytes + 9 + index, getMemberSize()));
  val.setUnsigned(value);
  return val;
}

ValuePtr BufferArray::insert(const ui64 index, const i64 value) {
  if(index > getMemberCount()) throw SException(ErrCode::binom_out_of_range, "Out of buffer array range!");
  ValuePtr val(*data.type, maddto(data.bytes + 9 + index, getMemberSize()));
  val.setSigned(value);
  return val;
}

ValuePtr BufferArray::insert(const ui64 index, const f64 value) {
  if(index > getMemberCount()) throw SException(ErrCode::binom_out_of_range, "Out of buffer array range!");
  ValuePtr val(*data.type, maddto(data.bytes + 9 + index, getMemberSize()));
  val.setFloat(value);
  return val;
}

BufferArray::iterator BufferArray::insert(const ui64 index, const BufferArray& other) {
  if(index > getMemberCount()) throw SException(ErrCode::binom_out_of_range, "Out of buffer array range!");
  iterator it(*data.type, maddto(data.bytes + 9 + index, other.getMemberCount() * getMemberSize())), ret(it);
  for(ValuePtr val : other) {
    *it << val;
    ++it;
  }
  return ret;
}

std::ostream& operator<<(std::ostream& os, BufferArray& buffer) {
  for(ValuePtr val : buffer)
    os << val << ' ';
  return os;
}
