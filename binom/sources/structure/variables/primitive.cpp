#include "binom/includes/structure/variables/variable.h"

void binom::Primitive::destroy() {tryFree(data.ptr);data.ptr = nullptr;}

void* binom::Primitive::clone() {
  ui64 size = msize();
  void* ptr = tryMalloc(size);
  memcpy(ptr, data.ptr, size);
  return ptr;
}


binom::Primitive::Primitive(bool value) : data(tryMalloc(2)) {
  data.type[0] = VarType::byte;
  reinterpret_cast<ui8*>(data.bytes)[1] = value;
}

binom::Primitive::Primitive(binom::ui8 value) : data(tryMalloc(2)) {
  data.type[0] = VarType::byte;
  data.bytes[1] = value;
}

binom::Primitive::Primitive(binom::ui16 value) : data(tryMalloc(3)) {
  data.type[0] = VarType::word;
  *reinterpret_cast<ui16*>(data.bytes + 1) = value;
}

binom::Primitive::Primitive(binom::ui32 value) : data(tryMalloc(5)) {
  data.type[0] = VarType::dword;
  *reinterpret_cast<ui32*>(data.bytes + 1) = value;
}

binom::Primitive::Primitive(binom::ui64 value) : data(tryMalloc(9)) {
  data.type[0] = VarType::qword;
  *reinterpret_cast<ui64*>(data.bytes + 1) = value;
}




binom::Primitive::Primitive(binom::i8 value) : data(tryMalloc(2)) {
  data.type[0] = VarType::byte;
  *reinterpret_cast<i8*>(data.bytes + 1) = value;
}

binom::Primitive::Primitive(binom::i16 value) : data(tryMalloc(3)) {
  data.type[0] = VarType::word;
  *reinterpret_cast<i16*>(data.bytes + 1) = value;
}

binom::Primitive::Primitive(binom::i32 value) : data(tryMalloc(5)) {
  data.type[0] = VarType::dword;
  *reinterpret_cast<i32*>(data.bytes + 1) = value;
}

binom::Primitive::Primitive(binom::i64 value) : data(tryMalloc(9)) {
  data.type[0] = VarType::qword;
  *reinterpret_cast<i64*>(data.bytes + 1) = value;
}

binom::Primitive::Primitive(binom::f32 value) : data(tryMalloc(5)) {
  data.type[0] = VarType::dword;
  *reinterpret_cast<f32*>(data.bytes + 1) = value;
}

binom::Primitive::Primitive(binom::f64 value) : data(tryMalloc(9)) {
  data.type[0] = VarType::qword;
  *reinterpret_cast<f64*>(data.bytes + 1) = value;
}

binom::Primitive::Primitive(binom::Primitive&& other) : data(other.data.ptr) {other.data.ptr = nullptr;}
binom::Primitive::Primitive(binom::Primitive& other) : data(other.clone()) {}

binom::Primitive::Primitive(binom::ValueRef value) : data(tryMalloc(1 + value.getSize())) {
  data.type[0] = toVarType(value.getType());
  switch (value.getType()) {
    case ValType::byte: data.bytes[1] = value.asUnsigned();return;
    case ValType::word: reinterpret_cast<ui16*>(data.bytes+1)[0] = value.asUnsigned();return;
    case ValType::dword:reinterpret_cast<ui32*>(data.bytes+1)[0] = value.asUnsigned();return;
    case ValType::qword:reinterpret_cast<ui64*>(data.bytes+1)[0] = value.asUnsigned();return;
  }
}

binom::Primitive::~Primitive() {destroy();}

binom::ByteArray binom::Primitive::serialize() const {return ByteArray(data.ptr, msize ());}

binom::Primitive& binom::Primitive::operator=(binom::Primitive& other) {
  getValue() << other.getValue();
  return *this;
}

std::ostream& operator<<(std::ostream& os, binom::Primitive& primitive) {return os << *primitive;}
