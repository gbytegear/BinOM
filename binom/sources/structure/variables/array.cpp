#include "binom/includes/structure/variables/variable.h"

using namespace binom;


void Array::mch(size_t new_size) {
  data.ptr = tryRealloc(data.ptr, new_size);
}

void* Array::madd(size_t add_size) {
  size_t shift = msize();
  mch(shift + add_size);
  return data.bytes + shift;
}

void Array::msub(size_t sub_size) {
  mch(msize() - sub_size);
}

void* Array::maddto(void* to, size_t size) {
  size_t pos = reinterpret_cast<byte*>(to) - data.bytes;
  size_t old_size = msize();
  madd(size);
  memmove(data.bytes + pos + size, data.bytes + pos, old_size - pos);
  return data.bytes + pos;
}

void Array::msubfrom(void* from, size_t size) {
  if(from < data.ptr) throw Exception(ErrCode::binom_out_of_range);
  size_t old_size = msize();
  memmove(from, reinterpret_cast<byte*>(from) + size,
          old_size - (reinterpret_cast<byte*>(from) - data.bytes) - size);
  mch(old_size - size);
}

void* Array::clone() const {
  ui64 size = msize();
  void* ptr = tryMalloc(size);
  memcpy(ptr, data.ptr, 9);

  void** it = reinterpret_cast<void**>(reinterpret_cast<ui8*>(ptr) + 9);
  for(Variable& var : *this) {
    *it = var.clone();
    ++it;
  }
  return ptr;
}

void Array::destroy() {
  if(!data.ptr) return;
  for(Variable& var : *this) var.destroy();
  free(data.ptr);
  data.ptr = nullptr;
}

Array::Array() : data(tryMalloc(9)) {
  *data.type = VarType::array;
  length() = 0;
}

Array::Array(varr array) : data(tryMalloc(9 + array.size()*sizeof(Variable))) {
  data.type[0] = VarType::array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  void** it = reinterpret_cast<void**>(data.bytes + 9);
  for(const Variable& value : array) {
    *it = value.getDataPointer();
    const_cast<Variable&>(value).data.ptr = nullptr;
    ++it;
  }
}

Array::Array(Array&& other) : data(other.data.ptr) {other.data.ptr = nullptr;}
Array::Array(const binom::Array& other) : data(other.clone()) {}

ByteArray Array::serialize() const {
  ByteArray serialized;
  serialized += byte(VarType::array);
  for(Variable& var : *this)
    serialized += var.serialize ();
  serialized += byte(VarType::end);
  return serialized;
}

binom::Array Array::deserialize(ByteArray::iterator& it) {
  VarType type = VarType(*it);
  if(type != VarType::array) throw Exception(ErrCode::binom_invalid_type);
  ++it;
  Array arr;
  while (VarType(*it) != VarType::end) {
    arr.pushBack(Variable::deserialize(it));
  }
  ++it;
  return arr;
}

Variable& Array::getVariable(ui64 index) const {
  if(index >= getMemberCount()) throw Exception(ErrCode::binom_out_of_range, "Out of array range");
  return begin()[index];
}

Variable& Array::insert(ui64 index, Variable var) {
  if(index > getMemberCount()) throw Exception(ErrCode::binom_out_of_range, "Out of array range");
  Variable* new_var = reinterpret_cast<Variable*>(maddto(data.bytes + msize(), sizeof (Variable)));
  ++length();
  new_var->data.ptr = var.data.ptr;
  var.data.ptr = nullptr;
  return *new_var;
}

Variable& Array::pushBack(Variable var) {
  Variable* new_var = reinterpret_cast<Variable*>(madd(sizeof (Variable)));
  ++length();
  new_var->data.ptr = var.data.ptr;
  var.data.ptr = nullptr;
  return *new_var;
}

Variable& Array::pushFront(Variable var) {
  Variable* new_var = reinterpret_cast<Variable*>(maddto(data.bytes + 9, sizeof (Variable)));
  ++length();
  new_var->data.ptr = var.data.ptr;
  var.data.ptr = nullptr;
  return *new_var;
}

void Array::remove(ui64 index, ui64 n) {
  if(index + n > getMemberCount()) throw Exception(ErrCode::binom_out_of_range);
  Variable* start = reinterpret_cast<Variable*>(data.bytes + 9 + index*sizeof(Variable));
  {
    Variable* it = start;
    for(ui64 i = 0;i < n;(++i,++it))
      it->destroy();
  }
  msubfrom(start, n*sizeof(Variable));
  length() -= n;
}

void Array::popBack(ui64 n) {
  if(n > getMemberCount()) throw Exception(ErrCode::binom_out_of_range);
  Variable* start = reinterpret_cast<Variable*>(data.bytes + msize() - n*sizeof(Variable));
  {
    Variable* it = start;
    for(ui64 i = 0;i < n;(++i,++it))
      it->destroy();
  }
  msubfrom(start, n*sizeof(Variable));
  length() -= n;
}

void Array::popFront(ui64 n) {
  if(n > getMemberCount()) throw Exception(ErrCode::binom_out_of_range);
  Variable* start = reinterpret_cast<Variable*>(data.bytes + 9);
  {
    Variable* it = start;
    for(ui64 i = 0;i < n;(++i,++it))
      it->destroy();
  }
  msubfrom(start, n*sizeof(Variable));
  length() -= n;
}

void Array::clear() {
  for(Variable& var : *this)
    var.destroy();
  mch(9);
  length() = 0;
}

Array& Array::operator+=(Variable var) {
  Variable* new_var = reinterpret_cast<Variable*>(madd(sizeof (Variable)));
  ++length();
  new_var->data.ptr = var.data.ptr;
  var.data.ptr = nullptr;
  return *this;
}

Array& Array::operator=(Array other) {
  destroy();
  data.ptr = other.data.ptr;
  other.data.ptr = nullptr;
  return *this;
}
