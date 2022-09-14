#include "libbinom/include/variables/generic_value.hxx"
#include "libbinom/include/binom_impl/resource_control.hxx"

binom::arithmetic::ArithmeticData& binom::GenericValue::getArithmeticDataImpl() const {return const_cast<arithmetic::ArithmeticData&>(data);}

binom::ValType binom::GenericValue::getValTypeImpl() const {return type;}

void binom::GenericValue::reallocateImpl(ValType type) noexcept {}

void binom::GenericValue::setTypeImpl(ValType new_type) noexcept {type = new_type;}

binom::GenericValue::GenericValue(ValType type, arithmetic::ArithmeticData data) : type(type), data(data) {}

binom::priv::ResourceData binom::GenericValue::toResourceData() const noexcept {return {toVarType(type), reinterpret_cast<const priv::ResourceData::Data&>(data)};}

binom::GenericValue::GenericValue() noexcept {}

binom::GenericValue::GenericValue(bool value) noexcept : type(ValType::boolean), data{.bool_val = value} {getLock(MtxLockType::shared_locked);}

binom::GenericValue::GenericValue(ui8 value) noexcept : type(ValType::ui8), data{.ui8_val = value} {}

binom::GenericValue::GenericValue(i8 value) noexcept : type(ValType::si8), data{.i8_val = value} {}

binom::GenericValue::GenericValue(ui16 value) noexcept : type(ValType::ui16), data{.ui16_val = value} {}

binom::GenericValue::GenericValue(i16 value) noexcept : type(ValType::si16), data{.i16_val = value} {}

binom::GenericValue::GenericValue(ui32 value) noexcept : type(ValType::ui32), data{.ui32_val = value} {}

binom::GenericValue::GenericValue(i32 value) noexcept : type(ValType::si32), data{.i32_val = value} {}

binom::GenericValue::GenericValue(f32 value) noexcept : type(ValType::f32), data{.f32_val = value} {}

binom::GenericValue::GenericValue(ui64 value) noexcept : type(ValType::ui64), data{.ui64_val = value} {}

binom::GenericValue::GenericValue(i64 value) noexcept : type(ValType::si64), data{.i64_val = value} {}

binom::GenericValue::GenericValue(f64 value) noexcept : type(ValType::f64), data{.f64_val = value} {}

binom::GenericValue::GenericValue(const GenericValue& other) : type(other.type), data{.ui64_val = other.data.ui64_val} {}

binom::GenericValue::GenericValue(const GenericValue&& other) : type(other.type), data{.ui64_val = other.data.ui64_val} {}






binom::arithmetic::ArithmeticData& binom::GenericValueRef::getArithmeticDataImpl() const noexcept {return *ptr.num_data_ptr;}

binom::ValType binom::GenericValueRef::getValTypeImpl() const noexcept {if(ptr.ptr) return value_type; else return ValType::invalid_type;}

binom::GenericValueRef::GenericValueRef(ValType value_type, void* ptr)
  : value_type(value_type), ptr(ptr) {}

binom::GenericValueRef::GenericValueRef(const GenericValueRef& other) : value_type(other.value_type), ptr(other.ptr.ptr) {}

binom::GenericValueRef::GenericValueRef(const GenericValueRef&& other) : value_type(other.value_type), ptr(other.ptr.ptr) {}

binom::GenericValueRef::operator GenericValue() const {return GenericValue();}






binom::GenericValueIterator::GenericValueIterator(ValType value_type, void* ptr)
  : value_type(value_type), ptr(ptr) {}

binom::GenericValueIterator::GenericValueIterator(const GenericValueIterator& other)
  : value_type(other.value_type), ptr(other.ptr.ptr) {}

binom::GenericValueIterator::GenericValueIterator(const GenericValueIterator&& other)
  : value_type(other.value_type), ptr(other.ptr.ptr) {}

binom::GenericValueIterator& binom::GenericValueIterator::operator=(GenericValueIterator&& other) noexcept {
  return *new(this) GenericValueIterator(other);
}

binom::GenericValueIterator& binom::GenericValueIterator::operator++() noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ++ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: ++ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: ++ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: ++ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

binom::GenericValueIterator& binom::GenericValueIterator::operator--() noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: --ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: --ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: --ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: --ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

binom::GenericValueIterator binom::GenericValueIterator::operator+(ssize_t shift) noexcept { return GenericValueIterator(self) += shift; }
binom::GenericValueIterator binom::GenericValueIterator::operator-(ssize_t shift) noexcept { return GenericValueIterator(self) += shift; }
const binom::GenericValueIterator binom::GenericValueIterator::operator+(ssize_t shift) const noexcept { return GenericValueIterator(self) += shift; }
const binom::GenericValueIterator binom::GenericValueIterator::operator-(ssize_t shift) const noexcept { return GenericValueIterator(self) += shift; }

const binom::GenericValueIterator& binom::GenericValueIterator::operator++() const noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ++ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: ++ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: ++ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: ++ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

const binom::GenericValueIterator& binom::GenericValueIterator::operator--() const noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: --ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: --ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: --ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: --ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

binom::GenericValueIterator binom::GenericValueIterator::operator++(int) noexcept {
  GenericValueIterator tmp(*this);
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ++ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: ++ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: ++ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: ++ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return tmp;
}

binom::GenericValueIterator binom::GenericValueIterator::operator--(int) noexcept {
  GenericValueIterator tmp(*this);
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: --ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: --ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: --ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: --ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return tmp;
}

const binom::GenericValueIterator binom::GenericValueIterator::operator++(int) const noexcept {
  GenericValueIterator tmp(*this);
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ++ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: ++ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: ++ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: ++ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return tmp;
}

const binom::GenericValueIterator binom::GenericValueIterator::operator--(int) const noexcept {
  GenericValueIterator tmp(*this);
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: --ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: --ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: --ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: --ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return tmp;
}

binom::GenericValueIterator& binom::GenericValueIterator::operator+=(ssize_t shift) noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ptr.ui8_ptr += shift; break;
  case binom::VarBitWidth::word: ptr.ui16_ptr += shift; break;
  case binom::VarBitWidth::dword: ptr.ui32_ptr += shift; break;
  case binom::VarBitWidth::qword: ptr.ui64_ptr += shift; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

binom::GenericValueIterator& binom::GenericValueIterator::operator-=(ssize_t shift) noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ptr.ui8_ptr -= shift; break;
  case binom::VarBitWidth::word: ptr.ui16_ptr -= shift; break;
  case binom::VarBitWidth::dword: ptr.ui32_ptr -= shift; break;
  case binom::VarBitWidth::qword: ptr.ui64_ptr -= shift; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

const binom::GenericValueIterator& binom::GenericValueIterator::operator+=(ssize_t shift) const noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ptr.ui8_ptr += shift; break;
  case binom::VarBitWidth::word: ptr.ui16_ptr += shift; break;
  case binom::VarBitWidth::dword: ptr.ui32_ptr += shift; break;
  case binom::VarBitWidth::qword: ptr.ui64_ptr += shift; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

const binom::GenericValueIterator& binom::GenericValueIterator::operator-=(ssize_t shift) const noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ptr.ui8_ptr -= shift; break;
  case binom::VarBitWidth::word: ptr.ui16_ptr -= shift; break;
  case binom::VarBitWidth::dword: ptr.ui32_ptr -= shift; break;
  case binom::VarBitWidth::qword: ptr.ui64_ptr -= shift; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

bool binom::GenericValueIterator::operator==(GenericValueIterator&& other) const noexcept {return ptr.ptr == other.ptr.ptr;}

bool binom::GenericValueIterator::operator!=(GenericValueIterator&& other) const noexcept {return ptr.ptr != other.ptr.ptr;}

bool binom::GenericValueIterator::operator==(const GenericValueIterator& other) const noexcept {return ptr.ptr == other.ptr.ptr;}

bool binom::GenericValueIterator::operator!=(const GenericValueIterator& other) const noexcept {return ptr.ptr != other.ptr.ptr;}

binom::GenericValueRef binom::GenericValueIterator::operator*() noexcept {return GenericValueRef(value_type, ptr.ptr);}

binom::GenericValueRef binom::GenericValueIterator::operator[](ssize_t shift) noexcept {return *(self + shift);}
const binom::GenericValueRef binom::GenericValueIterator::operator[](ssize_t shift) const noexcept {return *(self + shift);}

const binom::GenericValueRef binom::GenericValueIterator::operator*() const noexcept {return GenericValueRef(value_type, ptr.ptr);}






binom::ReverseGenericValueIterator::ReverseGenericValueIterator(ValType value_type, void* ptr)
  : value_type(value_type), ptr(ptr) {}

binom::ReverseGenericValueIterator::ReverseGenericValueIterator(const ReverseGenericValueIterator& other)
  : value_type(other.value_type), ptr(other.ptr.ptr) {}

binom::ReverseGenericValueIterator::ReverseGenericValueIterator(ReverseGenericValueIterator&& other)
  : value_type(other.value_type), ptr(other.ptr.ptr) {}

binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator=(ReverseGenericValueIterator&& other) noexcept {
  return *new(this) ReverseGenericValueIterator(other);
}

binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator++() noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: --ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: --ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: --ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: --ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator--() noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ++ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: ++ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: ++ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: ++ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

const binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator++() const noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: --ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: --ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: --ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: --ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

const binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator--() const noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ++ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: ++ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: ++ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: ++ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

binom::ReverseGenericValueIterator binom::ReverseGenericValueIterator::operator++(int) noexcept {
  ReverseGenericValueIterator tmp(*this);
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: --ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: --ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: --ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: --ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return tmp;
}

binom::ReverseGenericValueIterator binom::ReverseGenericValueIterator::operator--(int) noexcept {
  ReverseGenericValueIterator tmp(*this);
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ++ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: ++ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: ++ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: ++ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return tmp;
}

const binom::ReverseGenericValueIterator binom::ReverseGenericValueIterator::operator++(int) const noexcept {
  ReverseGenericValueIterator tmp(*this);
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: --ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: --ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: --ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: --ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return tmp;
}

const binom::ReverseGenericValueIterator binom::ReverseGenericValueIterator::operator--(int) const noexcept {
  ReverseGenericValueIterator tmp(*this);
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ++ptr.ui8_ptr; break;
  case binom::VarBitWidth::word: ++ptr.ui16_ptr; break;
  case binom::VarBitWidth::dword: ++ptr.ui32_ptr; break;
  case binom::VarBitWidth::qword: ++ptr.ui64_ptr; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return tmp;
}

binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator+=(ptrdiff_t shift) noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ptr.ui8_ptr -= shift; break;
  case binom::VarBitWidth::word: ptr.ui16_ptr -= shift; break;
  case binom::VarBitWidth::dword: ptr.ui32_ptr -= shift; break;
  case binom::VarBitWidth::qword: ptr.ui64_ptr -= shift; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator-=(ptrdiff_t shift) noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ptr.ui8_ptr += shift; break;
  case binom::VarBitWidth::word: ptr.ui16_ptr += shift; break;
  case binom::VarBitWidth::dword: ptr.ui32_ptr += shift; break;
  case binom::VarBitWidth::qword: ptr.ui64_ptr += shift; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

const binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator+=(ptrdiff_t shift) const noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ptr.ui8_ptr -= shift; break;
  case binom::VarBitWidth::word: ptr.ui16_ptr -= shift; break;
  case binom::VarBitWidth::dword: ptr.ui32_ptr -= shift; break;
  case binom::VarBitWidth::qword: ptr.ui64_ptr -= shift; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

const binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator-=(ptrdiff_t shift) const noexcept {
  switch (toBitWidth(value_type)) {
  case binom::VarBitWidth::byte: ptr.ui8_ptr += shift; break;
  case binom::VarBitWidth::word: ptr.ui16_ptr += shift; break;
  case binom::VarBitWidth::dword: ptr.ui32_ptr += shift; break;
  case binom::VarBitWidth::qword: ptr.ui64_ptr += shift; break;
  case binom::VarBitWidth::invalid_type: break;
  }
  return self;
}

bool binom::ReverseGenericValueIterator::operator==(ReverseGenericValueIterator&& other) const noexcept {return ptr.ptr == other.ptr.ptr;}

bool binom::ReverseGenericValueIterator::operator!=(ReverseGenericValueIterator&& other) const noexcept {return ptr.ptr != other.ptr.ptr;}

binom::GenericValueRef binom::ReverseGenericValueIterator::operator*() noexcept {return GenericValueRef(value_type, ptr.ptr);}
const binom::GenericValueRef binom::ReverseGenericValueIterator::operator*() const noexcept {return GenericValueRef(value_type, ptr.ptr);}
