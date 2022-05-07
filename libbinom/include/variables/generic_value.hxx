#ifndef GENERIC_VALUE_HXX
#define GENERIC_VALUE_HXX

#include "../utils/generic_arithmetic.hxx"
#include "../utils/resource_control.hxx"

namespace binom {

class GenericValueIterator;
class GenericValueRef;

class GenericValue :
    public arithmetic::ArithmeticTypeBase<GenericValue>,
    public arithmetic::CopyableArithmeticTypeBase<GenericValue>,
    public arithmetic::CastableArithmeticTypeBase<GenericValue>,
    public arithmetic::ArithmeticImplPlaceholders<GenericValue> {
  USE_ARITHMETIC
  USE_ARITHMETIC_CAST
  USE_COPYABLE_ARITHMETIC

  ValType type;
  arithmetic::ArithmeticData data;

  // ArithmeticTypeBase & CopyableArithmeticTypeBase Implementation
  arithmetic::ArithmeticData& getArithmeticDataImpl() const {return const_cast<arithmetic::ArithmeticData&>(data);}
  ValType getValTypeImpl() const {return type;}
  priv::OptionalLockPlaceholder getLock(priv::MtxLockType lock_type) const noexcept {return priv::OptionalLockPlaceholder(nullptr, lock_type);}
  bool checkLock(const priv::OptionalLockPlaceholder&) const noexcept {return true;}

  // CastableArithmeticTypeBase implementation
  void reallocateImpl([[maybe_unused]] ValType type) noexcept {}
  void setTypeImpl(ValType new_type) noexcept {type = new_type;}

  friend class Variable;
  friend class Number;
  friend class GenericValueRef;
  GenericValue(ValType type, arithmetic::ArithmeticData data) : type(type), data(data) {}

  priv::ResourceData toResourceData() const noexcept {return {toVarType(type), reinterpret_cast<const priv::ResourceData::Data&>(data)};}

public:
  using arithmetic::ArithmeticTypeBase<GenericValue>::operator=;
  GenericValue() noexcept {}
  GenericValue(bool value) noexcept : type(ValType::boolean), data{.bool_val = value} {getLock(priv::MtxLockType::shared_locked);}
  GenericValue(ui8 value) noexcept : type(ValType::ui8), data{.ui8_val = value} {}
  GenericValue(i8 value) noexcept : type(ValType::si8), data{.i8_val = value} {}
  GenericValue(ui16 value) noexcept : type(ValType::ui16), data{.ui16_val = value} {}
  GenericValue(i16 value) noexcept : type(ValType::si16), data{.i16_val = value} {}
  GenericValue(ui32 value) noexcept : type(ValType::ui32), data{.ui32_val = value} {}
  GenericValue(i32 value) noexcept : type(ValType::si32), data{.i32_val = value} {}
  GenericValue(f32 value) noexcept : type(ValType::f32), data{.f32_val = value} {}
  GenericValue(ui64 value) noexcept : type(ValType::ui64), data{.ui64_val = value} {}
  GenericValue(i64 value) noexcept : type(ValType::si64), data{.i64_val = value} {}
  GenericValue(f64 value) noexcept : type(ValType::f64), data{.f64_val = value} {}
  GenericValue(const GenericValue& other) : type(other.type), data{.ui64_val = other.data.ui64_val} {}
  GenericValue(GenericValue&& other) : type(other.type), data{.ui64_val = other.data.ui64_val} {}
};

class GenericValueRef :
    public arithmetic::ArithmeticTypeBase<GenericValueRef> {
  USE_ARITHMETIC

  Variable* owner;
  ValType value_type;
  union pointer {
      void* ptr;
      bool* bool_ptr;
      ui8* ui8_ptr;
      ui16* ui16_ptr;
      ui32* ui32_ptr;
      ui64* ui64_ptr;
      i8* i8_ptr;
      i16* i16_ptr;
      i32* i32_ptr;
      i64* i64_ptr;
      f32* f32_ptr;
      f64* f64_ptr;
      arithmetic::ArithmeticData* num_data_ptr;
      pointer(void* ptr) : ptr(ptr) {}
  } ptr;

  arithmetic::ArithmeticData& getArithmeticDataImpl() const noexcept {return *ptr.num_data_ptr;}
  ValType getValTypeImpl() const noexcept {return value_type;}
  static bool checkLock(const OptionalSharedRecursiveLock& lock) noexcept;

  friend class GenericValueIterator;
  friend class Number;
  friend class BufferArray;

  GenericValueRef(ValType value_type, void* ptr, Variable* owner)
    : owner(owner) ,value_type(value_type), ptr(ptr) {}
public:
  using arithmetic::ArithmeticTypeBase<GenericValueRef>::operator=;
  GenericValueRef(const GenericValueRef& other) : value_type(other.value_type), ptr(other.ptr.ptr) {}
  GenericValueRef(GenericValueRef&& other) : value_type(other.value_type), ptr(other.ptr.ptr) {}
  GenericValueRef(const GenericValueIterator& it);
  GenericValueRef(GenericValueIterator&& it);

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept;

  operator GenericValue() {
    if(auto lk = getLock(MtxLockType::shared_locked); lk)
      return GenericValue(value_type, *ptr.num_data_ptr);
    return GenericValue();
  }

};

class GenericValueIterator {

};

}

#endif // GENERIC_VALUE_HXX
