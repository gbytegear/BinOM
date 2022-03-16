#include "libbinom/include/variables/generic_value.hxx"

using namespace binom;


GenericValue::GenericValue() noexcept {}

GenericValue::GenericValue(bool value) noexcept : value_type(ValType::boolean), data(value) {}

GenericValue::GenericValue(ui8 value) noexcept : value_type(ValType::ui8), data(value) {}

GenericValue::GenericValue(i8 value) noexcept : value_type(ValType::si8), data(value) {}

GenericValue::GenericValue(ui16 value) noexcept : value_type(ValType::ui16), data(value) {}

GenericValue::GenericValue(i16 value) noexcept : value_type(ValType::si16), data(value) {}

GenericValue::GenericValue(ui32 value) noexcept : value_type(ValType::ui32), data(value) {}

GenericValue::GenericValue(i32 value) noexcept : value_type(ValType::si32), data(value) {}

GenericValue::GenericValue(f32 value) noexcept : value_type(ValType::f32), data(value) {}

GenericValue::GenericValue(ui64 value) noexcept : value_type(ValType::ui64), data(value) {}

GenericValue::GenericValue(i64 value) noexcept : value_type(ValType::si64), data(value) {}

GenericValue::GenericValue(f64 value) noexcept : value_type(ValType::f64), data(value) {}

GenericValue::GenericValue(const GenericValue& other) : value_type(other.value_type), data(other.data.ui64_val) {}

GenericValue::GenericValue(GenericValue&& other) : value_type(other.value_type), data(other.data.ui64_val) {}

ValType GenericValue::getType() const noexcept {return value_type;}

VarNumberType GenericValue::getNumberType() const noexcept {return binom::getNumberType(value_type);}

VarBitWidth GenericValue::getBitWidth() const noexcept {return binom::getBitWidth(value_type);}

binom::GenericValue::operator bool() const noexcept {
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

binom::GenericValue::operator ui8() const noexcept {
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

binom::GenericValue::operator i8() const noexcept {
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

binom::GenericValue::operator ui16() const noexcept {
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

binom::GenericValue::operator i16() const noexcept {
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

binom::GenericValue::operator ui32() const noexcept {
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

binom::GenericValue::operator i32() const noexcept {
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

binom::GenericValue::operator f32() const noexcept {
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

binom::GenericValue::operator ui64() const noexcept {
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

binom::GenericValue::operator i64() const noexcept {
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

binom::GenericValue::operator f64() const noexcept {
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

bool GenericValue::operator==(GenericValue other) const noexcept {
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

bool GenericValue::operator!=(GenericValue other) const noexcept {return !(*this == other);}

bool GenericValue::operator>(GenericValue other) const noexcept {
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

bool GenericValue::operator>=(GenericValue other) const noexcept {
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

bool GenericValue::operator<(GenericValue other) const noexcept {
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

bool GenericValue::operator<=(GenericValue other) const noexcept {
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

GenericValue& GenericValue::operator=(bool value) noexcept  {return *new(this) GenericValue(value);}

GenericValue& GenericValue::operator=(ui8 value) noexcept   {return *new(this) GenericValue(value);}

GenericValue& GenericValue::operator=(i8 value) noexcept    {return *new(this) GenericValue(value);}

GenericValue& GenericValue::operator=(ui16 value) noexcept  {return *new(this) GenericValue(value);}

GenericValue& GenericValue::operator=(i16 value) noexcept   {return *new(this) GenericValue(value);}

GenericValue& GenericValue::operator=(ui32 value) noexcept  {return *new(this) GenericValue(value);}

GenericValue& GenericValue::operator=(i32 value) noexcept   {return *new(this) GenericValue(value);}

GenericValue& GenericValue::operator=(f32 value) noexcept   {return *new(this) GenericValue(value);}

GenericValue& GenericValue::operator=(ui64 value) noexcept  {return *new(this) GenericValue(value);}

GenericValue& GenericValue::operator=(i64 value) noexcept   {return *new(this) GenericValue(value);}

GenericValue& GenericValue::operator=(f64 value) noexcept   {return *new(this) GenericValue(value);}

GenericValue& GenericValue::operator=(const GenericValue& value) noexcept {return *new(this) GenericValue(value);}

GenericValue& GenericValue::operator=(GenericValue&& value) noexcept      {return *new(this) GenericValue(value);}

GenericValue& GenericValue::operator+=(GenericValue value) noexcept {
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

GenericValue& GenericValue::operator-=(GenericValue value) noexcept {
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

GenericValue& GenericValue::operator*=(GenericValue value) noexcept {
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

GenericValue& GenericValue::operator/=(GenericValue value) noexcept {
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

GenericValue& GenericValue::operator%=(GenericValue value) noexcept {
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

GenericValue& GenericValue::operator++() noexcept {
  switch (value_type) {
    case binom::ValType::boolean:
    case binom::ValType::ui8: ++data.ui8_val; break;
    case binom::ValType::si8: ++data.i8_val; break;
    case binom::ValType::ui16: ++data.ui16_val; break;
    case binom::ValType::si16: ++data.i16_val; break;
    case binom::ValType::ui32: ++data.ui32_val; break;
    case binom::ValType::si32: ++data.i32_val; break;
    case binom::ValType::f32: ++data.f32_val; break;
    case binom::ValType::ui64: ++data.ui64_val; break;
    case binom::ValType::si64: ++data.i64_val; break;
    case binom::ValType::f64: ++data.f64_val; break;
    case binom::ValType::invalid_type:
    default: ++data.ui64_val; break;
  }
  return *this;
}

GenericValue& GenericValue::operator--() noexcept {
  switch (value_type) {
    case binom::ValType::boolean:
    case binom::ValType::ui8: --data.ui8_val; break;
    case binom::ValType::si8: --data.i8_val; break;
    case binom::ValType::ui16: --data.ui16_val; break;
    case binom::ValType::si16: --data.i16_val; break;
    case binom::ValType::ui32: --data.ui32_val; break;
    case binom::ValType::si32: --data.i32_val; break;
    case binom::ValType::f32: --data.f32_val; break;
    case binom::ValType::ui64: --data.ui64_val; break;
    case binom::ValType::si64: --data.i64_val; break;
    case binom::ValType::f64: --data.f64_val; break;
    case binom::ValType::invalid_type:
    default: --data.ui64_val; break;
  }
  return *this;
}

GenericValue GenericValue::operator+(GenericValue value) const noexcept {return GenericValue(*this) += value;}

GenericValue GenericValue::operator-(GenericValue value) const noexcept {return GenericValue(*this) -= value;}

GenericValue GenericValue::operator*(GenericValue value) const noexcept {return GenericValue(*this) *= value;}

GenericValue GenericValue::operator/(GenericValue value) const noexcept {return GenericValue(*this) /= value;}

GenericValue GenericValue::operator%(GenericValue value) const noexcept {return GenericValue(*this) %= value;}

GenericValue GenericValue::operator++(int) noexcept {
  GenericValue tmp(*this);
  switch (value_type) {
    case binom::ValType::boolean:
    case binom::ValType::ui8: ++data.ui8_val; break;
    case binom::ValType::si8: ++data.i8_val; break;
    case binom::ValType::ui16: ++data.ui16_val; break;
    case binom::ValType::si16: ++data.i16_val; break;
    case binom::ValType::ui32: ++data.ui32_val; break;
    case binom::ValType::si32: ++data.i32_val; break;
    case binom::ValType::f32: ++data.f32_val; break;
    case binom::ValType::ui64: ++data.ui64_val; break;
    case binom::ValType::si64: ++data.i64_val; break;
    case binom::ValType::f64: ++data.f64_val; break;
    case binom::ValType::invalid_type:
    default: ++data.ui64_val; break;
  }
  return tmp;
}

GenericValue GenericValue::operator--(int) noexcept {
  GenericValue tmp(*this);
  switch (value_type) {
    case binom::ValType::boolean:
    case binom::ValType::ui8: --data.ui8_val; break;
    case binom::ValType::si8: --data.i8_val; break;
    case binom::ValType::ui16: --data.ui16_val; break;
    case binom::ValType::si16: --data.i16_val; break;
    case binom::ValType::ui32: --data.ui32_val; break;
    case binom::ValType::si32: --data.i32_val; break;
    case binom::ValType::f32: --data.f32_val; break;
    case binom::ValType::ui64: --data.ui64_val; break;
    case binom::ValType::si64: --data.i64_val; break;
    case binom::ValType::f64: --data.f64_val; break;
    case binom::ValType::invalid_type:
    default: --data.ui64_val; break;
  }
  return tmp;
}
