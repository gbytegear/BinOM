#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include "../utils/types.hxx"

namespace binom::arithmetic {

union ArithmeticData {
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
};

template <typename ArithmeticTypeDriven>
class ExtendedArithmeticTypeBase;

/* To implementation for driven
ArithmeticData& getArithmeticDataImpl() const;
void setArithmeticDataImpl(ValType type, ArithmeticData data);
ValType getTypeImpl() const;
 */

/* Boilerplate (change ArithmeticTypeDriven to name of driven class)
ArithmeticTypeDriven& operator=(bool value) noexcept {return ArithmeticTypeBase::operator=(value);}
ArithmeticTypeDriven& operator=(ui8 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
ArithmeticTypeDriven& operator=(i8 value) noexcept   {return ArithmeticTypeBase::operator=(value);}
ArithmeticTypeDriven& operator=(ui16 value) noexcept {return ArithmeticTypeBase::operator=(value);}
ArithmeticTypeDriven& operator=(i16 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
ArithmeticTypeDriven& operator=(ui32 value) noexcept {return ArithmeticTypeBase::operator=(value);}
ArithmeticTypeDriven& operator=(i32 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
ArithmeticTypeDriven& operator=(f32 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
ArithmeticTypeDriven& operator=(ui64 value) noexcept {return ArithmeticTypeBase::operator=(value);}
ArithmeticTypeDriven& operator=(i64 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
ArithmeticTypeDriven& operator=(f64 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
ArithmeticTypeDriven& operator=(const ArithmeticTypeDriven& value) noexcept {return ArithmeticTypeBase::operator=(value);}
ArithmeticTypeDriven& operator=(ArithmeticTypeDriven&& value) noexcept      {return ArithmeticTypeBase::operator=(std::move(value));}
*/


/**
 * @brief The ArithmeticTypeBase class
 */
template <typename ArithmeticTypeDriven>
class ArithmeticTypeBase {

  inline ArithmeticData& getArithmeticData() const {return reinterpret_cast<const ArithmeticTypeDriven*>(this)->getArithmeticDataImpl();}
  inline void setArithmeticData(ValType type, ArithmeticData data) {return reinterpret_cast<ArithmeticTypeDriven*>(this)->setArithmeticDataImpl(type, data);}
  inline ArithmeticTypeDriven& downcast() noexcept {return *reinterpret_cast<ArithmeticTypeDriven*>(this);}
  inline const ArithmeticTypeDriven& downcast() const noexcept {return *reinterpret_cast<const ArithmeticTypeDriven*>(this);}

  template <typename T>
  inline bool equalWithUnsigned(T value) const noexcept {
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) == value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) == value : false;
    case binom::VarNumberType::float_point: return f64(*this) == value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool notEqualWithUnsigned(T value) const noexcept {
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) != value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) != value : true;
    case binom::VarNumberType::float_point: return f64(*this) != value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool highterThenUnsigned(T value) const noexcept {
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) > value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) > value : false;
    case binom::VarNumberType::float_point: return f64(*this) > value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool highterOrEqualThenUnsigned(T value) const noexcept {
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) >= value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) >= value : false;
    case binom::VarNumberType::float_point: return f64(*this) >= value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool lowerThenUnsigned(T value) const noexcept {
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) < value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) < value : true;
    case binom::VarNumberType::float_point: return f64(*this) < value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool lowerOrEqualThenUnsigned(T value) const noexcept {
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) <= value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) <= value : true;
    case binom::VarNumberType::float_point: return f64(*this) <= value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool equalWithSigned(T value) const noexcept {
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) == value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) == value : false;
    case binom::VarNumberType::float_point: return f64(*this) == value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool notEqualWithSigned(T value) const noexcept {
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(*this) != value : true;
    case binom::VarNumberType::signed_integer: return i64(*this) != value;
    case binom::VarNumberType::float_point: return f64(*this) != value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool highterThenSigned(T value) const noexcept {
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(*this) > value : true;
    case binom::VarNumberType::signed_integer: return i64(*this) > value;
    case binom::VarNumberType::float_point: return f64(*this) > value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool highterOrEqualThenSigned(T value) const noexcept {
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(*this) >= value : true;
    case binom::VarNumberType::signed_integer: return i64(*this) >= value;
    case binom::VarNumberType::float_point: return f64(*this) >= value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool lowerThenSigned(T value) const noexcept {
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(*this) < value : false;
    case binom::VarNumberType::signed_integer: return i64(*this) < value;
    case binom::VarNumberType::float_point: return f64(*this) < value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool lowerOrEqualThenSigned(T value) const noexcept {
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(*this) <= value : false;
    case binom::VarNumberType::signed_integer: return i64(*this) <= value;
    case binom::VarNumberType::float_point: return f64(*this) <= value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

public:

  ValType getType() const noexcept {return reinterpret_cast<const ArithmeticTypeDriven*>(this)->getTypeImpl();}
  VarNumberType getNumberType() const noexcept {return binom::getNumberType(getType());}
  VarBitWidth getBitWidth() const noexcept {return getBitWidth(getType());}

  operator bool () const noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
      case binom::ValType::ui8:
      case binom::ValType::si8: return getArithmeticData().bool_val;
      case binom::ValType::ui16: return getArithmeticData().ui16_val;
      case binom::ValType::si16: return getArithmeticData().i16_val;
      case binom::ValType::ui32: return getArithmeticData().ui32_val;
      case binom::ValType::si32: return getArithmeticData().i32_val;
      case binom::ValType::f32: return getArithmeticData().f32_val;
      case binom::ValType::ui64: return getArithmeticData().ui64_val;
      case binom::ValType::si64: return getArithmeticData().i64_val;
      case binom::ValType::f64: return getArithmeticData().f64_val;
      case binom::ValType::invalid_type:
      default: return false;
    }
  }

  operator ui8 () const noexcept {
    switch (getType()) {
      case binom::ValType::boolean: return getArithmeticData().ui8_val;
      case binom::ValType::ui8: return getArithmeticData().i8_val;
      case binom::ValType::si8: return getArithmeticData().ui8_val;
      case binom::ValType::ui16: return getArithmeticData().ui16_val;
      case binom::ValType::si16: return getArithmeticData().i16_val;
      case binom::ValType::ui32: return getArithmeticData().ui32_val;
      case binom::ValType::si32: return getArithmeticData().i32_val;
      case binom::ValType::f32: return getArithmeticData().f32_val;
      case binom::ValType::ui64: return getArithmeticData().ui64_val;
      case binom::ValType::si64: return getArithmeticData().i64_val;
      case binom::ValType::f64: return getArithmeticData().f64_val;
      case binom::ValType::invalid_type:
      default: return getArithmeticData().ui64_val;
    }
  }

  operator i8 () const noexcept {
    switch (getType()) {
      case binom::ValType::boolean: return getArithmeticData().i8_val;
      case binom::ValType::ui8: return getArithmeticData().ui8_val;
      case binom::ValType::si8: return getArithmeticData().i8_val;
      case binom::ValType::ui16: return getArithmeticData().ui16_val;
      case binom::ValType::si16: return getArithmeticData().i16_val;
      case binom::ValType::ui32: return getArithmeticData().ui32_val;
      case binom::ValType::si32: return getArithmeticData().i32_val;
      case binom::ValType::f32: return getArithmeticData().f32_val;
      case binom::ValType::ui64: return getArithmeticData().ui64_val;
      case binom::ValType::si64: return getArithmeticData().i64_val;
      case binom::ValType::f64: return getArithmeticData().f64_val;
      case binom::ValType::invalid_type:
      default: return getArithmeticData().ui64_val;
    }
  }

  operator ui16 () const noexcept {
    switch (getType()) {
      case binom::ValType::boolean: return getArithmeticData().ui8_val;
      case binom::ValType::ui8: return getArithmeticData().ui8_val;
      case binom::ValType::si8: return getArithmeticData().i8_val;
      case binom::ValType::ui16: return getArithmeticData().ui16_val;
      case binom::ValType::si16: return getArithmeticData().i16_val;
      case binom::ValType::ui32: return getArithmeticData().ui32_val;
      case binom::ValType::si32: return getArithmeticData().i32_val;
      case binom::ValType::f32: return getArithmeticData().f32_val;
      case binom::ValType::ui64: return getArithmeticData().ui64_val;
      case binom::ValType::si64: return getArithmeticData().i64_val;
      case binom::ValType::f64: return getArithmeticData().f64_val;
      case binom::ValType::invalid_type:
      default: return getArithmeticData().ui64_val;
    }
  }

  operator i16 () const noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
      case binom::ValType::ui8: return getArithmeticData().ui8_val;
      case binom::ValType::si8: return getArithmeticData().i8_val;
      case binom::ValType::ui16: return getArithmeticData().ui16_val;
      case binom::ValType::si16: return getArithmeticData().i16_val;
      case binom::ValType::ui32: return getArithmeticData().ui32_val;
      case binom::ValType::si32: return getArithmeticData().i32_val;
      case binom::ValType::f32: return getArithmeticData().f32_val;
      case binom::ValType::ui64: return getArithmeticData().ui64_val;
      case binom::ValType::si64: return getArithmeticData().i64_val;
      case binom::ValType::f64: return getArithmeticData().f64_val;
      case binom::ValType::invalid_type:
      default: return getArithmeticData().ui64_val;
    }
  }

  operator ui32 () const noexcept {
    switch (getType()) {
      case binom::ValType::boolean: return getArithmeticData().ui8_val;
      case binom::ValType::ui8: return getArithmeticData().ui8_val;
      case binom::ValType::si8: return getArithmeticData().i8_val;
      case binom::ValType::ui16: return getArithmeticData().ui16_val;
      case binom::ValType::si16: return getArithmeticData().i16_val;
      case binom::ValType::ui32: return getArithmeticData().ui32_val;
      case binom::ValType::si32: return getArithmeticData().i32_val;
      case binom::ValType::f32: return getArithmeticData().f32_val;
      case binom::ValType::ui64: return getArithmeticData().ui64_val;
      case binom::ValType::si64: return getArithmeticData().i64_val;
      case binom::ValType::f64: return getArithmeticData().f64_val;
      case binom::ValType::invalid_type:
      default: return getArithmeticData().ui64_val;
    }
  }

  operator i32 () const noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
      case binom::ValType::ui8: return getArithmeticData().ui8_val;
      case binom::ValType::si8: return getArithmeticData().i8_val;
      case binom::ValType::ui16: return getArithmeticData().ui16_val;
      case binom::ValType::si16: return getArithmeticData().i16_val;
      case binom::ValType::ui32: return getArithmeticData().ui32_val;
      case binom::ValType::si32: return getArithmeticData().i32_val;
      case binom::ValType::f32: return getArithmeticData().f32_val;
      case binom::ValType::ui64: return getArithmeticData().ui64_val;
      case binom::ValType::si64: return getArithmeticData().i64_val;
      case binom::ValType::f64: return getArithmeticData().f64_val;
      case binom::ValType::invalid_type:
      default: return getArithmeticData().ui64_val;
    }
  }

  operator f32 () const noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
      case binom::ValType::ui8: return getArithmeticData().ui8_val;
      case binom::ValType::si8: return getArithmeticData().i8_val;
      case binom::ValType::ui16: return getArithmeticData().ui16_val;
      case binom::ValType::si16: return getArithmeticData().i16_val;
      case binom::ValType::ui32: return getArithmeticData().ui32_val;
      case binom::ValType::si32: return getArithmeticData().i32_val;
      case binom::ValType::f32: return getArithmeticData().f32_val;
      case binom::ValType::ui64: return getArithmeticData().ui64_val;
      case binom::ValType::si64: return getArithmeticData().i64_val;
      case binom::ValType::f64: return getArithmeticData().f64_val;
      case binom::ValType::invalid_type:
      default: return getArithmeticData().ui64_val;
    }
  }

  operator ui64 () const noexcept {
    switch (getType()) {
      case binom::ValType::boolean: return getArithmeticData().ui8_val;
      case binom::ValType::ui8: return getArithmeticData().ui8_val;
      case binom::ValType::si8: return getArithmeticData().i8_val;
      case binom::ValType::ui16: return getArithmeticData().ui16_val;
      case binom::ValType::si16: return getArithmeticData().i16_val;
      case binom::ValType::ui32: return getArithmeticData().ui32_val;
      case binom::ValType::si32: return getArithmeticData().i32_val;
      case binom::ValType::f32: return getArithmeticData().f32_val;
      case binom::ValType::ui64: return getArithmeticData().ui64_val;
      case binom::ValType::si64: return getArithmeticData().i64_val;
      case binom::ValType::f64: return getArithmeticData().f64_val;
      case binom::ValType::invalid_type:
      default: return getArithmeticData().ui64_val;
    }
  }

  operator i64 () const noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
      case binom::ValType::ui8: return getArithmeticData().ui8_val;
      case binom::ValType::si8: return getArithmeticData().i8_val;
      case binom::ValType::ui16: return getArithmeticData().ui16_val;
      case binom::ValType::si16: return getArithmeticData().i16_val;
      case binom::ValType::ui32: return getArithmeticData().ui32_val;
      case binom::ValType::si32: return getArithmeticData().i32_val;
      case binom::ValType::f32: return getArithmeticData().f32_val;
      case binom::ValType::ui64: return getArithmeticData().ui64_val;
      case binom::ValType::si64: return getArithmeticData().i64_val;
      case binom::ValType::f64: return getArithmeticData().f64_val;
      case binom::ValType::invalid_type:
      default: return getArithmeticData().ui64_val;
    }
  }

  operator f64 () const noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
      case binom::ValType::ui8: return getArithmeticData().ui8_val;
      case binom::ValType::si8: return getArithmeticData().i8_val;
      case binom::ValType::ui16: return getArithmeticData().ui16_val;
      case binom::ValType::si16: return getArithmeticData().i16_val;
      case binom::ValType::ui32: return getArithmeticData().ui32_val;
      case binom::ValType::si32: return getArithmeticData().i32_val;
      case binom::ValType::f32: return getArithmeticData().f32_val;
      case binom::ValType::ui64: return getArithmeticData().ui64_val;
      case binom::ValType::si64: return getArithmeticData().i64_val;
      case binom::ValType::f64: return getArithmeticData().f64_val;
      case binom::ValType::invalid_type:
      default: return getArithmeticData().ui64_val;
    }
  }

  bool operator==(ArithmeticTypeDriven&& other) const noexcept {return *this == other;}
  bool operator==(const ArithmeticTypeDriven& other) const noexcept {
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

  bool operator!=(ArithmeticTypeDriven&& other) const noexcept {return !(*this == other);}
  bool operator!=(const ArithmeticTypeDriven& other) const noexcept {return !(*this == other);}

  bool operator>(ArithmeticTypeDriven&& other) const noexcept {return *this > other;}
  bool operator>(const ArithmeticTypeDriven& other) const noexcept {
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

  bool operator>=(ArithmeticTypeDriven&& other) const noexcept {return *this >= other;}
  bool operator>=(const ArithmeticTypeDriven& other) const noexcept {
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

  bool operator<(ArithmeticTypeDriven&& other) const noexcept {return *this < other;}
  bool operator<(const ArithmeticTypeDriven& other) const noexcept {
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

  bool operator<=(ArithmeticTypeDriven&& other) const noexcept {return *this <= other;}
  bool operator<=(const ArithmeticTypeDriven& other) const noexcept {
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

  inline bool operator==(bool value) const noexcept {return equalWithUnsigned(value);}
  inline bool operator!=(bool value) const noexcept {return notEqualWithUnsigned(value);}
  inline bool operator>(bool value) const noexcept  {return highterThenUnsigned(value);}
  inline bool operator>=(bool value) const noexcept  {return highterOrEqualThenUnsigned(value);}
  inline bool operator<(bool value) const noexcept  {return lowerThenUnsigned(value);}
  inline bool operator<=(bool value) const noexcept  {return lowerOrEqualThenUnsigned(value);}

  inline bool operator==(ui8 value) const noexcept {return equalWithUnsigned(value);}
  inline bool operator!=(ui8 value) const noexcept {return notEqualWithUnsigned(value);}
  inline bool operator>(ui8 value) const noexcept  {return highterThenUnsigned(value);}
  inline bool operator>=(ui8 value) const noexcept  {return highterOrEqualThenUnsigned(value);}
  inline bool operator<(ui8 value) const noexcept  {return lowerThenUnsigned(value);}
  inline bool operator<=(ui8 value) const noexcept  {return lowerOrEqualThenUnsigned(value);}


  inline bool operator==(i8 value) const noexcept {return equalWithSigned(value);}
  inline bool operator!=(i8 value) const noexcept {return notEqualWithSigned(value);}
  inline bool operator>(i8 value) const noexcept  {return highterThenSigned(value);}
  inline bool operator>=(i8 value) const noexcept  {return highterOrEqualThenSigned(value);}
  inline bool operator<(i8 value) const noexcept  {return lowerThenSigned(value);}
  inline bool operator<=(i8 value) const noexcept  {return lowerOrEqualThenSigned(value);}

  inline bool operator==(ui16 value) const noexcept {return equalWithUnsigned(value);}
  inline bool operator!=(ui16 value) const noexcept {return notEqualWithUnsigned(value);}
  inline bool operator>(ui16 value) const noexcept  {return highterThenUnsigned(value);}
  inline bool operator>=(ui16 value) const noexcept  {return highterOrEqualThenUnsigned(value);}
  inline bool operator<(ui16 value) const noexcept  {return lowerThenUnsigned(value);}
  inline bool operator<=(ui16 value) const noexcept  {return lowerOrEqualThenUnsigned(value);}

  inline bool operator==(i16 value) const noexcept {return equalWithSigned(value);}
  inline bool operator!=(i16 value) const noexcept {return notEqualWithSigned(value);}
  inline bool operator>(i16 value) const noexcept  {return highterThenSigned(value);}
  inline bool operator>=(i16 value) const noexcept  {return highterOrEqualThenSigned(value);}
  inline bool operator<(i16 value) const noexcept  {return lowerThenSigned(value);}
  inline bool operator<=(i16 value) const noexcept  {return lowerOrEqualThenSigned(value);}

  inline bool operator==(ui32 value) const noexcept {return equalWithUnsigned(value);}
  inline bool operator!=(ui32 value) const noexcept {return notEqualWithUnsigned(value);}
  inline bool operator>(ui32 value) const noexcept  {return highterThenUnsigned(value);}
  inline bool operator>=(ui32 value) const noexcept  {return highterOrEqualThenUnsigned(value);}
  inline bool operator<(ui32 value) const noexcept  {return lowerThenUnsigned(value);}
  inline bool operator<=(ui32 value) const noexcept  {return lowerOrEqualThenUnsigned(value);}

  inline bool operator==(i32 value) const noexcept {return equalWithSigned(value);}
  inline bool operator!=(i32 value) const noexcept {return notEqualWithSigned(value);}
  inline bool operator>(i32 value) const noexcept  {return highterThenSigned(value);}
  inline bool operator>=(i32 value) const noexcept  {return highterOrEqualThenSigned(value);}
  inline bool operator<(i32 value) const noexcept  {return lowerThenSigned(value);}
  inline bool operator<=(i32 value) const noexcept  {return lowerOrEqualThenSigned(value);}

  inline bool operator==(f32 value) const noexcept {return equalWithSigned(value);}
  inline bool operator!=(f32 value) const noexcept {return notEqualWithSigned(value);}
  inline bool operator>(f32 value) const noexcept  {return highterThenSigned(value);}
  inline bool operator>=(f32 value) const noexcept  {return highterOrEqualThenSigned(value);}
  inline bool operator<(f32 value) const noexcept  {return lowerThenSigned(value);}
  inline bool operator<=(f32 value) const noexcept  {return lowerOrEqualThenSigned(value);}

  inline bool operator==(ui64 value) const noexcept {return equalWithUnsigned(value);}
  inline bool operator!=(ui64 value) const noexcept {return notEqualWithUnsigned(value);}
  inline bool operator>(ui64 value) const noexcept  {return highterThenUnsigned(value);}
  inline bool operator>=(ui64 value) const noexcept  {return highterOrEqualThenUnsigned(value);}
  inline bool operator<(ui64 value) const noexcept  {return lowerThenUnsigned(value);}
  inline bool operator<=(ui64 value) const noexcept  {return lowerOrEqualThenUnsigned(value);}

  inline bool operator==(i64 value) const noexcept {return equalWithSigned(value);}
  inline bool operator!=(i64 value) const noexcept {return notEqualWithSigned(value);}
  inline bool operator>(i64 value) const noexcept  {return highterThenSigned(value);}
  inline bool operator>=(i64 value) const noexcept  {return highterOrEqualThenSigned(value);}
  inline bool operator<(i64 value) const noexcept  {return lowerThenSigned(value);}
  inline bool operator<=(i64 value) const noexcept  {return lowerOrEqualThenSigned(value);}

  inline bool operator==(f64 value) const noexcept {return equalWithSigned(value);}
  inline bool operator!=(f64 value) const noexcept {return notEqualWithSigned(value);}
  inline bool operator>(f64 value) const noexcept  {return highterThenSigned(value);}
  inline bool operator>=(f64 value) const noexcept  {return highterOrEqualThenSigned(value);}
  inline bool operator<(f64 value) const noexcept  {return lowerThenSigned(value);}
  inline bool operator<=(f64 value) const noexcept  {return lowerOrEqualThenSigned(value);}

  ArithmeticTypeDriven& operator=(bool value) noexcept { setArithmeticData(ValType::boolean,  ArithmeticData{.bool_val =  value}); return downcast(); }
  ArithmeticTypeDriven& operator=(ui8 value) noexcept  { setArithmeticData(ValType::ui8,      ArithmeticData{.ui8_val =   value}); return downcast(); }
  ArithmeticTypeDriven& operator=(i8 value) noexcept   { setArithmeticData(ValType::si8,      ArithmeticData{.i8_val =    value}); return downcast(); }
  ArithmeticTypeDriven& operator=(ui16 value) noexcept { setArithmeticData(ValType::ui16,     ArithmeticData{.ui16_val =  value}); return downcast(); }
  ArithmeticTypeDriven& operator=(i16 value) noexcept  { setArithmeticData(ValType::si16,     ArithmeticData{.i16_val =   value}); return downcast(); }
  ArithmeticTypeDriven& operator=(ui32 value) noexcept { setArithmeticData(ValType::ui32,     ArithmeticData{.ui32_val =  value}); return downcast(); }
  ArithmeticTypeDriven& operator=(i32 value) noexcept  { setArithmeticData(ValType::si32,     ArithmeticData{.i32_val =   value}); return downcast(); }
  ArithmeticTypeDriven& operator=(f32 value) noexcept  { setArithmeticData(ValType::f32,      ArithmeticData{.f32_val =   value}); return downcast(); }
  ArithmeticTypeDriven& operator=(ui64 value) noexcept { setArithmeticData(ValType::ui64,     ArithmeticData{.ui64_val =  value}); return downcast(); }
  ArithmeticTypeDriven& operator=(i64 value) noexcept  { setArithmeticData(ValType::si64,     ArithmeticData{.i64_val =   value}); return downcast(); }
  ArithmeticTypeDriven& operator=(f64 value) noexcept  { setArithmeticData(ValType::f64,      ArithmeticData{.f64_val =   value}); return downcast(); }
  ArithmeticTypeDriven& operator=(const ArithmeticTypeDriven& value) noexcept { setArithmeticData(dynamic_cast<const ArithmeticTypeBase&>(value).getType(), dynamic_cast<const ArithmeticTypeBase&>(value).getArithmeticData()); return downcast(); }
  ArithmeticTypeDriven& operator=(ArithmeticTypeDriven&& value) noexcept      { setArithmeticData(dynamic_cast<ArithmeticTypeBase&>(value).getType(), dynamic_cast<ArithmeticTypeBase&>(value).getArithmeticData()); return downcast(); }

  ArithmeticTypeDriven& operator+=(ArithmeticTypeDriven&& value) noexcept {return *this += value;}
  ArithmeticTypeDriven& operator+=(const ArithmeticTypeDriven& value) noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().bool_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().bool_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().bool_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui8_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui8_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui8_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i8_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i8_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i8_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui16_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui16_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui16_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i16_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i16_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i16_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui32_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui32_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui32_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i32_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i32_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i32_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().f32_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().f32_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().f32_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui64_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui64_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui64_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i64_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i64_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i64_val += f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().f64_val += ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().f64_val += i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().f64_val += f64(value);
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
    return downcast();
  }

  ArithmeticTypeDriven& operator-=(ArithmeticTypeDriven&& value) noexcept {return *this -= value;}
  ArithmeticTypeDriven& operator-=(const ArithmeticTypeDriven& value) noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().bool_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().bool_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().bool_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui8_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui8_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui8_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i8_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i8_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i8_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui16_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui16_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui16_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i16_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i16_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i16_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui32_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui32_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui32_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i32_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i32_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i32_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().f32_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().f32_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().f32_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui64_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui64_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui64_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i64_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i64_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i64_val -= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().f64_val -= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().f64_val -= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().f64_val -= f64(value);
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
    return downcast();
  }

  ArithmeticTypeDriven& operator*=(ArithmeticTypeDriven&& value) noexcept {return *this *= value;}
  ArithmeticTypeDriven& operator*=(const ArithmeticTypeDriven& value) noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().bool_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().bool_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().bool_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui8_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui8_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui8_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i8_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i8_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i8_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui16_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui16_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui16_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i16_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i16_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i16_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui32_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui32_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui32_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i32_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i32_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i32_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().f32_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().f32_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().f32_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui64_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui64_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui64_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i64_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i64_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i64_val *= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().f64_val *= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().f64_val *= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().f64_val *= f64(value);
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
    return downcast();
  }

  ArithmeticTypeDriven& operator/=(ArithmeticTypeDriven&& value) noexcept {return *this /= value;}
  ArithmeticTypeDriven& operator/=(const ArithmeticTypeDriven& value) noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().bool_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().bool_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().bool_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui8_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui8_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui8_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i8_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i8_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i8_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui16_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui16_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui16_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i16_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i16_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i16_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui32_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui32_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui32_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i32_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i32_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i32_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().f32_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().f32_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().f32_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui64_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui64_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui64_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i64_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i64_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i64_val /= f64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().f64_val /= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().f64_val /= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().f64_val /= f64(value);
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
    return downcast();
  }

  ArithmeticTypeDriven& operator%=(ArithmeticTypeDriven&& value) noexcept {return *this %= value;}
  ArithmeticTypeDriven& operator%=(const ArithmeticTypeDriven& value) noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().bool_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().bool_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().bool_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui8_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui8_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui8_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si8:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i8_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i8_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i8_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui16_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui16_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui16_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si16:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i16_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i16_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i16_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui32_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui32_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui32_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i32_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i32_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i32_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f32:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().f32_val = i64(*this) % ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().f32_val = i64(*this) % i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().f32_val = i64(*this) % i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::ui64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().ui64_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().ui64_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().ui64_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::si64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().i64_val %= ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().i64_val %= i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().i64_val %= i64(value);
          break;
          default:
          case binom::VarNumberType::invalid_type:
          break;
        }
      break;
      case binom::ValType::f64:
        switch (value.getNumberType()) {
          case binom::VarNumberType::unsigned_integer:
            getArithmeticData().f64_val = i64(*this) % ui64(value);
          break;
          case binom::VarNumberType::signed_integer:
            getArithmeticData().f64_val = i64(*this) % i64(value);
          break;
          case binom::VarNumberType::float_point:
            getArithmeticData().f64_val = i64(*this) % i64(value);
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
    return downcast();
  }

  ArithmeticTypeDriven& operator++() noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
      case binom::ValType::ui8: ++getArithmeticData().ui8_val; break;
      case binom::ValType::si8: ++getArithmeticData().i8_val; break;
      case binom::ValType::ui16: ++getArithmeticData().ui16_val; break;
      case binom::ValType::si16: ++getArithmeticData().i16_val; break;
      case binom::ValType::ui32: ++getArithmeticData().ui32_val; break;
      case binom::ValType::si32: ++getArithmeticData().i32_val; break;
      case binom::ValType::f32: ++getArithmeticData().f32_val; break;
      case binom::ValType::ui64: ++getArithmeticData().ui64_val; break;
      case binom::ValType::si64: ++getArithmeticData().i64_val; break;
      case binom::ValType::f64: ++getArithmeticData().f64_val; break;
      case binom::ValType::invalid_type:
      default: break;
    }
    return downcast();
  }

  ArithmeticTypeDriven& operator--() noexcept {
    switch (getType()) {
      case binom::ValType::boolean:
      case binom::ValType::ui8: --getArithmeticData().ui8_val; break;
      case binom::ValType::si8: --getArithmeticData().i8_val; break;
      case binom::ValType::ui16: --getArithmeticData().ui16_val; break;
      case binom::ValType::si16: --getArithmeticData().i16_val; break;
      case binom::ValType::ui32: --getArithmeticData().ui32_val; break;
      case binom::ValType::si32: --getArithmeticData().i32_val; break;
      case binom::ValType::f32: --getArithmeticData().f32_val; break;
      case binom::ValType::ui64: --getArithmeticData().ui64_val; break;
      case binom::ValType::si64: --getArithmeticData().i64_val; break;
      case binom::ValType::f64: --getArithmeticData().f64_val; break;
      case binom::ValType::invalid_type:
      default: break;
    }
    return downcast();
  }
};


template <typename ArithmeticTypeDriven>
class ExtendedArithmeticTypeBase : public ArithmeticTypeBase<ArithmeticTypeDriven> {
  friend class ArithmeticTypeBase<ArithmeticTypeDriven>;

  inline ArithmeticTypeDriven& downcast() noexcept {return *reinterpret_cast<ArithmeticTypeDriven*>(this);}
  inline const ArithmeticTypeDriven& downcast() const noexcept {return *reinterpret_cast<const ArithmeticTypeDriven*>(this);}

public:
  ArithmeticTypeDriven operator+(ArithmeticTypeDriven value) const noexcept {return ArithmeticTypeDriven(downcast()) += std::move(value);}
  ArithmeticTypeDriven operator-(ArithmeticTypeDriven value) const noexcept {return ArithmeticTypeDriven(downcast()) -= std::move(value);}
  ArithmeticTypeDriven operator*(ArithmeticTypeDriven value) const noexcept {return ArithmeticTypeDriven(downcast()) *= std::move(value);}
  ArithmeticTypeDriven operator/(ArithmeticTypeDriven value) const noexcept {return ArithmeticTypeDriven(downcast()) /= std::move(value);}
  ArithmeticTypeDriven operator%(ArithmeticTypeDriven value) const noexcept {return ArithmeticTypeDriven(downcast()) %= std::move(value);}
  ArithmeticTypeDriven operator++(int) noexcept {return ArithmeticTypeDriven(downcast()) += 1;}
  ArithmeticTypeDriven operator--(int) noexcept {return ArithmeticTypeDriven(downcast()) -= 1;}
};

}

#endif // ARITHMETIC_H
