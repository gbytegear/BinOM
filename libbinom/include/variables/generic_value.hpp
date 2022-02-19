#ifndef GENERIC_VALUE_HPP
#define GENERIC_VALUE_HPP

#include "../utils/types.hpp"
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
  GenericValue() noexcept {};
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
  inline VarNumberType getNumberType() const noexcept {return binom::getNumberType(value_type);}
  inline VarBitWidth getBitWidth() const noexcept {return binom::getBitWidth(value_type);}

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

  bool operator==(GenericValue other) const noexcept {
    switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return ui64(*this) == ui64(other);
          case binom::VarNumberType::signed_integer: return i64(other) >= 0 ? ui64(*this) == ui64(other) : false;
          case binom::VarNumberType::float_point: return ui64(*this) == f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::signed_integer:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return i64(*this) >= 0 ? ui64(*this) == ui64(other) : false;
          case binom::VarNumberType::signed_integer: return i64(*this) == i64(other);
          case binom::VarNumberType::float_point: return i64(*this) == f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::float_point:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return f64(*this) == ui64(other);
          case binom::VarNumberType::signed_integer: return f64(*this) == ui64(other);
          case binom::VarNumberType::float_point: return f64(*this) == f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      default:
      case binom::VarNumberType::invalid_type: return false;
    }
  }

  bool operator!=(GenericValue other) const noexcept {return !(*this == other);}

  bool operator>(GenericValue other) const noexcept {
    switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return ui64(*this) > ui64(other);
          case binom::VarNumberType::signed_integer: return i64(other) >= 0 ? ui64(*this) > ui64(other) : true;
          case binom::VarNumberType::float_point: return ui64(*this) > f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::signed_integer:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return i64(*this) >= 0 ? ui64(*this) > ui64(other) : false;
          case binom::VarNumberType::signed_integer: return i64(*this) > i64(other);
          case binom::VarNumberType::float_point: return i64(*this) > f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::float_point:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return f64(*this) > ui64(other);
          case binom::VarNumberType::signed_integer: return f64(*this) > ui64(other);
          case binom::VarNumberType::float_point: return f64(*this) > f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      default:
      case binom::VarNumberType::invalid_type: return false;
    }
  }

  bool operator>=(GenericValue other) const noexcept {
    switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return ui64(*this) >= ui64(other);
          case binom::VarNumberType::signed_integer: return i64(other) >= 0 ? ui64(*this) >= ui64(other) : true;
          case binom::VarNumberType::float_point: return ui64(*this) >= f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::signed_integer:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return i64(*this) >= 0 ? ui64(*this) >= ui64(other) : false;
          case binom::VarNumberType::signed_integer: return i64(*this) >= i64(other);
          case binom::VarNumberType::float_point: return i64(*this) >= f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::float_point:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return f64(*this) >= ui64(other);
          case binom::VarNumberType::signed_integer: return f64(*this) >= ui64(other);
          case binom::VarNumberType::float_point: return f64(*this) >= f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      default:
      case binom::VarNumberType::invalid_type: return false;
    }
  }

  bool operator<(GenericValue other) const noexcept {
    switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return ui64(*this) < ui64(other);
          case binom::VarNumberType::signed_integer: return i64(other) >= 0 ? ui64(*this) < ui64(other) : false;
          case binom::VarNumberType::float_point: return ui64(*this) < f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::signed_integer:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return i64(*this) >= 0 ? ui64(*this) < ui64(other) : true;
          case binom::VarNumberType::signed_integer: return i64(*this) < i64(other);
          case binom::VarNumberType::float_point: return i64(*this) < f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::float_point:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return f64(*this) < ui64(other);
          case binom::VarNumberType::signed_integer: return f64(*this) < ui64(other);
          case binom::VarNumberType::float_point: return f64(*this) < f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      default:
      case binom::VarNumberType::invalid_type: return false;
    }
  }

  bool operator<=(GenericValue other) const noexcept {
    switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return ui64(*this) <= ui64(other);
          case binom::VarNumberType::signed_integer: return i64(other) >= 0 ? ui64(*this) <= ui64(other) : false;
          case binom::VarNumberType::float_point: return ui64(*this) <= f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::signed_integer:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return i64(*this) >= 0 ? ui64(*this) <= ui64(other) : true;
          case binom::VarNumberType::signed_integer: return i64(*this) <= i64(other);
          case binom::VarNumberType::float_point: return i64(*this) <= f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::float_point:
        switch (other.getNumberType()) {
          case binom::VarNumberType::unsigned_integer: return f64(*this) <= ui64(other);
          case binom::VarNumberType::signed_integer: return f64(*this) <= ui64(other);
          case binom::VarNumberType::float_point: return f64(*this) <= f64(other);
          default:
          case binom::VarNumberType::invalid_type: return false;
        }
      default:
      case binom::VarNumberType::invalid_type: return false;
    }
  }

  GenericValue& operator=(bool value) noexcept  {return *new(this) GenericValue(value);}
  GenericValue& operator=(ui8 value) noexcept   {return *new(this) GenericValue(value);}
  GenericValue& operator=(i8 value) noexcept    {return *new(this) GenericValue(value);}
  GenericValue& operator=(ui16 value) noexcept  {return *new(this) GenericValue(value);}
  GenericValue& operator=(i16 value) noexcept   {return *new(this) GenericValue(value);}
  GenericValue& operator=(ui32 value) noexcept  {return *new(this) GenericValue(value);}
  GenericValue& operator=(i32 value) noexcept   {return *new(this) GenericValue(value);}
  GenericValue& operator=(f32 value) noexcept   {return *new(this) GenericValue(value);}
  GenericValue& operator=(ui64 value) noexcept  {return *new(this) GenericValue(value);}
  GenericValue& operator=(i64 value) noexcept   {return *new(this) GenericValue(value);}
  GenericValue& operator=(f64 value) noexcept   {return *new(this) GenericValue(value);}
  GenericValue& operator=(const GenericValue& value) noexcept {return *new(this) GenericValue(value);}
  GenericValue& operator=(GenericValue&& value) noexcept      {return *new(this) GenericValue(value);}

  GenericValue& operator+=(GenericValue value) noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.bool_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.bool_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.bool_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui8_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui8_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui8_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i8_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i8_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i8_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui16_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui16_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui16_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i16_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i16_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i16_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui32_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui32_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui32_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i32_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i32_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i32_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.f32_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.f32_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.f32_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui64_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui64_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui64_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i64_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i64_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i64_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.f64_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.f64_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.f64_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      default:
      case binom::ValType::invalid_type:
      break;

    }
    return *this;
  }

  GenericValue& operator-=(GenericValue value) noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.bool_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.bool_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.bool_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui8_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui8_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui8_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i8_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i8_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i8_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui16_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui16_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui16_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i16_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i16_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i16_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui32_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui32_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui32_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i32_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i32_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i32_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.f32_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.f32_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.f32_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui64_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui64_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui64_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i64_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i64_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i64_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.f64_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.f64_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.f64_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      default:
      case binom::ValType::invalid_type:
      break;

    }
    return *this;
  }

  GenericValue& operator*=(GenericValue value) noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.bool_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.bool_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.bool_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui8_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui8_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui8_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i8_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i8_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i8_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui16_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui16_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui16_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i16_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i16_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i16_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui32_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui32_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui32_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i32_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i32_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i32_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.f32_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.f32_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.f32_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui64_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui64_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui64_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i64_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i64_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i64_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.f64_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.f64_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.f64_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      default:
      case binom::ValType::invalid_type:
      break;

    }
    return *this;
  }

  GenericValue& operator/=(GenericValue value) noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.bool_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.bool_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.bool_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui8_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui8_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui8_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i8_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i8_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i8_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui16_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui16_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui16_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i16_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i16_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i16_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui32_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui32_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui32_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i32_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i32_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i32_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.f32_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.f32_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.f32_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui64_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui64_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui64_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i64_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i64_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i64_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.f64_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.f64_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.f64_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      default:
      case binom::ValType::invalid_type:
      break;

    }
    return *this;
  }

  GenericValue& operator%=(GenericValue value) noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.bool_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.bool_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.bool_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui8_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui8_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui8_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i8_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i8_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i8_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui16_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui16_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui16_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i16_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i16_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i16_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui32_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui32_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui32_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i32_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i32_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i32_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.f32_val = i64(*this) % ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.f32_val = i64(*this) % i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.f32_val = i64(*this) % i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.ui64_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.ui64_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.ui64_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.i64_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.i64_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.i64_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            data.f64_val = i64(*this) % ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            data.f64_val = i64(*this) % i64(value);
          break;
          case binom::VarNumberType::float_point:
            data.f64_val = i64(*this) % i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      default:
      case binom::ValType::invalid_type:
      break;

    }
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
