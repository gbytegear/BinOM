#ifndef GENERIC_VALUE_HXX
#define GENERIC_VALUE_HXX

#include "../utils/generic_arithmetic.hxx"
#include "../binom_impl/resource_control.hxx"
#include <new>

namespace binom {

class GenericValueIterator;
class ReverseGenericValueIterator;
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
  arithmetic::ArithmeticData& getArithmeticDataImpl() const;
  ValType getValTypeImpl() const;

  // CastableArithmeticTypeBase implementation
  void reallocateImpl([[maybe_unused]] ValType type) noexcept;
  void setTypeImpl(ValType new_type) noexcept;

  friend class binom::Variable;
  friend class binom::Number;
  friend class binom::GenericValueRef;
  friend class binom::KeyValue;
  GenericValue(ValType type, arithmetic::ArithmeticData data);

  priv::ResourceData toResourceData() const noexcept;

public:
  GenericValue() noexcept;
  GenericValue(bool value) noexcept;
  GenericValue(ui8 value) noexcept;
  GenericValue(i8 value) noexcept;
  GenericValue(ui16 value) noexcept;
  GenericValue(i16 value) noexcept;
  GenericValue(ui32 value) noexcept;
  GenericValue(i32 value) noexcept;
  GenericValue(f32 value) noexcept;
  GenericValue(ui64 value) noexcept;
  GenericValue(i64 value) noexcept;
  GenericValue(f64 value) noexcept;
  GenericValue(const GenericValue& other);
  GenericValue(const GenericValue&& other);
  using arithmetic::ArithmeticTypeBase<GenericValue>::operator=;
};

class GenericValueRef :
    public arithmetic::ArithmeticTypeBase<GenericValueRef> {
  USE_ARITHMETIC
  static constexpr size_t INVALID_INDEX = 0xFFFFFFFFFFFFFFFF;

  priv::Link link;
  size_t index = 0;

  ValType getValTypeImpl() const noexcept;

  arithmetic::ArithmeticData& getArithmeticDataImpl() noexcept;

  const arithmetic::ArithmeticData& getArithmeticDataImpl() const noexcept;

  static bool checkLock(const OptionalSharedRecursiveLock& lock) noexcept;

  friend class binom::GenericValueIterator;
  friend class binom::ReverseGenericValueIterator;
  friend class binom::Number;
  friend class binom::BufferArray;

  GenericValueRef(priv::Link link, size_t index = INVALID_INDEX);
public:
  GenericValueRef(const GenericValueRef& other);
  GenericValueRef(const GenericValueRef&& other);

  using arithmetic::ArithmeticTypeBase<GenericValueRef>::operator=;

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept;

  operator GenericValue() const;
};

class GenericValueIterator {
  static constexpr ssize_t INVALID_INDEX = 0xFFFFFFFFFFFFFFFF;

  priv::Link link;
  ssize_t index;

  friend class BufferArray;
  GenericValueIterator(priv::Link link, ssize_t index = INVALID_INDEX);
public:
  GenericValueIterator(const GenericValueIterator& other);
  GenericValueIterator(const GenericValueIterator&& other);

  GenericValueIterator& operator=(GenericValueIterator&& other) noexcept;
  GenericValueIterator& operator++() noexcept;
  GenericValueIterator& operator--() noexcept;
  GenericValueIterator operator++(int) noexcept;
  GenericValueIterator operator--(int) noexcept;
  GenericValueIterator& operator+=(ssize_t shift) noexcept;
  GenericValueIterator& operator-=(ssize_t shift) noexcept;

  bool operator==(GenericValueIterator&& other) const noexcept;
  bool operator!=(GenericValueIterator&& other) const noexcept;
  bool operator==(const GenericValueIterator& other) const noexcept;
  bool operator!=(const GenericValueIterator& other) const noexcept;

  GenericValueRef operator*() const noexcept;

};

class ReverseGenericValueIterator {
  friend class GenericValueIterator;
  static constexpr ssize_t INVALID_INDEX = 0xFFFFFFFFFFFFFFFF;
  priv::Link link;
  ssize_t index;

  friend class BufferArray;
  ReverseGenericValueIterator(priv::Link link, ssize_t index);
public:
  ReverseGenericValueIterator(const ReverseGenericValueIterator& other);
  ReverseGenericValueIterator(ReverseGenericValueIterator&& other);
  ReverseGenericValueIterator& operator=(ReverseGenericValueIterator&& other) noexcept;

  ReverseGenericValueIterator& operator++() noexcept;

  ReverseGenericValueIterator& operator--() noexcept;

  ReverseGenericValueIterator operator++(int) noexcept;

  ReverseGenericValueIterator operator--(int) noexcept;

  ReverseGenericValueIterator& operator+=(ssize_t shift) noexcept;

  ReverseGenericValueIterator& operator-=(ssize_t shift) noexcept;

  bool operator==(ReverseGenericValueIterator&& other) const noexcept;
  bool operator!=(ReverseGenericValueIterator&& other) const noexcept;
  bool operator==(const ReverseGenericValueIterator& other) const noexcept;
  bool operator!=(const ReverseGenericValueIterator& other) const noexcept;

  GenericValueRef operator*() const noexcept;

};

}

#endif // GENERIC_VALUE_HXX
