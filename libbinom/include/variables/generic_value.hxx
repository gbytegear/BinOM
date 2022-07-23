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

  friend class Variable;
  friend class Number;
  friend class GenericValueRef;
  friend class KeyValue;

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
    public arithmetic::ArithmeticTypeBase<GenericValueRef>,
    arithmetic::ArithmeticMthrImplPlaceholders {
  USE_ARITHMETIC

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

  arithmetic::ArithmeticData& getArithmeticDataImpl() const noexcept;
  ValType getValTypeImpl() const noexcept;

  friend class GenericValueIterator;
  friend class ReverseGenericValueIterator;
  friend class Number;
  friend class BufferArray;
  friend class priv::BufferArrayImplementation;

  GenericValueRef(ValType value_type, void* ptr);
public:
  GenericValueRef(const GenericValueRef& other);
  GenericValueRef(const GenericValueRef&& other);

  using arithmetic::ArithmeticTypeBase<GenericValueRef>::operator=;

  operator GenericValue() const;
};



class GenericValueIterator {
  ValType value_type;
  mutable union pointer {
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
  friend class priv::BufferArrayImplementation;

  GenericValueIterator(ValType value_type, void* ptr);
public:
  GenericValueIterator(const GenericValueIterator& other);
  GenericValueIterator(const GenericValueIterator&& other);
  GenericValueIterator& operator=(GenericValueIterator&& other) noexcept;

  GenericValueIterator& operator++() noexcept;
  GenericValueIterator& operator--() noexcept;
  const GenericValueIterator& operator++() const noexcept;
  const GenericValueIterator& operator--() const noexcept;

  GenericValueIterator operator++(int) noexcept;
  GenericValueIterator operator--(int) noexcept;

  const GenericValueIterator operator++(int) const noexcept;
  const GenericValueIterator operator--(int) const noexcept;

  GenericValueIterator& operator+=(ssize_t shift) noexcept;
  GenericValueIterator& operator-=(ssize_t shift) noexcept;

  const GenericValueIterator& operator+=(ssize_t shift) const noexcept;
  const GenericValueIterator& operator-=(ssize_t shift) const noexcept;

  bool operator==(GenericValueIterator&& other) const noexcept;
  bool operator!=(GenericValueIterator&& other) const noexcept;
  bool operator==(const GenericValueIterator& other) const noexcept;
  bool operator!=(const GenericValueIterator& other) const noexcept;

  GenericValueRef operator*() noexcept;
  const GenericValueRef operator*() const noexcept;

};

class ReverseGenericValueIterator {
  friend class GenericValueIterator;
  ValType value_type;
  mutable union pointer {
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
  friend class priv::BufferArrayImplementation;

  ReverseGenericValueIterator(ValType value_type, void* ptr);
public:
  ReverseGenericValueIterator(const ReverseGenericValueIterator& other);
  ReverseGenericValueIterator(ReverseGenericValueIterator&& other);
  ReverseGenericValueIterator& operator=(ReverseGenericValueIterator&& other) noexcept;

  ReverseGenericValueIterator& operator++() noexcept;
  ReverseGenericValueIterator& operator--() noexcept;

  const ReverseGenericValueIterator& operator++() const noexcept;
  const ReverseGenericValueIterator& operator--() const noexcept;

  ReverseGenericValueIterator operator++(int) noexcept;
  ReverseGenericValueIterator operator--(int) noexcept;

  const ReverseGenericValueIterator operator++(int) const noexcept;
  const ReverseGenericValueIterator operator--(int) const noexcept;

  ReverseGenericValueIterator& operator+=(ptrdiff_t shift) noexcept;
  ReverseGenericValueIterator& operator-=(ptrdiff_t shift) noexcept;

  const ReverseGenericValueIterator& operator+=(ptrdiff_t shift) const noexcept;
  const ReverseGenericValueIterator& operator-=(ptrdiff_t shift) const noexcept;

  bool operator==(ReverseGenericValueIterator&& other) const noexcept;
  bool operator!=(ReverseGenericValueIterator&& other) const noexcept;

  GenericValueRef operator*() noexcept;
  const GenericValueRef operator*() const noexcept;

};

}

#endif // GENERIC_VALUE_HXX
