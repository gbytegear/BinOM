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

void* Array::clone() {
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
  for(Variable* it = reinterpret_cast<Variable*>(data.bytes + 9),
      *const end = reinterpret_cast<Variable*>(data.bytes + 9 + length()*PTR_SZ);
      it != end;
      ++it) {
    it->destroy();
  }
}

Array::Array(varr array) : data(tryMalloc(9 + array.size()*sizeof(Variable))) {
  data.type[0] = VarType::array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  void** it = reinterpret_cast<void**>(data.bytes + 9);
  for(const Variable& value : array) {
    *it = value.getDataPointer();
    const_cast<Variable&>(value).data.ptr = nullptr;
  }
}

Array::Array(Array&& other) : data(other.data.ptr) {other.data.ptr = nullptr;}
Array::Array(Array& other) : data(other.clone()) {}

Variable& Array::getVariable(ui64 index) {
  if(index >= getMemberCount()) throw SException(ErrCode::binom_out_of_range, "Out of array range");
  return begin()[index];
}
