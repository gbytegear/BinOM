#include "libbinom/include/variables/generic_value.hxx"
#include "libbinom/include/binom_impl/resource_control.hxx"
#include "libbinom/include/binom_impl/ram_storage_implementation.hxx"

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




binom::ValType binom::GenericValueRef::getValTypeImpl() const noexcept {
  if(index == INVALID_INDEX) return ValType::invalid_type;
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk)
    return ValType::invalid_type;
  return toValueType(link->type);
}

binom::arithmetic::ArithmeticData& binom::GenericValueRef::getArithmeticDataImpl() noexcept {
  switch (toTypeClass(link->type)) {
  default:
  case binom::VarTypeClass::null:
  case binom::VarTypeClass::number: return reinterpret_cast<arithmetic::ArithmeticData&>(link->data);
  case binom::VarTypeClass::buffer_array:
  return *reinterpret_cast<arithmetic::ArithmeticData*>(link->data.buffer_array_header->get(getBitWidth(), index));
  }
}

const binom::arithmetic::ArithmeticData& binom::GenericValueRef::getArithmeticDataImpl() const noexcept {
  switch (toTypeClass(link->type)) {
  default:
  case binom::VarTypeClass::null:
  case binom::VarTypeClass::number: return reinterpret_cast<const arithmetic::ArithmeticData&>(link->data);
  case binom::VarTypeClass::buffer_array:
  return *reinterpret_cast<const arithmetic::ArithmeticData*>(link->data.buffer_array_header->get(getBitWidth(), index));
  }
}

bool binom::GenericValueRef::checkLock(const OptionalSharedRecursiveLock& lock) noexcept {return lock.has_value();}

binom::GenericValueRef::GenericValueRef(priv::Link link, size_t index)
  : link(std::move(link)), index(index) {}

binom::GenericValueRef::GenericValueRef(const GenericValueRef& other) : link(other.link), index(other.index) {}

binom::GenericValueRef::GenericValueRef(const GenericValueRef&& other) : link(other.link), index(other.index) {}

binom::OptionalSharedRecursiveLock binom::GenericValueRef::getLock(shared_recursive_mtx::MtxLockType lock_type) const noexcept { return link.getLock(lock_type); }

binom::GenericValueRef::operator GenericValue() const { return GenericValue(getValTypeImpl(), getArithmeticDataImpl()); }





binom::GenericValueIterator::GenericValueIterator(priv::Link link, ssize_t index)
  : link(std::move(link)), index(index) {}

binom::GenericValueIterator::GenericValueIterator(const GenericValueIterator& other)
  : link(other.link), index(other.index) {}

binom::GenericValueIterator::GenericValueIterator(const GenericValueIterator&& other)
  : link(other.link), index(other.index) {}

binom::GenericValueIterator& binom::GenericValueIterator::operator=(GenericValueIterator&& other) noexcept {
  if(index == INVALID_INDEX) return self;
  return *new(this) GenericValueIterator(other);
}

binom::GenericValueIterator& binom::GenericValueIterator::operator++() noexcept {
  if(index == INVALID_INDEX) return self;
  ++index;
  return self;
}

binom::GenericValueIterator& binom::GenericValueIterator::operator--() noexcept {
  if(index == INVALID_INDEX) return self;
  --index;
  return self;
}

binom::GenericValueIterator binom::GenericValueIterator::operator++(int) noexcept {
  if(index == INVALID_INDEX) return self;
  GenericValueIterator tmp(*this);
  ++index;
  return tmp;
}

binom::GenericValueIterator binom::GenericValueIterator::operator--(int) noexcept {
  if(index == INVALID_INDEX) return self;
  GenericValueIterator tmp(*this);
  --index;
  return tmp;
}

binom::GenericValueIterator& binom::GenericValueIterator::operator+=(ssize_t shift) noexcept {
  if(index == INVALID_INDEX) return self;
  index += shift;
  return self;
}

binom::GenericValueIterator& binom::GenericValueIterator::operator-=(ssize_t shift) noexcept {
  if(index == INVALID_INDEX) return self;
  index -= shift;
  return self;
}

bool binom::GenericValueIterator::operator==(GenericValueIterator&& other) const noexcept {return index == other.index;}

bool binom::GenericValueIterator::operator!=(GenericValueIterator&& other) const noexcept {return index != other.index;}

bool binom::GenericValueIterator::operator==(const GenericValueIterator& other) const noexcept {return index == other.index;}

bool binom::GenericValueIterator::operator!=(const GenericValueIterator& other) const noexcept {return index != other.index;}

binom::GenericValueRef binom::GenericValueIterator::operator*() const noexcept {if(index == INVALID_INDEX) return GenericValueRef(link); return GenericValueRef(link, index);}





binom::ReverseGenericValueIterator::ReverseGenericValueIterator(priv::Link link, ssize_t index)
  : link(std::move(link)), index(index) {}

binom::ReverseGenericValueIterator::ReverseGenericValueIterator(const ReverseGenericValueIterator& other)
  : link(other.link), index(other.index) {}

binom::ReverseGenericValueIterator::ReverseGenericValueIterator(ReverseGenericValueIterator&& other)
  : link(other.link), index(other.index) {}

binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator=(ReverseGenericValueIterator&& other) noexcept {
  return *new(this) ReverseGenericValueIterator(other);
}

binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator++() noexcept {
  --index;
  return self;
}

binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator--() noexcept {
  ++index;
  return self;
}

binom::ReverseGenericValueIterator binom::ReverseGenericValueIterator::operator++(int) noexcept {
  ReverseGenericValueIterator tmp(*this);
  --index;
  return tmp;
}

binom::ReverseGenericValueIterator binom::ReverseGenericValueIterator::operator--(int) noexcept {
  ReverseGenericValueIterator tmp(*this);
  ++index;
  return tmp;
}

binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator+=(ssize_t shift) noexcept {
  index -= shift;
  return self;
}

binom::ReverseGenericValueIterator& binom::ReverseGenericValueIterator::operator-=(ssize_t shift) noexcept {
  index += shift;
  return self;
}

bool binom::ReverseGenericValueIterator::operator==(ReverseGenericValueIterator&& other) const noexcept {return index == other.index;}

bool binom::ReverseGenericValueIterator::operator!=(ReverseGenericValueIterator&& other) const noexcept {return index != other.index;}

bool binom::ReverseGenericValueIterator::operator==(const ReverseGenericValueIterator& other) const noexcept {return index == other.index;}

bool binom::ReverseGenericValueIterator::operator!=(const ReverseGenericValueIterator& other) const noexcept {return index != other.index;}

binom::GenericValueRef binom::ReverseGenericValueIterator::operator*() const noexcept {return GenericValueRef(link, index);}
