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

  friend class binom::Variable;
  friend class binom::Number;
  friend class binom::GenericValueRef;
  friend class binom::KeyValue;
  GenericValue(ValType type, arithmetic::ArithmeticData data) : type(type), data(data) {}

  priv::ResourceData toResourceData() const noexcept {return {toVarType(type), reinterpret_cast<const priv::ResourceData::Data&>(data)};}

public:
  GenericValue() noexcept {}
  GenericValue(bool value) noexcept : type(ValType::boolean), data{.bool_val = value} {getLock(MtxLockType::shared_locked);}
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

  priv::Link link;
  size_t index = 0;

  ValType getValTypeImpl() const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk)
      return ValType::invalid_type;
    return toValueType(link->type);
  }

  arithmetic::ArithmeticData& getArithmeticDataImpl() noexcept {
    switch (toTypeClass(link->type)) {
    default:
    case binom::VarTypeClass::null:
    case binom::VarTypeClass::number: return reinterpret_cast<arithmetic::ArithmeticData&>(link->data);
    case binom::VarTypeClass::buffer_array:
    return *reinterpret_cast<arithmetic::ArithmeticData*>(link->data.buffer_array_header->get(getBitWidth(), index));
    }
  }

  const arithmetic::ArithmeticData& getArithmeticDataImpl() const noexcept {
    switch (toTypeClass(link->type)) {
    default:
    case binom::VarTypeClass::null:
    case binom::VarTypeClass::number: return reinterpret_cast<const arithmetic::ArithmeticData&>(link->data);
    case binom::VarTypeClass::buffer_array:
    return *reinterpret_cast<const arithmetic::ArithmeticData*>(link->data.buffer_array_header->get(getBitWidth(), index));
    }
  }

  static bool checkLock(const OptionalSharedRecursiveLock& lock) noexcept {return lock.has_value();}

  friend class GenericValueIterator;
  friend class ReverseGenericValueIterator;
  friend class Number;
  friend class BufferArray;

  GenericValueRef(priv::Link link, size_t index)
    : link(std::move(link)), index(index) {}
public:
  GenericValueRef(const GenericValueRef& other) : link(other.link), index(other.index) {}
  GenericValueRef(const GenericValueRef&& other) : link(other.link), index(other.index) {}

  using arithmetic::ArithmeticTypeBase<GenericValueRef>::operator=;

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept { return link.getLock(lock_type); }

  operator GenericValue() const { return GenericValue(getValTypeImpl(), getArithmeticDataImpl()); }

};

class GenericValueIterator {
  priv::Link link;
  ptrdiff_t index;

  friend class BufferArray;
  GenericValueIterator(priv::Link link, ptrdiff_t index)
    : link(std::move(link)), index(index) {}
public:
  GenericValueIterator(const GenericValueIterator& other)
    : link(other.link), index(other.index) {}
  GenericValueIterator(const GenericValueIterator&& other)
    : link(other.link), index(other.index) {}

  GenericValueIterator& operator=(GenericValueIterator&& other) noexcept {
    return *new(this) GenericValueIterator(other);
  }

  GenericValueIterator& operator++() noexcept {
    ++index;
    return self;
  }

  GenericValueIterator& operator--() noexcept {
    --index;
    return self;
  }

  GenericValueIterator operator++(int) noexcept {
    GenericValueIterator tmp(*this);
    ++index;
    return tmp;
  }

  GenericValueIterator operator--(int) noexcept {
    GenericValueIterator tmp(*this);
    --index;
    return tmp;
  }

  GenericValueIterator& operator+=(ptrdiff_t shift) noexcept {
    index += shift;
    return self;
  }

  GenericValueIterator& operator-=(ptrdiff_t shift) noexcept {
    index -= shift;
    return self;
  }

  bool operator==(GenericValueIterator&& other) const noexcept {return index == other.index;}
  bool operator!=(GenericValueIterator&& other) const noexcept {return index != other.index;}
  bool operator==(const GenericValueIterator& other) const noexcept {return index == other.index;}
  bool operator!=(const GenericValueIterator& other) const noexcept {return index != other.index;}

  GenericValueRef operator*() const noexcept {return GenericValueRef(link, index);}

};

class ReverseGenericValueIterator {
  friend class GenericValueIterator;
  priv::Link link;
  ptrdiff_t index;

  friend class BufferArray;
  ReverseGenericValueIterator(priv::Link link, ptrdiff_t index)
    : link(std::move(link)), index(index) {}
public:
  ReverseGenericValueIterator(const ReverseGenericValueIterator& other)
    : link(other.link), index(other.index) {}
  ReverseGenericValueIterator(ReverseGenericValueIterator&& other)
    : link(other.link), index(other.index) {}
  ReverseGenericValueIterator& operator=(ReverseGenericValueIterator&& other) noexcept {
    return *new(this) ReverseGenericValueIterator(other);
  }

  ReverseGenericValueIterator& operator++() noexcept {
    --index;
    return self;
  }

  ReverseGenericValueIterator& operator--() noexcept {
    ++index;
    return self;
  }

  ReverseGenericValueIterator operator++(int) noexcept {
    ReverseGenericValueIterator tmp(*this);
    --index;
    return tmp;
  }

  ReverseGenericValueIterator operator--(int) noexcept {
    ReverseGenericValueIterator tmp(*this);
    ++index;
    return tmp;
  }

  ReverseGenericValueIterator& operator+=(ptrdiff_t shift) noexcept {
    index -= shift;
    return self;
  }

  ReverseGenericValueIterator& operator-=(ptrdiff_t shift) noexcept {
    index += shift;
    return self;
  }

  bool operator==(ReverseGenericValueIterator&& other) const noexcept {return index == other.index;}
  bool operator!=(ReverseGenericValueIterator&& other) const noexcept {return index != other.index;}
  bool operator==(const ReverseGenericValueIterator& other) const noexcept {return index == other.index;}
  bool operator!=(const ReverseGenericValueIterator& other) const noexcept {return index != other.index;}

  GenericValueRef operator*() const noexcept {return GenericValueRef(link, index);}

};

}

#endif // GENERIC_VALUE_HXX
