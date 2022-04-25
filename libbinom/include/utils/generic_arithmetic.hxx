// Some JavaScript in my C++

/* # Instructions for inheriting arithmetic logic from the base classes of this header file

 * To implementation for driven class of ExtendedArithmeticTypeBase or ArithmeticTypeBase:
ArithmeticData& getArithmeticDataImpl() const; - get union with number data
void setArithmeticDataImpl(ValType type, ArithmeticData data); - set union with number data
ValType getTypeImpl() const; - get number type

 * Boilerplate for moving assignment operators to a child class (change ArithmeticTypeDriven to name of driven class):
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

 * Optional thread-safety mechanic(disabled by default):
Template argument LockType used for implemt thread safety:
  For example can be set std::optinal<std::variant<std::unique_lock<std::shared_mutex>, std::shared_lock<std::shared_mutex>>>
  LockType getLockImpl(priv::MtxLockType lock_type) const noexcept; - overload in driven class for change mechanic of getting optional mutex lock
Template argument LockCheck - to check if the lock is received or not:
  Can be set invokable type with overloaded operator: bool operator()(LockType& lock) noexcept;

 * ExtendedArithmeticTypeBase - can be used if the child class has a copy constructor from the same type class:

*/

#ifndef GENERIC_ARITHMETIC_H
#define GENERIC_ARITHMETIC_H

#include "types.hxx"
#include "shared_recursive_mutex_wrapper.hxx"

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

template <typename OptionalLock>
struct OptionalLockCheck {bool operator()(OptionalLock& lock) const noexcept {return lock;}};

template <typename ArithmeticTypeDriven, typename LockType = priv::OptionalLockPlaceholder, typename LockCheck = OptionalLockCheck<LockType>>
class ExtendedArithmeticTypeBase;

/**
 * @brief The ArithmeticTypeBase class
 */
template <typename ArithmeticTypeDriven, typename LockType = priv::OptionalLockPlaceholder, typename LockCheck = OptionalLockCheck<LockType>>
class ArithmeticTypeBase {

  inline ArithmeticData& getArithmeticData() const {return reinterpret_cast<const ArithmeticTypeDriven*>(this)->getArithmeticDataImpl();}

  inline void setArithmeticData(ValType type, ArithmeticData data) {
    auto lk = getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return;
    return reinterpret_cast<ArithmeticTypeDriven*>(this)->setArithmeticDataImpl(type, data);
  }

  inline ArithmeticTypeDriven& downcast() noexcept {return *reinterpret_cast<ArithmeticTypeDriven*>(this);}
  inline const ArithmeticTypeDriven& downcast() const noexcept {return *reinterpret_cast<const ArithmeticTypeDriven*>(this);}
  LockType getLockImpl(priv::MtxLockType lock_type) const noexcept {return LockType(nullptr, lock_type);}

  template <typename T>
  inline bool equalWithUnsigned(T value) const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) == value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) == value : false;
    case binom::VarNumberType::float_point: return f64(*this) == value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool notEqualWithUnsigned(T value) const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) != value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) != value : true;
    case binom::VarNumberType::float_point: return f64(*this) != value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool highterThenUnsigned(T value) const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) > value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) > value : false;
    case binom::VarNumberType::float_point: return f64(*this) > value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool highterOrEqualThenUnsigned(T value) const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) >= value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) >= value : false;
    case binom::VarNumberType::float_point: return f64(*this) >= value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool lowerThenUnsigned(T value) const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) < value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) < value : true;
    case binom::VarNumberType::float_point: return f64(*this) < value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool lowerOrEqualThenUnsigned(T value) const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) <= value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) <= value : true;
    case binom::VarNumberType::float_point: return f64(*this) <= value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool equalWithSigned(T value) const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return ui64(*this) == value;
    case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) == value : false;
    case binom::VarNumberType::float_point: return f64(*this) == value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool notEqualWithSigned(T value) const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(*this) != value : true;
    case binom::VarNumberType::signed_integer: return i64(*this) != value;
    case binom::VarNumberType::float_point: return f64(*this) != value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool highterThenSigned(T value) const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(*this) > value : true;
    case binom::VarNumberType::signed_integer: return i64(*this) > value;
    case binom::VarNumberType::float_point: return f64(*this) > value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool highterOrEqualThenSigned(T value) const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(*this) >= value : true;
    case binom::VarNumberType::signed_integer: return i64(*this) >= value;
    case binom::VarNumberType::float_point: return f64(*this) >= value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool lowerThenSigned(T value) const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(*this) < value : false;
    case binom::VarNumberType::signed_integer: return i64(*this) < value;
    case binom::VarNumberType::float_point: return f64(*this) < value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template <typename T>
  inline bool lowerOrEqualThenSigned(T value) const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
    switch (getNumberType()) {
    case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(*this) <= value : false;
    case binom::VarNumberType::signed_integer: return i64(*this) <= value;
    case binom::VarNumberType::float_point: return f64(*this) <= value;
    case binom::VarNumberType::invalid_type: return false;
    }
  }

  template<typename T>
  inline T castArithmeticData() const {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
    switch (getType()) {
      case binom::ValType::boolean:
      case binom::ValType::ui8:
      case binom::ValType::si8: return static_cast<T>(getArithmeticData().bool_val);
      case binom::ValType::ui16: return static_cast<T>(getArithmeticData().ui16_val);
      case binom::ValType::si16: return static_cast<T>(getArithmeticData().i16_val);
      case binom::ValType::ui32: return static_cast<T>(getArithmeticData().ui32_val);
      case binom::ValType::si32: return static_cast<T>(getArithmeticData().i32_val);
      case binom::ValType::f32: return static_cast<T>(getArithmeticData().f32_val);
      case binom::ValType::ui64: return static_cast<T>(getArithmeticData().ui64_val);
      case binom::ValType::si64: return static_cast<T>(getArithmeticData().i64_val);
      case binom::ValType::f64: return static_cast<T>(getArithmeticData().f64_val);
      case binom::ValType::invalid_type:
      default: return false;
    }
  }

public:

  ValType getType() const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return ValType::invalid_type;
    return reinterpret_cast<const ArithmeticTypeDriven*>(this)->getTypeImpl();
  }

  VarNumberType getNumberType() const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return VarNumberType::invalid_type;
    return binom::getNumberType(getType());
  }
  VarBitWidth getBitWidth() const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return VarBitWidth::invalid_type;
    return getBitWidth(getType());
  }

  LockType getLock(priv::MtxLockType lock_type) const noexcept {return downcast().getLockImpl(lock_type);}

  operator bool () const noexcept {return castArithmeticData<bool>();}
  operator ui8 () const noexcept  {return castArithmeticData<ui8>();}
  operator i8 () const noexcept   {return castArithmeticData<i8>();}
  operator ui16 () const noexcept {return castArithmeticData<ui16>();}
  operator i16 () const noexcept  {return castArithmeticData<i16>();}
  operator ui32 () const noexcept {return castArithmeticData<ui32>();}
  operator i32 () const noexcept  {return castArithmeticData<i32>();}
  operator f32 () const noexcept  {return castArithmeticData<f32>();}
  operator ui64 () const noexcept {return castArithmeticData<ui64>();}
  operator i64 () const noexcept  {return castArithmeticData<i64>();}
  operator f64 () const noexcept  {return castArithmeticData<f64>();}

  bool operator==(ArithmeticTypeDriven&& other) const noexcept {return *this == other;}
  bool operator==(const ArithmeticTypeDriven& other) const noexcept {
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
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
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
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
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
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
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
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
    auto lk = getLock(priv::MtxLockType::shared_locked);
    if(!LockCheck()(lk)) return false;
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
    auto lk = getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
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
    auto lk = getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
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
    auto lk = getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
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
    auto lk = getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
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
    auto lk = getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
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
    auto lk = getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
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
    auto lk = getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
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


template <typename ArithmeticTypeDriven, typename LockType, typename LockCheck>
class ExtendedArithmeticTypeBase : public ArithmeticTypeBase<ArithmeticTypeDriven, LockType, LockCheck> {
  typedef ArithmeticTypeBase<ArithmeticTypeDriven, LockType, LockCheck> CurrentBase;
  friend class ArithmeticTypeBase<ArithmeticTypeDriven, LockType, LockCheck>;
  inline ArithmeticTypeDriven& downcast() noexcept {return *reinterpret_cast<ArithmeticTypeDriven*>(this);}
  inline const ArithmeticTypeDriven& downcast() const noexcept {return *reinterpret_cast<const ArithmeticTypeDriven*>(this);}

public:

  ArithmeticTypeDriven operator+(ArithmeticTypeDriven value) const noexcept {
    auto lk = CurrentBase::getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
    return ArithmeticTypeDriven(downcast()) += std::move(value);
  }

  ArithmeticTypeDriven operator-(ArithmeticTypeDriven value) const noexcept {
    auto lk = CurrentBase::getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
    return ArithmeticTypeDriven(downcast()) -= std::move(value);
  }
  ArithmeticTypeDriven operator*(ArithmeticTypeDriven value) const noexcept {
    auto lk = CurrentBase::getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
    return ArithmeticTypeDriven(downcast()) *= std::move(value);
  }
  ArithmeticTypeDriven operator/(ArithmeticTypeDriven value) const noexcept {
    auto lk = CurrentBase::getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
    return ArithmeticTypeDriven(downcast()) /= std::move(value);
  }
  ArithmeticTypeDriven operator%(ArithmeticTypeDriven value) const noexcept {
    auto lk = CurrentBase::getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
    return ArithmeticTypeDriven(downcast()) %= std::move(value);
  }
  ArithmeticTypeDriven operator++(int) noexcept {
    auto lk = CurrentBase::getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
    return ArithmeticTypeDriven(downcast()) += 1;
  }
  ArithmeticTypeDriven operator--(int) noexcept {
    auto lk = CurrentBase::getLock(priv::MtxLockType::unique_locked);
    if(!LockCheck()(lk)) return downcast();
    return ArithmeticTypeDriven(downcast()) -= 1;
  }
};

}

#endif // GENERIC_ARITHMETIC_H
