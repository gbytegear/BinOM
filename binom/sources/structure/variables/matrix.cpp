#include "binom/includes/structure/variables/variable.h"

using namespace binom;

void Matrix::mch(size_t new_size) {
  data.ptr = tryRealloc(data.ptr, new_size);
}

void* Matrix::madd(size_t add_size) {
  size_t shift = msize();
  mch(shift + add_size);
  return data.bytes + shift;
}

void Matrix::msub(size_t sub_size) {
  mch(msize() - sub_size);
}

void* Matrix::maddto(void* to, size_t size) {
  size_t pos = reinterpret_cast<byte*>(to) - data.bytes;
  size_t old_size = msize();
  madd(size);
  memmove(data.bytes + pos + size, data.bytes + pos, old_size - pos);
  return data.bytes + pos;
}

void Matrix::msubfrom(void* from, size_t size) {
  if(from < data.ptr) throw SException(ErrCode::binom_out_of_range);
  size_t old_size = msize();
  memmove(from, reinterpret_cast<byte*>(from) + size,
          old_size - (reinterpret_cast<byte*>(from) - data.bytes) - size);
  mch(old_size - size);
}
