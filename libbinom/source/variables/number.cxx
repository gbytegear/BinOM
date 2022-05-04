#include "libbinom/include/variables/number.hxx"

using namespace binom;
using namespace binom::priv;

GenericValue::operator Number() {
  switch (getValType()) {
  case binom::ValType::boolean: return data.bool_val;
  case binom::ValType::ui8: return data.ui8_val;
  case binom::ValType::si8: return data.i8_val;
  case binom::ValType::ui16: return data.ui16_val;
  case binom::ValType::si16: return data.i16_val;
  case binom::ValType::ui32: return data.ui32_val;
  case binom::ValType::si32: return data.i32_val;
  case binom::ValType::f32: return data.f32_val;
  case binom::ValType::ui64: return data.ui64_val;
  case binom::ValType::si64: return data.i64_val;
  case binom::ValType::f64: return data.f64_val;
  case binom::ValType::invalid_type: return 0;
  }
  return 0;
}

OptionalSharedRecursiveLock GenericValueRef::getLockImpl(binom::priv::MtxLockType lock_type) const noexcept {
  return owner->getLock(lock_type);
}
