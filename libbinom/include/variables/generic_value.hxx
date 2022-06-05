#ifndef GENERIC_VALUE_HXX
#define GENERIC_VALUE_HXX

#include "../utils/generic_arithmetic.hxx"
#include "../utils/resource_control.hxx"
#include <new>

namespace binom {

class GenericValueIterator;
class GenericValueRef;

class GenericValue :
    public arithmetic::ArithmeticTypeBase<GenericValue>,
    public arithmetic::CastableArithmeticTypeBase<GenericValue>,
    arithmetic::EnableCopyableArithmetic,
    arithmetic::ArithmeticMthrImplPlaceholders {
  USE_ARITHMETIC
  USE_ARITHMETIC_CAST

  ValType type;
  arithmetic::ArithmeticData data;

  // ArithmeticTypeBase & CopyableArithmeticTypeBase Implementation
  arithmetic::ArithmeticData& getArithmeticDataImpl() const {return const_cast<arithmetic::ArithmeticData&>(data);}
  ValType getValTypeImpl() const {return type;}

  // CastableArithmeticTypeBase implementation
  void reallocateImpl([[maybe_unused]] ValType type) noexcept {}
  void setTypeImpl(ValType new_type) noexcept {type = new_type;}

  friend class Variable;
  friend class Number;
  friend class GenericValueRef;
  GenericValue(ValType type, arithmetic::ArithmeticData data) : type(type), data(data) {}

  priv::ResourceData toResourceData() const noexcept {return {toVarType(type), reinterpret_cast<const priv::ResourceData::Data&>(data)};}

public:
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
  GenericValue(const GenericValue&& other) : type(other.type), data{.ui64_val = other.data.ui64_val} {}
  using arithmetic::ArithmeticTypeBase<GenericValue>::operator=;
};

class GenericValueRef :
    public arithmetic::ArithmeticTypeBase<GenericValueRef> {
  USE_ARITHMETIC

  priv::Link owner;
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
  ValType getValTypeImpl() const noexcept {if(ptr.ptr) return value_type; else return ValType::invalid_type;}
  static bool checkLock(const OptionalSharedRecursiveLock& lock) noexcept {return lock.has_value();}

  friend class GenericValueIterator;
  friend class ReverseGenericValueIterator;
  friend class Number;
  friend class BufferArray;

  GenericValueRef(ValType value_type, void* ptr, priv::Link owner)
    : owner(std::move(owner)), value_type(value_type), ptr(ptr) {}
public:
  GenericValueRef(const GenericValueRef& other) : owner(other.owner), value_type(other.value_type), ptr(other.ptr.ptr) {}
  GenericValueRef(const GenericValueRef&& other) : owner(std::move(other.owner)), value_type(other.value_type), ptr(other.ptr.ptr) {}

  using arithmetic::ArithmeticTypeBase<GenericValueRef>::operator=;

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept { return owner.getLock(lock_type); }

  operator GenericValue() {
    if(auto lk = getLock(MtxLockType::shared_locked); lk)
      return GenericValue(value_type, *ptr.num_data_ptr);
    return GenericValue();
  }

};

class GenericValueIterator {
  priv::Link owner;
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

  friend class BufferArray;
  GenericValueIterator(ValType value_type, void* ptr, priv::Link owner)
    : owner(std::move(owner)), value_type(value_type), ptr(ptr) {}
public:
  GenericValueIterator(const GenericValueIterator& other)
    : owner(other.owner), value_type(other.value_type), ptr(other.ptr.ptr) {}
  GenericValueIterator(const GenericValueIterator&& other)
    : owner(std::move(other.owner)), value_type(other.value_type), ptr(other.ptr.ptr) {}
  GenericValueIterator& operator=(GenericValueIterator&& other) noexcept {
    return *new(this) GenericValueIterator(other);
  }

  GenericValueIterator& operator++() noexcept {
    switch (toBitWidth(value_type)) {
    case binom::VarBitWidth::byte: ++ptr.ui8_ptr; break;
    case binom::VarBitWidth::word: ++ptr.ui16_ptr; break;
    case binom::VarBitWidth::dword: ++ptr.ui32_ptr; break;
    case binom::VarBitWidth::qword: ++ptr.ui64_ptr; break;
    case binom::VarBitWidth::invalid_type: break;
    }
    return self;
  }

  GenericValueIterator& operator--() noexcept {
    switch (toBitWidth(value_type)) {
    case binom::VarBitWidth::byte: --ptr.ui8_ptr; break;
    case binom::VarBitWidth::word: --ptr.ui16_ptr; break;
    case binom::VarBitWidth::dword: --ptr.ui32_ptr; break;
    case binom::VarBitWidth::qword: --ptr.ui64_ptr; break;
    case binom::VarBitWidth::invalid_type: break;
    }
    return self;
  }

  GenericValueIterator operator++(int) noexcept {
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

  GenericValueIterator operator--(int) noexcept {
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

  GenericValueIterator& operator+=(ptrdiff_t shift) noexcept {
    switch (toBitWidth(value_type)) {
    case binom::VarBitWidth::byte: ptr.ui8_ptr += shift; break;
    case binom::VarBitWidth::word: ptr.ui16_ptr += shift; break;
    case binom::VarBitWidth::dword: ptr.ui32_ptr += shift; break;
    case binom::VarBitWidth::qword: ptr.ui64_ptr += shift; break;
    case binom::VarBitWidth::invalid_type: break;
    }
    return self;
  }

  GenericValueIterator& operator-=(ptrdiff_t shift) noexcept {
    switch (toBitWidth(value_type)) {
    case binom::VarBitWidth::byte: ptr.ui8_ptr -= shift; break;
    case binom::VarBitWidth::word: ptr.ui16_ptr -= shift; break;
    case binom::VarBitWidth::dword: ptr.ui32_ptr -= shift; break;
    case binom::VarBitWidth::qword: ptr.ui64_ptr -= shift; break;
    case binom::VarBitWidth::invalid_type: break;
    }
    return self;
  }

  bool operator==(GenericValueIterator&& other) const noexcept {return ptr.ptr == other.ptr.ptr;}
  bool operator!=(GenericValueIterator&& other) const noexcept {return ptr.ptr != other.ptr.ptr;}
  bool operator==(const GenericValueIterator& other) const noexcept {return ptr.ptr == other.ptr.ptr;}
  bool operator!=(const GenericValueIterator& other) const noexcept {return ptr.ptr != other.ptr.ptr;}

  GenericValueRef operator*() const noexcept {return GenericValueRef(value_type, ptr.ptr, owner);}

};

class ReverseGenericValueIterator {
  friend class GenericValueIterator;
  priv::Link owner;
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

  friend class BufferArray;
  ReverseGenericValueIterator(ValType value_type, void* ptr, priv::Link owner)
    : owner(std::move(owner)), value_type(value_type), ptr(ptr) {}
public:
  ReverseGenericValueIterator(const ReverseGenericValueIterator& other)
    : owner(other.owner), value_type(other.value_type), ptr(other.ptr.ptr) {}
  ReverseGenericValueIterator(ReverseGenericValueIterator&& other)
    : owner(std::move(other.owner)), value_type(other.value_type), ptr(other.ptr.ptr) {}
  ReverseGenericValueIterator& operator=(ReverseGenericValueIterator&& other) noexcept {
    return *new(this) ReverseGenericValueIterator(other);
  }

  ReverseGenericValueIterator& operator++() noexcept {
    switch (toBitWidth(value_type)) {
    case binom::VarBitWidth::byte: --ptr.ui8_ptr; break;
    case binom::VarBitWidth::word: --ptr.ui16_ptr; break;
    case binom::VarBitWidth::dword: --ptr.ui32_ptr; break;
    case binom::VarBitWidth::qword: --ptr.ui64_ptr; break;
    case binom::VarBitWidth::invalid_type: break;
    }
    return self;
  }

  ReverseGenericValueIterator& operator--() noexcept {
    switch (toBitWidth(value_type)) {
    case binom::VarBitWidth::byte: ++ptr.ui8_ptr; break;
    case binom::VarBitWidth::word: ++ptr.ui16_ptr; break;
    case binom::VarBitWidth::dword: ++ptr.ui32_ptr; break;
    case binom::VarBitWidth::qword: ++ptr.ui64_ptr; break;
    case binom::VarBitWidth::invalid_type: break;
    }
    return self;
  }

  ReverseGenericValueIterator operator++(int) noexcept {
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

  ReverseGenericValueIterator operator--(int) noexcept {
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

  ReverseGenericValueIterator& operator+=(ptrdiff_t shift) noexcept {
    switch (toBitWidth(value_type)) {
    case binom::VarBitWidth::byte: ptr.ui8_ptr -= shift; break;
    case binom::VarBitWidth::word: ptr.ui16_ptr -= shift; break;
    case binom::VarBitWidth::dword: ptr.ui32_ptr -= shift; break;
    case binom::VarBitWidth::qword: ptr.ui64_ptr -= shift; break;
    case binom::VarBitWidth::invalid_type: break;
    }
    return self;
  }

  ReverseGenericValueIterator& operator-=(ptrdiff_t shift) noexcept {
    switch (toBitWidth(value_type)) {
    case binom::VarBitWidth::byte: ptr.ui8_ptr += shift; break;
    case binom::VarBitWidth::word: ptr.ui16_ptr += shift; break;
    case binom::VarBitWidth::dword: ptr.ui32_ptr += shift; break;
    case binom::VarBitWidth::qword: ptr.ui64_ptr += shift; break;
    case binom::VarBitWidth::invalid_type: break;
    }
    return self;
  }

  bool operator==(ReverseGenericValueIterator&& other) const noexcept {return ptr.ptr == other.ptr.ptr;}
  bool operator!=(ReverseGenericValueIterator&& other) const noexcept {return ptr.ptr != other.ptr.ptr;}

  GenericValueRef operator*() const noexcept {return GenericValueRef(value_type, ptr.ptr, owner);}

};

}

#endif // GENERIC_VALUE_HXX
