#ifndef GENERIC_VALUE_HPP
#define GENERIC_VALUE_HPP

#include "../utils/types.hxx"
#include <utility>

namespace binom {

class GenericValueIterator;
class GenericValueRef;

class GenericValue {
  ValType value_type = ValType::invalid_type;
  union Data {
      bool bool_val;
      ui8 ui8_val;
      ui16 ui16_val;
      ui32 ui32_val;
      ui64 ui64_val;
      i8 i8_val;
      i16 i16_val;
      i32 i32_val;
      i64 i64_val;
      f32 f32_val;
      f64 f64_val;
      Data(bool bool_val) : bool_val(bool_val) {}
      Data(ui8 ui8_val)   : ui8_val(ui8_val) {}
      Data(ui16 ui16_val) : ui16_val(ui16_val) {}
      Data(ui32 ui32_val) : ui32_val(ui32_val) {}
      Data(ui64 ui64_val) : ui64_val(ui64_val) {}
      Data(i8 i8_val)     : i8_val(i8_val) {}
      Data(i16 i16_val)   : i16_val(i16_val) {}
      Data(i32 i32_val)   : i32_val(i32_val) {}
      Data(i64 i64_val)   : i64_val(i64_val) {}
      Data(f32 f32_val)   : f32_val(f32_val) {}
      Data(f64 f64_val)   : f64_val(f64_val) {}
    } data = false;
  friend class GenericValueIterator;
  friend class Number;
  friend class BufferArray;
public:
  GenericValue() noexcept;;
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
  GenericValue(GenericValue&& other);

  ValType getType() const noexcept;
  VarNumberType getNumberType() const noexcept;
  VarBitWidth getBitWidth() const noexcept;

  operator bool () const noexcept;
  operator ui8 () const noexcept;
  operator i8 () const noexcept;
  operator ui16 () const noexcept;
  operator i16 () const noexcept;
  operator ui32 () const noexcept;
  operator i32 () const noexcept;
  operator f32 () const noexcept;
  operator ui64 () const noexcept;
  operator i64 () const noexcept;
  operator f64 () const noexcept;

  bool operator==(GenericValue other) const noexcept;
  bool operator!=(GenericValue other) const noexcept;
  bool operator>(GenericValue other) const noexcept;
  bool operator>=(GenericValue other) const noexcept;
  bool operator<(GenericValue other) const noexcept;
  bool operator<=(GenericValue other) const noexcept;

  GenericValue& operator=(bool value) noexcept;
  GenericValue& operator=(ui8 value) noexcept;
  GenericValue& operator=(i8 value) noexcept;
  GenericValue& operator=(ui16 value) noexcept;
  GenericValue& operator=(i16 value) noexcept;
  GenericValue& operator=(ui32 value) noexcept;
  GenericValue& operator=(i32 value) noexcept;
  GenericValue& operator=(f32 value) noexcept;
  GenericValue& operator=(ui64 value) noexcept;
  GenericValue& operator=(i64 value) noexcept;
  GenericValue& operator=(f64 value) noexcept;
  GenericValue& operator=(const GenericValue& value) noexcept;
  GenericValue& operator=(GenericValue&& value) noexcept;

  GenericValue& operator+=(GenericValue value) noexcept;
  GenericValue& operator-=(GenericValue value) noexcept;
  GenericValue& operator*=(GenericValue value) noexcept;
  GenericValue& operator/=(GenericValue value) noexcept;
  GenericValue& operator%=(GenericValue value) noexcept;

  GenericValue operator+(GenericValue value) const noexcept;
  GenericValue operator-(GenericValue value) const noexcept;
  GenericValue operator*(GenericValue value) const noexcept;
  GenericValue operator/(GenericValue value) const noexcept;
  GenericValue operator%(GenericValue value) const noexcept;

};

class GenericValueRef {
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
      pointer(void* ptr) : ptr(ptr) {}
    } ptr;
  friend class GenericValueIterator;
  friend class Number;
  friend class BufferArray;
  GenericValueRef(ValType value_type, void* ptr)
    : value_type(value_type), ptr(ptr) {}
  GenericValueRef(VarType variable_type, void* ptr)
    : value_type(getValueType(variable_type)), ptr(ptr) {}
public:
  GenericValueRef(const GenericValueRef& other) : value_type(other.value_type), ptr(other.ptr.ptr) {}
  GenericValueRef(const GenericValueRef&& other) : value_type(other.value_type), ptr(other.ptr.ptr) {}
  GenericValueRef(const GenericValueIterator& it);
  GenericValueRef(const GenericValueIterator&& it);


};

class GenericValueIterator {

};


}

#endif // GENERIC_VALUE_HPP
