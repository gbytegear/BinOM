#ifndef GENERIC_VALUE_HPP
#define GENERIC_VALUE_HPP

#include "../utils/types.hpp"

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
      Data(ui8 ui8_val) : ui8_val(ui8_val) {}
      Data(ui16 ui16_val) : ui16_val(ui16_val) {}
      Data(ui32 ui32_val) : ui32_val(ui32_val) {}
      Data(ui64 ui64_val) : ui64_val(ui64_val) {}
      Data(i8 i8_val) : i8_val(i8_val) {}
      Data(i16 i16_val) : i16_val(i16_val) {}
      Data(i32 i32_val) : i32_val(i32_val) {}
      Data(i64 i64_val) : i64_val(i64_val) {}
      Data(f32 f32_val) : f32_val(f32_val) {}
      Data(f64 f64_val) : f64_val(f64_val) {}
    } data = false;
  friend class GenericValueIterator;
  friend class Number;
  friend class BufferArray;
public:
  GenericValue() noexcept = default;
  GenericValue(bool value) noexcept : value_type(ValType::boolean), data(value) {}
  GenericValue(ui8 value) noexcept : value_type(ValType::ui8), data(value) {}
  GenericValue(i8 value) noexcept : value_type(ValType::si8), data(value) {}
  GenericValue(ui16 value) noexcept : value_type(ValType::ui16), data(value) {}
  GenericValue(i16 value) noexcept : value_type(ValType::si16), data(value) {}
  GenericValue(ui32 value) noexcept : value_type(ValType::ui32), data(value) {}
  GenericValue(i32 value) noexcept : value_type(ValType::si32), data(value) {}
  GenericValue(f32 value) noexcept : value_type(ValType::f32), data(value) {}
  GenericValue(ui64 value) noexcept : value_type(ValType::ui64), data(value) {}
  GenericValue(i64 value) noexcept : value_type(ValType::si64), data(value) {}
  GenericValue(f64 value) noexcept : value_type(ValType::f64), data(value) {}
  GenericValue(const GenericValue& other) : value_type(other.value_type), data(other.data.ui64_val) {}
  GenericValue(GenericValue&& other) : value_type(other.value_type), data(other.data.ui64_val) {}

  inline ValType getType() const noexcept {return value_type;}

  inline operator ValType () const noexcept {return value_type;}

  inline VarBitWidth getBitWidth() const noexcept {return binom::getBitWidth(value_type);}

  inline operator VarBitWidth () const noexcept {return binom::getBitWidth(value_type);}

  inline operator bool () const noexcept {
    switch (value_type) {
      case binom::ValType::boolean:
      case binom::ValType::ui8:
      case binom::ValType::si8: return data.bool_val;
      case binom::ValType::ui16: return data.ui16_val;
      case binom::ValType::si16: return data.i16_val;
      case binom::ValType::ui32: return data.ui32_val;
      case binom::ValType::si32: return data.i32_val;
      case binom::ValType::f32: return data.f32_val;
      case binom::ValType::ui64: return data.ui64_val;
      case binom::ValType::si64: return data.i64_val;
      case binom::ValType::f64: return data.f64_val;
      case binom::ValType::invalid_type:
      default: return data.ui64_val;
    }
  }

  inline operator ui8 () const noexcept {
    switch (value_type) {
      case binom::ValType::boolean: return data.ui8_val;
      case binom::ValType::ui8: return data.i8_val;
      case binom::ValType::si8: return data.ui8_val;
      case binom::ValType::ui16: return data.ui16_val;
      case binom::ValType::si16: return data.i16_val;
      case binom::ValType::ui32: return data.ui32_val;
      case binom::ValType::si32: return data.i32_val;
      case binom::ValType::f32: return data.f32_val;
      case binom::ValType::ui64: return data.ui64_val;
      case binom::ValType::si64: return data.i64_val;
      case binom::ValType::f64: return data.f64_val;
      case binom::ValType::invalid_type:
      default: return data.ui64_val;
    }
  }

  inline operator i8 () const noexcept {
    switch (value_type) {
      case binom::ValType::boolean: return data.i8_val;
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
      case binom::ValType::invalid_type:
      default: return data.ui64_val;
    }
  }

  inline operator ui16 () const noexcept {
    switch (value_type) {
      case binom::ValType::boolean: return data.ui8_val;
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
      case binom::ValType::invalid_type:
      default: return data.ui64_val;
    }
  }

  inline operator i16 () const noexcept {
    switch (value_type) {
      case binom::ValType::boolean:
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
      case binom::ValType::invalid_type:
      default: return data.ui64_val;
    }
  }

  inline operator ui32 () const noexcept {
    switch (value_type) {
      case binom::ValType::boolean: return data.ui8_val;
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
      case binom::ValType::invalid_type:
      default: return data.ui64_val;
    }
  }

  inline operator i32 () const noexcept {
    switch (value_type) {
      case binom::ValType::boolean:
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
      case binom::ValType::invalid_type:
      default: return data.ui64_val;
    }
  }

  inline operator f32 () const noexcept {
    switch (value_type) {
      case binom::ValType::boolean:
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
      case binom::ValType::invalid_type:
      default: return data.ui64_val;
    }
  }

  inline operator ui64 () const noexcept {
    switch (value_type) {
      case binom::ValType::boolean: return data.ui8_val;
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
      case binom::ValType::invalid_type:
      default: return data.ui64_val;
    }
  }

  inline operator i64 () const noexcept {
    switch (value_type) {
      case binom::ValType::boolean:
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
      case binom::ValType::invalid_type:
      default: return data.ui64_val;
    }
  }

  inline operator f64 () const noexcept {
    switch (value_type) {
      case binom::ValType::boolean:
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
      case binom::ValType::invalid_type:
      default: return data.ui64_val;
    }
  }

  bool operator==(const GenericValue& other) const noexcept {
    if(value_type != other.value_type) return false;
    switch (getBitWidth()) {
      case binom::VarBitWidth::byte: return data.ui8_val == other.data.ui8_val;
      case binom::VarBitWidth::word: return data.ui16_val == other.data.ui16_val;
      case binom::VarBitWidth::dword: return data.ui32_val == other.data.ui32_val;
      case binom::VarBitWidth::qword: return data.ui64_val == other.data.ui64_val;
      case binom::VarBitWidth::invalid_type: return data.ui64_val == other.data.ui64_val;
    }
  }

  bool operator!=(GenericValue other) const noexcept {
    if(value_type != other.value_type) return true;
    switch (getBitWidth()) {
      case binom::VarBitWidth::byte: return data.ui8_val != other.data.ui8_val;
      case binom::VarBitWidth::word: return data.ui16_val != other.data.ui16_val;
      case binom::VarBitWidth::dword: return data.ui32_val != other.data.ui32_val;
      case binom::VarBitWidth::qword: return data.ui64_val != other.data.ui64_val;
      case binom::VarBitWidth::invalid_type: return data.ui64_val != other.data.ui64_val;
    }
  }

  GenericValue& operator=(const GenericValue&& other) noexcept {
    value_type = other.value_type;
    data.ui64_val = other.data.ui64_val;
    return *this;
  }

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
