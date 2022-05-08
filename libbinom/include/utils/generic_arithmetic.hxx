#ifndef GENERIC_ARITHMETIC_H
#define GENERIC_ARITHMETIC_H

#include "types.hxx"
#include "shared_recursive_mutex_wrapper.hxx"
#include <cmath>

namespace binom::arithmetic {


// Type checks

template <class T>
struct remove_cvref : std::remove_cv<std::remove_reference_t<T>> {};

template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <class T, class U>
struct is_base_of : std::is_base_of<remove_cvref_t<T>, remove_cvref_t<U>> {};

template <class T, class U>
inline constexpr bool is_base_of_v = is_base_of<T, U>::value;

template <template<typename T> class CRTP_Base, class CRTP_Driven>
struct is_crtp_base_of : std::is_base_of<CRTP_Base<remove_cvref_t<CRTP_Driven>>, remove_cvref_t<CRTP_Driven>> {};

template <template<typename T> class CRTP_Base, class CRTP_Driven>
inline constexpr bool is_crtp_base_of_v = is_crtp_base_of<CRTP_Base, CRTP_Driven>::value;

template <class T>
struct is_integral_without_cvref : std::is_integral<remove_cvref_t<T>> {};

template <class T>
inline constexpr bool is_integral_without_cvref_v = is_integral_without_cvref<T>::value;

template <class T>
struct is_floating_point_without_cvref : std::is_floating_point<remove_cvref_t<T>> {};

template <class T>
inline constexpr bool is_floating_point_without_cvref_v = is_floating_point_without_cvref<T>::value;

template <class T>
struct is_arithmetic_without_cvref : std::is_arithmetic<remove_cvref_t<T>> {};

template <class T>
inline constexpr bool is_arithmetic_without_cvref_v = is_arithmetic_without_cvref<T>::value;

template <class T>
struct is_signed_without_cvref : std::is_signed<remove_cvref_t<T>> {};

template <class T>
inline constexpr bool is_signed_without_cvref_v = is_signed_without_cvref<T>::value;

template <class T>
struct is_unsigned_without_cvref : std::is_unsigned<remove_cvref_t<T>> {};

template <class T>
inline constexpr bool is_unsigned_without_cvref_v = is_unsigned_without_cvref<T>::value;

template <class T>
struct is_float_without_cvref : std::is_floating_point<remove_cvref_t<T>> {};

template <class T>
inline constexpr bool is_float_without_cvref_v = is_float_without_cvref<T>::value;

template <class T, class U>
struct is_same_without_cvref : std::is_same<remove_cvref_t<T>, remove_cvref_t<U>> {};

template <class T, class U>
inline constexpr bool is_same_without_cvref_v = is_same_without_cvref<T, U>::value;

//////////////////////




// Placehoders

template <typename OptionalLock>
struct OptionalLockCheck {bool operator()(OptionalLock& lock) const noexcept {return lock.has_value();}};

template <typename ArithmeticTypeDriven, typename LockType = priv::OptionalLockPlaceholder>
class ArithmeticImplPlaceholders {
protected:
  LockType getLockImpl(priv::MtxLockType lock_type) const noexcept {return LockType(nullptr, lock_type);}
};

//////////////////////

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


class EnableCopyableArithmetic {};


template <typename ArithmeticTypeDriven>
class ArithmeticTypeBase {
  ArithmeticTypeDriven& downcast() noexcept {return *reinterpret_cast<ArithmeticTypeDriven*>(this);}
  const ArithmeticTypeDriven& downcast() const noexcept {return *reinterpret_cast<const ArithmeticTypeDriven*>(this);}

  inline ArithmeticData& getArithmeticData() const {return downcast().getArithmeticDataImpl();}

public:

  ValType getValType() const noexcept {
    auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
    if(!downcast().checkLock(lk)) return ValType::invalid_type;
    return downcast().getValTypeImpl();
  }

  VarNumberType getNumberType() const noexcept {
    auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
    if(!downcast().checkLock(lk)) return VarNumberType::invalid_type;
    return binom::toNumberType(getValType());
  }

  VarBitWidth getBitWidth() const noexcept {
    auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
    if(!downcast().checkLock(lk)) return VarBitWidth::invalid_type;
    return getBitWidth(getValType());
  }

  template<typename T>
  operator T() const noexcept {
    static_assert (std::is_arithmetic_v<T> || (is_base_of_v<EnableCopyableArithmetic, T> && is_crtp_base_of_v<ArithmeticTypeBase, T>));
    if constexpr (std::is_arithmetic<T>::value) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getValType()) {
      case binom::ValType::boolean: return static_cast<T>(getArithmeticData().bool_val);
      case binom::ValType::ui8: return static_cast<T>(getArithmeticData().ui8_val);
      case binom::ValType::si8: return static_cast<T>(getArithmeticData().i8_val);
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
    } else if constexpr(is_base_of_v<EnableCopyableArithmetic, T> && is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getValType()) {
      case binom::ValType::boolean: return bool(self);
      case binom::ValType::ui8: return ui8(self);
      case binom::ValType::si8: return i8(self);
      case binom::ValType::ui16: return ui16(self);
      case binom::ValType::si16: return i16(self);
      case binom::ValType::ui32: return ui32(self);
      case binom::ValType::si32: return i32(self);
      case binom::ValType::f32: return f32(self);
      case binom::ValType::ui64: return ui64(self);
      case binom::ValType::si64: return i64(self);
      case binom::ValType::f64: return f64(self);
      case binom::ValType::invalid_type:
      default: return 0;
      }
    }
  }

  template<typename T>
  bool operator==(T&& value) const {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer:
        switch (value.getNumberType()) {
        case binom::VarNumberType::unsigned_integer: return ui64(self) == ui64(value);
        case binom::VarNumberType::signed_integer: return i64(value) >= 0 ? ui64(self) == ui64(value) : false;
        case binom::VarNumberType::float_point: return f64(value) >= 0 ? ui64(self) == f64(value) : false;
        default:
        case binom::VarNumberType::invalid_type: default: return false;
        }
      case binom::VarNumberType::signed_integer:
        switch (value.getNumberType()) {
        case binom::VarNumberType::unsigned_integer: return i64(self) >= 0 ? ui64(self) == ui64(value) : false;
        case binom::VarNumberType::signed_integer: return i64(self) == i64(value);
        case binom::VarNumberType::float_point: return i64(self) == f64(value);
        default:
        case binom::VarNumberType::invalid_type: default: return false;
        }
      case binom::VarNumberType::float_point:
        switch (value.getNumberType()) {
        case binom::VarNumberType::unsigned_integer: return f64(self) >= 0 ? ui64(self) == f64(value) : false;
        case binom::VarNumberType::signed_integer: return f64(self) == ui64(value);
        case binom::VarNumberType::float_point: return f64(self) == f64(value);
        default:
        case binom::VarNumberType::invalid_type: default: return false;
        }
      default:
      case binom::VarNumberType::invalid_type: default: return false;
      }
    } else if constexpr (is_unsigned_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer: return ui64(self) == value;
      case binom::VarNumberType::signed_integer: return i64(self) >= 0 ? ui64(self) == value : false;
      case binom::VarNumberType::float_point: return f64(self) >= 0 ? f64(self) > value : false;
      case binom::VarNumberType::invalid_type: default: return false;
      }
    } else if constexpr (is_signed_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(self) == value : false;
      case binom::VarNumberType::signed_integer: return i64(self) == value;
      case binom::VarNumberType::float_point: return f64(self) == value;
      case binom::VarNumberType::invalid_type: return false;
      }
    }
  }

  template<typename T>
  bool operator!=(T&& value) const {return !(self == value);}

  template<typename T>
  bool operator>(T&& value) const {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer:
        switch (value.getNumberType()) {
        case binom::VarNumberType::unsigned_integer: return ui64(self) > ui64(value);
        case binom::VarNumberType::signed_integer: return i64(value) >= 0 ? ui64(self) > ui64(value) : false;
        case binom::VarNumberType::float_point: return f64(value) >= 0 ? ui64(self) > f64(value) : false;
        default:
        case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::signed_integer:
        switch (value.getNumberType()) {
        case binom::VarNumberType::unsigned_integer: return i64(self) >= 0 ? ui64(self) > ui64(value) : false;
        case binom::VarNumberType::signed_integer: return i64(self) > i64(value);
        case binom::VarNumberType::float_point: return i64(self) > f64(value);
        default:
        case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::float_point:
        switch (value.getNumberType()) {
        case binom::VarNumberType::unsigned_integer: return f64(self) >= 0 ? ui64(self) > f64(value) : false;
        case binom::VarNumberType::signed_integer: return f64(self) > ui64(value);
        case binom::VarNumberType::float_point: return f64(self) > f64(value);
        default:
        case binom::VarNumberType::invalid_type: return false;
        }
      default:
      case binom::VarNumberType::invalid_type: return false;
      }
    } else if constexpr (is_unsigned_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer: return ui64(self) > value;
      case binom::VarNumberType::signed_integer: return i64(self) >= 0 ? ui64(self) > value : false;
      case binom::VarNumberType::float_point: return f64(self) >= 0 ? f64(self) > value : false;
      case binom::VarNumberType::invalid_type:default: return false;
      }
    } else if constexpr (is_signed_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(self) > value : false;
      case binom::VarNumberType::signed_integer: return ui64(self) > value;
      case binom::VarNumberType::float_point: return f64(self) > value;
      case binom::VarNumberType::invalid_type:default: return false;
      }
    }
  }

  template<typename T>
  bool operator>=(T&& value) const {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer:
        switch (value.getNumberType()) {
        case binom::VarNumberType::unsigned_integer: return ui64(self) >= ui64(value);
        case binom::VarNumberType::signed_integer: return i64(value) >= 0 ? ui64(self) >= ui64(value) : false;
        case binom::VarNumberType::float_point: return f64(value) >= 0 ? ui64(self) >= f64(value) : false;
        default:
        case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::signed_integer:
        switch (value.getNumberType()) {
        case binom::VarNumberType::unsigned_integer: return i64(self) >= 0 ? ui64(self) >= ui64(value) : false;
        case binom::VarNumberType::signed_integer: return i64(self) >= i64(value);
        case binom::VarNumberType::float_point: return i64(self) >= f64(value);
        default:
        case binom::VarNumberType::invalid_type: return false;
        }
      case binom::VarNumberType::float_point:
        switch (value.getNumberType()) {
        case binom::VarNumberType::unsigned_integer: return f64(self) >= 0 ? ui64(self) >= f64(value) : false;
        case binom::VarNumberType::signed_integer: return f64(self) >= ui64(value);
        case binom::VarNumberType::float_point: return f64(self) >= f64(value);
        default:
        case binom::VarNumberType::invalid_type: return false;
        }
      default:
      case binom::VarNumberType::invalid_type: return false;
      }
    } else if constexpr (is_unsigned_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer: return ui64(self) >= value;
      case binom::VarNumberType::signed_integer: return i64(self) >= 0 ? ui64(self) >= value : false;
      case binom::VarNumberType::float_point: return f64(self) >= 0 ? f64(self) >= value : false;
      case binom::VarNumberType::invalid_type:default: return false;
      }
    } else if constexpr (is_signed_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(self) >= value : false;
      case binom::VarNumberType::signed_integer: return ui64(self) >= value;
      case binom::VarNumberType::float_point: return f64(self) >= value;
      case binom::VarNumberType::invalid_type:default: return false;
      }
    }
  }

  template<typename T>
  bool operator<(T&& value) const {return !(self >= value);}

  template<typename T>
  bool operator<=(T&& value) const {return !(self > value);}

  template<typename T>
  ArithmeticTypeDriven& operator=(T&& value) noexcept {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr(is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (value.getValType()) {
      case binom::ValType::boolean: return  self = bool(value);
      case binom::ValType::ui8: return      self = ui8(value);
      case binom::ValType::si8: return      self = i8(value);
      case binom::ValType::ui16: return     self = ui16(value);
      case binom::ValType::si16: return     self = i16(value);
      case binom::ValType::ui32: return     self = ui32(value);
      case binom::ValType::si32: return     self = i32(value);
      case binom::ValType::f32: return      self = f32(value);
      case binom::ValType::ui64: return     self = ui64(value);
      case binom::ValType::si64: return     self = i64(value);
      case binom::ValType::f64: return      self = f64(value);
      case binom::ValType::invalid_type:default: return downcast();
       }
    } else if constexpr(is_arithmetic_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (getValType()) {
      case binom::ValType::boolean:
        getArithmeticData().bool_val = static_cast<bool>(value);
      break;
      case binom::ValType::ui8:
        getArithmeticData().ui8_val = static_cast<ui8>(value);
      break;
      case binom::ValType::si8:
        getArithmeticData().i8_val = static_cast<i8>(value);
      break;
      case binom::ValType::ui16:
        getArithmeticData().ui16_val = static_cast<ui16>(value);
      break;
      case binom::ValType::si16:
        getArithmeticData().i16_val = static_cast<i16>(value);
      break;
      case binom::ValType::ui32:
        getArithmeticData().ui32_val = static_cast<ui32>(value);
      break;
      case binom::ValType::si32:
        getArithmeticData().i32_val = static_cast<i32>(value);
      break;
      case binom::ValType::f32:
        getArithmeticData().f32_val = static_cast<f32>(value);
      break;
      case binom::ValType::ui64:
        getArithmeticData().ui64_val = static_cast<ui64>(value);
      break;
      case binom::ValType::si64:
        getArithmeticData().i64_val = static_cast<i64>(value);
      break;
      case binom::ValType::f64:
        getArithmeticData().f64_val = static_cast<f64>(value);
      break;
      case binom::ValType::invalid_type:default:
      break;
      }
      return downcast();
    }
  }

  template<typename T>
  ArithmeticTypeDriven& operator+=(T&& value) noexcept {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (value.getValType()) {
      case binom::ValType::boolean: return  self += bool(value);
      case binom::ValType::ui8: return      self += ui8(value);
      case binom::ValType::si8: return      self += i8(value);
      case binom::ValType::ui16: return     self += ui16(value);
      case binom::ValType::si16: return     self += i16(value);
      case binom::ValType::ui32: return     self += ui32(value);
      case binom::ValType::si32: return     self += i32(value);
      case binom::ValType::f32: return      self += f32(value);
      case binom::ValType::ui64: return     self += ui64(value);
      case binom::ValType::si64: return     self += i64(value);
      case binom::ValType::f64: return      self += f64(value);
      case binom::ValType::invalid_type:default: return downcast();
       }
    } else if constexpr(is_arithmetic_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (getValType()) {
      case binom::ValType::boolean: getArithmeticData().bool_val += value; break;
      case binom::ValType::ui8:     getArithmeticData().ui8_val += value; break;
      case binom::ValType::si8:     getArithmeticData().i8_val += value; break;
      case binom::ValType::ui16:    getArithmeticData().ui16_val += value; break;
      case binom::ValType::si16:    getArithmeticData().i16_val += value; break;
      case binom::ValType::ui32:    getArithmeticData().ui32_val += value; break;
      case binom::ValType::si32:    getArithmeticData().i32_val += value; break;
      case binom::ValType::f32:     getArithmeticData().f32_val += value; break;
      case binom::ValType::ui64:    getArithmeticData().ui64_val += value; break;
      case binom::ValType::si64:    getArithmeticData().i64_val += value; break;
      case binom::ValType::f64:     getArithmeticData().f64_val += value; break;
      case binom::ValType::invalid_type:default: break;
      }
      return downcast();
    }
  }

  template<typename T>
  ArithmeticTypeDriven& operator-=(T&& value) noexcept {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (value.getValType()) {
      case binom::ValType::boolean: return  self -= bool(value);
      case binom::ValType::ui8: return      self -= ui8(value);
      case binom::ValType::si8: return      self -= i8(value);
      case binom::ValType::ui16: return     self -= ui16(value);
      case binom::ValType::si16: return     self -= i16(value);
      case binom::ValType::ui32: return     self -= ui32(value);
      case binom::ValType::si32: return     self -= i32(value);
      case binom::ValType::f32: return      self -= f32(value);
      case binom::ValType::ui64: return     self -= ui64(value);
      case binom::ValType::si64: return     self -= i64(value);
      case binom::ValType::f64: return      self -= f64(value);
      case binom::ValType::invalid_type:default: return downcast();
       }
    } else if constexpr(is_arithmetic_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (getValType()) {
      case binom::ValType::boolean: getArithmeticData().bool_val -= value; break;
      case binom::ValType::ui8:     getArithmeticData().ui8_val -= value; break;
      case binom::ValType::si8:     getArithmeticData().i8_val -= value; break;
      case binom::ValType::ui16:    getArithmeticData().ui16_val -= value; break;
      case binom::ValType::si16:    getArithmeticData().i16_val -= value; break;
      case binom::ValType::ui32:    getArithmeticData().ui32_val -= value; break;
      case binom::ValType::si32:    getArithmeticData().i32_val -= value; break;
      case binom::ValType::f32:     getArithmeticData().f32_val -= value; break;
      case binom::ValType::ui64:    getArithmeticData().ui64_val -= value; break;
      case binom::ValType::si64:    getArithmeticData().i64_val -= value; break;
      case binom::ValType::f64:     getArithmeticData().f64_val -= value; break;
      case binom::ValType::invalid_type:default: break;
      }
      return downcast();
    }
  }

  template<typename T>
  ArithmeticTypeDriven& operator*=(T&& value) noexcept {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (value.getValType()) {
      case binom::ValType::boolean: return  self *= bool(value);
      case binom::ValType::ui8: return      self *= ui8(value);
      case binom::ValType::si8: return      self *= i8(value);
      case binom::ValType::ui16: return     self *= ui16(value);
      case binom::ValType::si16: return     self *= i16(value);
      case binom::ValType::ui32: return     self *= ui32(value);
      case binom::ValType::si32: return     self *= i32(value);
      case binom::ValType::f32: return      self *= f32(value);
      case binom::ValType::ui64: return     self *= ui64(value);
      case binom::ValType::si64: return     self *= i64(value);
      case binom::ValType::f64: return      self *= f64(value);
      case binom::ValType::invalid_type:default: return downcast();
       }
    } else if constexpr(is_arithmetic_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (getValType()) {
      case binom::ValType::boolean: getArithmeticData().bool_val = getArithmeticData().bool_val && value; break;
      case binom::ValType::ui8:     getArithmeticData().ui8_val *= value; break;
      case binom::ValType::si8:     getArithmeticData().i8_val *= value; break;
      case binom::ValType::ui16:    getArithmeticData().ui16_val *= value; break;
      case binom::ValType::si16:    getArithmeticData().i16_val *= value; break;
      case binom::ValType::ui32:    getArithmeticData().ui32_val *= value; break;
      case binom::ValType::si32:    getArithmeticData().i32_val *= value; break;
      case binom::ValType::f32:     getArithmeticData().f32_val *= value; break;
      case binom::ValType::ui64:    getArithmeticData().ui64_val *= value; break;
      case binom::ValType::si64:    getArithmeticData().i64_val *= value; break;
      case binom::ValType::f64:     getArithmeticData().f64_val *= value; break;
      case binom::ValType::invalid_type:default: break;
      }
      return downcast();
    }
  }

  template<typename T>
  ArithmeticTypeDriven& operator/=(T&& value) noexcept {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (value.getValType()) {
      case binom::ValType::boolean: return  self /= bool(value);
      case binom::ValType::ui8: return      self /= ui8(value);
      case binom::ValType::si8: return      self /= i8(value);
      case binom::ValType::ui16: return     self /= ui16(value);
      case binom::ValType::si16: return     self /= i16(value);
      case binom::ValType::ui32: return     self /= ui32(value);
      case binom::ValType::si32: return     self /= i32(value);
      case binom::ValType::f32: return      self /= f32(value);
      case binom::ValType::ui64: return     self /= ui64(value);
      case binom::ValType::si64: return     self /= i64(value);
      case binom::ValType::f64: return      self /= f64(value);
      case binom::ValType::invalid_type:default: return downcast();
       }
    } else if constexpr(is_arithmetic_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (getValType()) {
      case binom::ValType::boolean: getArithmeticData().bool_val /= value; break;
      case binom::ValType::ui8:     getArithmeticData().ui8_val /= value; break;
      case binom::ValType::si8:     getArithmeticData().i8_val /= value; break;
      case binom::ValType::ui16:    getArithmeticData().ui16_val /= value; break;
      case binom::ValType::si16:    getArithmeticData().i16_val /= value; break;
      case binom::ValType::ui32:    getArithmeticData().ui32_val /= value; break;
      case binom::ValType::si32:    getArithmeticData().i32_val /= value; break;
      case binom::ValType::f32:     getArithmeticData().f32_val /= value; break;
      case binom::ValType::ui64:    getArithmeticData().ui64_val /= value; break;
      case binom::ValType::si64:    getArithmeticData().i64_val /= value; break;
      case binom::ValType::f64:     getArithmeticData().f64_val /= value; break;
      case binom::ValType::invalid_type:default: break;
      }
      return downcast();
    }
  }

  template<typename T>
  ArithmeticTypeDriven& operator%=(T&& value) noexcept {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (value.getValType()) {
      case binom::ValType::boolean: return  self %= bool(value);
      case binom::ValType::ui8: return      self %= ui8(value);
      case binom::ValType::si8: return      self %= i8(value);
      case binom::ValType::ui16: return     self %= ui16(value);
      case binom::ValType::si16: return     self %= i16(value);
      case binom::ValType::ui32: return     self %= ui32(value);
      case binom::ValType::si32: return     self %= i32(value);
      case binom::ValType::f32: return      self %= f32(value);
      case binom::ValType::ui64: return     self %= ui64(value);
      case binom::ValType::si64: return     self %= i64(value);
      case binom::ValType::f64: return      self %= f64(value);
      case binom::ValType::invalid_type:default: return downcast();
       }
    } else if constexpr(is_integral_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (getValType()) {
      case binom::ValType::boolean: getArithmeticData().bool_val %= value; break;
      case binom::ValType::ui8:     getArithmeticData().ui8_val %= value; break;
      case binom::ValType::si8:     getArithmeticData().i8_val %= value; break;
      case binom::ValType::ui16:    getArithmeticData().ui16_val %= value; break;
      case binom::ValType::si16:    getArithmeticData().i16_val %= value; break;
      case binom::ValType::ui32:    getArithmeticData().ui32_val %= value; break;
      case binom::ValType::si32:    getArithmeticData().i32_val %= value; break;
      case binom::ValType::f32:     getArithmeticData().f32_val = i64(std::round(getArithmeticData().f32_val)) % value; break;
      case binom::ValType::ui64:    getArithmeticData().ui64_val %= value; break;
      case binom::ValType::si64:    getArithmeticData().i64_val %= value; break;
      case binom::ValType::f64:     getArithmeticData().f64_val = i64(std::round(getArithmeticData().f64_val)) % value; break;
      case binom::ValType::invalid_type:default: break;
      }
      return downcast();
    } else if constexpr (is_float_without_cvref_v<T>) {
      switch (getValType()) {
      case binom::ValType::boolean: getArithmeticData().bool_val %= i64(std::round(value)); break;
      case binom::ValType::ui8:     getArithmeticData().ui8_val %= i64(std::round(value)); break;
      case binom::ValType::si8:     getArithmeticData().i8_val %= i64(std::round(value)); break;
      case binom::ValType::ui16:    getArithmeticData().ui16_val %= i64(std::round(value)); break;
      case binom::ValType::si16:    getArithmeticData().i16_val %= i64(std::round(value)); break;
      case binom::ValType::ui32:    getArithmeticData().ui32_val %= i64(std::round(value)); break;
      case binom::ValType::si32:    getArithmeticData().i32_val %= i64(std::round(value)); break;
      case binom::ValType::f32:     getArithmeticData().f32_val = i64(std::round(getArithmeticData().f32_val)) % i64(std::round(value)); break;
      case binom::ValType::ui64:    getArithmeticData().ui64_val %= i64(std::round(value)); break;
      case binom::ValType::si64:    getArithmeticData().i64_val %= i64(std::round(value)); break;
      case binom::ValType::f64:     getArithmeticData().f64_val = i64(std::round(getArithmeticData().f64_val)) % i64(std::round(value)); break;
      case binom::ValType::invalid_type:default: break;
      }
      return downcast();
    }
  }

  template<typename T>
  ArithmeticTypeDriven& operator&=(T&& value) noexcept {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (value.getValType()) {
      case binom::ValType::boolean: return  self &= bool(value);
      case binom::ValType::ui8: return      self &= ui8(value);
      case binom::ValType::si8: return      self &= i8(value);
      case binom::ValType::ui16: return     self &= ui16(value);
      case binom::ValType::si16: return     self &= i16(value);
      case binom::ValType::ui32: return     self &= ui32(value);
      case binom::ValType::si32: return     self &= i32(value);
      case binom::ValType::f32: return      self &= f32(value);
      case binom::ValType::ui64: return     self &= ui64(value);
      case binom::ValType::si64: return     self &= i64(value);
      case binom::ValType::f64: return      self &= f64(value);
      case binom::ValType::invalid_type:default: return downcast();
       }
    } else if constexpr(is_arithmetic_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (getValType()) {
      case binom::ValType::boolean: getArithmeticData().bool_val &= value; break;
      case binom::ValType::ui8:     getArithmeticData().ui8_val &= value; break;
      case binom::ValType::si8:     getArithmeticData().i8_val &= value; break;
      case binom::ValType::ui16:    getArithmeticData().ui16_val &= value; break;
      case binom::ValType::si16:    getArithmeticData().i16_val &= value; break;
      case binom::ValType::ui32:    getArithmeticData().ui32_val &= value; break;
      case binom::ValType::si32:    getArithmeticData().i32_val &= value; break;
      case binom::ValType::f32:     getArithmeticData().f32_val &= value; break;
      case binom::ValType::ui64:    getArithmeticData().ui64_val &= value; break;
      case binom::ValType::si64:    getArithmeticData().i64_val &= value; break;
      case binom::ValType::f64:     getArithmeticData().f64_val &= value; break;
      case binom::ValType::invalid_type:default: break;
      }
      return downcast();
    }
  }

  template<typename T>
  ArithmeticTypeDriven& operator|=(T&& value) noexcept {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (value.getValType()) {
      case binom::ValType::boolean: return  self |= bool(value);
      case binom::ValType::ui8: return      self |= ui8(value);
      case binom::ValType::si8: return      self |= i8(value);
      case binom::ValType::ui16: return     self |= ui16(value);
      case binom::ValType::si16: return     self |= i16(value);
      case binom::ValType::ui32: return     self |= ui32(value);
      case binom::ValType::si32: return     self |= i32(value);
      case binom::ValType::f32: return      self |= f32(value);
      case binom::ValType::ui64: return     self |= ui64(value);
      case binom::ValType::si64: return     self |= i64(value);
      case binom::ValType::f64: return      self |= f64(value);
      case binom::ValType::invalid_type:default: return downcast();
       }
    } else if constexpr(is_arithmetic_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (getValType()) {
      case binom::ValType::boolean: getArithmeticData().bool_val |= value; break;
      case binom::ValType::ui8:     getArithmeticData().ui8_val |= value; break;
      case binom::ValType::si8:     getArithmeticData().i8_val |= value; break;
      case binom::ValType::ui16:    getArithmeticData().ui16_val |= value; break;
      case binom::ValType::si16:    getArithmeticData().i16_val |= value; break;
      case binom::ValType::ui32:    getArithmeticData().ui32_val |= value; break;
      case binom::ValType::si32:    getArithmeticData().i32_val |= value; break;
      case binom::ValType::f32:     getArithmeticData().f32_val |= value; break;
      case binom::ValType::ui64:    getArithmeticData().ui64_val |= value; break;
      case binom::ValType::si64:    getArithmeticData().i64_val |= value; break;
      case binom::ValType::f64:     getArithmeticData().f64_val |= value; break;
      case binom::ValType::invalid_type:default: break;
      }
      return downcast();
    }
  }

  template<typename T>
  ArithmeticTypeDriven& operator^=(T&& value) noexcept {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (value.getValType()) {
      case binom::ValType::boolean: return  self ^= bool(value);
      case binom::ValType::ui8: return      self ^= ui8(value);
      case binom::ValType::si8: return      self ^= i8(value);
      case binom::ValType::ui16: return     self ^= ui16(value);
      case binom::ValType::si16: return     self ^= i16(value);
      case binom::ValType::ui32: return     self ^= ui32(value);
      case binom::ValType::si32: return     self ^= i32(value);
      case binom::ValType::f32: return      self ^= f32(value);
      case binom::ValType::ui64: return     self ^= ui64(value);
      case binom::ValType::si64: return     self ^= i64(value);
      case binom::ValType::f64: return      self ^= f64(value);
      case binom::ValType::invalid_type:default: return downcast();
       }
    } else if constexpr(is_arithmetic_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (getValType()) {
      case binom::ValType::boolean: getArithmeticData().bool_val ^= value; break;
      case binom::ValType::ui8:     getArithmeticData().ui8_val ^= value; break;
      case binom::ValType::si8:     getArithmeticData().i8_val ^= value; break;
      case binom::ValType::ui16:    getArithmeticData().ui16_val ^= value; break;
      case binom::ValType::si16:    getArithmeticData().i16_val ^= value; break;
      case binom::ValType::ui32:    getArithmeticData().ui32_val ^= value; break;
      case binom::ValType::si32:    getArithmeticData().i32_val ^= value; break;
      case binom::ValType::f32:     getArithmeticData().f32_val ^= value; break;
      case binom::ValType::ui64:    getArithmeticData().ui64_val ^= value; break;
      case binom::ValType::si64:    getArithmeticData().i64_val ^= value; break;
      case binom::ValType::f64:     getArithmeticData().f64_val ^= value; break;
      case binom::ValType::invalid_type:default: break;
      }
      return downcast();
    }
  }

  template<typename T>
  ArithmeticTypeDriven& operator<<=(T&& value) noexcept {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (value.getValType()) {
      case binom::ValType::boolean: return  self <<= bool(value);
      case binom::ValType::ui8: return      self <<= ui8(value);
      case binom::ValType::si8: return      self <<= i8(value);
      case binom::ValType::ui16: return     self <<= ui16(value);
      case binom::ValType::si16: return     self <<= i16(value);
      case binom::ValType::ui32: return     self <<= ui32(value);
      case binom::ValType::si32: return     self <<= i32(value);
      case binom::ValType::f32: return      self <<= f32(value);
      case binom::ValType::ui64: return     self <<= ui64(value);
      case binom::ValType::si64: return     self <<= i64(value);
      case binom::ValType::f64: return      self <<= f64(value);
      case binom::ValType::invalid_type:default: return downcast();
       }
    } else if constexpr(is_arithmetic_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (getValType()) {
      case binom::ValType::boolean: getArithmeticData().bool_val <<= value; break;
      case binom::ValType::ui8:     getArithmeticData().ui8_val <<= value; break;
      case binom::ValType::si8:     getArithmeticData().i8_val <<= value; break;
      case binom::ValType::ui16:    getArithmeticData().ui16_val <<= value; break;
      case binom::ValType::si16:    getArithmeticData().i16_val <<= value; break;
      case binom::ValType::ui32:    getArithmeticData().ui32_val <<= value; break;
      case binom::ValType::si32:    getArithmeticData().i32_val <<= value; break;
      case binom::ValType::f32:     getArithmeticData().f32_val <<= value; break;
      case binom::ValType::ui64:    getArithmeticData().ui64_val <<= value; break;
      case binom::ValType::si64:    getArithmeticData().i64_val <<= value; break;
      case binom::ValType::f64:     getArithmeticData().f64_val <<= value; break;
      case binom::ValType::invalid_type:default: break;
      }
      return downcast();
    }
  }

  template<typename T>
  ArithmeticTypeDriven& operator>>=(T&& value) noexcept {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (value.getValType()) {
      case binom::ValType::boolean: return  self >>= bool(value);
      case binom::ValType::ui8: return      self >>= ui8(value);
      case binom::ValType::si8: return      self >>= i8(value);
      case binom::ValType::ui16: return     self >>= ui16(value);
      case binom::ValType::si16: return     self >>= i16(value);
      case binom::ValType::ui32: return     self >>= ui32(value);
      case binom::ValType::si32: return     self >>= i32(value);
      case binom::ValType::f32: return      self >>= f32(value);
      case binom::ValType::ui64: return     self >>= ui64(value);
      case binom::ValType::si64: return     self >>= i64(value);
      case binom::ValType::f64: return      self >>= f64(value);
      case binom::ValType::invalid_type:default: return downcast();
       }
    } else if constexpr(is_arithmetic_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
      if(!downcast().checkLock(lk)) return downcast();
      switch (getValType()) {
      case binom::ValType::boolean: getArithmeticData().bool_val >>= value; break;
      case binom::ValType::ui8:     getArithmeticData().ui8_val >>= value; break;
      case binom::ValType::si8:     getArithmeticData().i8_val >>= value; break;
      case binom::ValType::ui16:    getArithmeticData().ui16_val >>= value; break;
      case binom::ValType::si16:    getArithmeticData().i16_val >>= value; break;
      case binom::ValType::ui32:    getArithmeticData().ui32_val >>= value; break;
      case binom::ValType::si32:    getArithmeticData().i32_val >>= value; break;
      case binom::ValType::f32:     getArithmeticData().f32_val >>= value; break;
      case binom::ValType::ui64:    getArithmeticData().ui64_val >>= value; break;
      case binom::ValType::si64:    getArithmeticData().i64_val >>= value; break;
      case binom::ValType::f64:     getArithmeticData().f64_val >>= value; break;
      case binom::ValType::invalid_type:default: break;
      }
      return downcast();
    }
  }

  ArithmeticTypeDriven& operator++() noexcept {
    auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
    if(!downcast().checkLock(lk)) return downcast();
    switch (getValType()) {
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
    auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
    if(!downcast().checkLock(lk)) return downcast();
    switch (getValType()) {
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

  ArithmeticTypeDriven operator++(int) const noexcept {
    static_assert (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>);
    if constexpr (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>) {
      if(auto lk = downcast().getLock(priv::MtxLockType::unique_locked);!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
      else return ++ArithmeticTypeDriven(downcast());
    }
  }

  ArithmeticTypeDriven operator--(int) const noexcept {
    static_assert (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>);
    if constexpr (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>) {
      if(auto lk = downcast().getLock(priv::MtxLockType::unique_locked);!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
      else return --ArithmeticTypeDriven(downcast());
    }
  }

  template<typename T>
  ArithmeticTypeDriven operator+(T&& value) const noexcept {
    static_assert (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>);
    if constexpr (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>) {
      if(auto lk = downcast().getLock(priv::MtxLockType::unique_locked);!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
      else return ArithmeticTypeDriven(downcast()) += value;
    }
  }

  template<typename T>
  ArithmeticTypeDriven operator-(T&& value) const noexcept {
    static_assert (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>);
    if constexpr (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>) {
      if(auto lk = downcast().getLock(priv::MtxLockType::unique_locked);!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
      else return ArithmeticTypeDriven(downcast()) -= value;
    }
  }

  template<typename T>
  ArithmeticTypeDriven operator*(T&& value) const noexcept {
    static_assert (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>);
    if constexpr (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>) {
      if(auto lk = downcast().getLock(priv::MtxLockType::unique_locked);!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
      else return ArithmeticTypeDriven(downcast()) *= value;
    }
  }

  template<typename T>
  ArithmeticTypeDriven operator/(T&& value) const noexcept {
    auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
    if(!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
    return ArithmeticTypeDriven(downcast()) /= value;
  }

  template<typename T>
  ArithmeticTypeDriven operator%(T&& value) const noexcept {
    static_assert (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>);
    if constexpr (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>) {
      if(auto lk = downcast().getLock(priv::MtxLockType::unique_locked);!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
      else return ArithmeticTypeDriven(downcast()) %= value;
    }
  }

  template<typename T>
  ArithmeticTypeDriven operator|(T&& value) const noexcept {
    static_assert (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>);
    if constexpr (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>) {
      if(auto lk = downcast().getLock(priv::MtxLockType::unique_locked);!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
      else return ArithmeticTypeDriven(downcast()) |= value;
    }
  }

  template<typename T>
  ArithmeticTypeDriven operator^(T&& value) const noexcept {
    static_assert (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>);
    if constexpr (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>) {
      if(auto lk = downcast().getLock(priv::MtxLockType::unique_locked);!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
      else return ArithmeticTypeDriven(downcast()) ^= value;
    }
  }

  template<typename T>
  ArithmeticTypeDriven operator<<(T&& value) const noexcept {
    static_assert (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>);
    if constexpr (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>) {
      if(auto lk = downcast().getLock(priv::MtxLockType::unique_locked);!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
      else return ArithmeticTypeDriven(downcast()) <<= value;
    }
  }

  template<typename T>
  ArithmeticTypeDriven operator>>(T&& value) const noexcept {
    static_assert (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>);
    if constexpr (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>) {
      if(auto lk = downcast().getLock(priv::MtxLockType::unique_locked);!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
      else return ArithmeticTypeDriven(downcast()) >>= value;
    }
  }

  template<typename T>
  ArithmeticTypeDriven operator~() const noexcept {
    static_assert (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>);
    if constexpr (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>) {
      if(auto lk = downcast().getLock(priv::MtxLockType::unique_locked);!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
      else switch (ArithmeticTypeBase tmp(downcast()); getValType()) {
      case binom::ValType::boolean: return ArithmeticTypeDriven(~bool(downcast()));
      case binom::ValType::ui8: return ArithmeticTypeDriven(~ui8(downcast()));
      case binom::ValType::si8: return ArithmeticTypeDriven(~i8(downcast()));
      case binom::ValType::ui16: return ArithmeticTypeDriven(~ui16(downcast()));
      case binom::ValType::si16: return ArithmeticTypeDriven(~i16(downcast()));
      case binom::ValType::ui32: return ArithmeticTypeDriven(~ui32(downcast()));
      case binom::ValType::si32: return ArithmeticTypeDriven(~i32(downcast()));
      case binom::ValType::f32: return ArithmeticTypeDriven(f32(~i32(downcast())));
      case binom::ValType::ui64: return ArithmeticTypeDriven(~ui64(downcast()));
      case binom::ValType::si64: return ArithmeticTypeDriven(~i64(downcast()));
      case binom::ValType::f64: return ArithmeticTypeDriven(f64(~i64(downcast())));
      case binom::ValType::invalid_type: return ArithmeticTypeDriven(downcast());
      }
    }
  }

  template<typename T>
  ArithmeticTypeDriven operator-() const noexcept {
    static_assert (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>);
    if constexpr (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>) {
      if(auto lk = downcast().getLock(priv::MtxLockType::unique_locked);!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
      else switch (ArithmeticTypeBase tmp(downcast()); getValType()) {
      case binom::ValType::boolean: return ArithmeticTypeDriven(bool(-bool(downcast())));
      case binom::ValType::ui8: return ArithmeticTypeDriven(i16(-ui8(downcast())));
      case binom::ValType::si8: return ArithmeticTypeDriven(i8(-i8(downcast())));
      case binom::ValType::ui16: return ArithmeticTypeDriven(i32(-ui16(downcast())));
      case binom::ValType::si16: return ArithmeticTypeDriven(i16(-i16(downcast())));
      case binom::ValType::ui32: return ArithmeticTypeDriven(i64(-ui32(downcast())));
      case binom::ValType::si32: return ArithmeticTypeDriven(i32(-i32(downcast())));
      case binom::ValType::f32: return ArithmeticTypeDriven(f32(-f32(downcast())));
      case binom::ValType::ui64: return ArithmeticTypeDriven(i64(-ui64(downcast())));
      case binom::ValType::si64: return ArithmeticTypeDriven(i64(-i64(downcast())));
      case binom::ValType::f64: return ArithmeticTypeDriven(f64(-f64(downcast())));
      case binom::ValType::invalid_type: return ArithmeticTypeDriven(downcast());
      }
    }
  }

  template<typename T>
  ArithmeticTypeDriven operator+() const noexcept {
    static_assert (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>);
    if constexpr (is_base_of_v<EnableCopyableArithmetic, ArithmeticTypeDriven>) {
      if(auto lk = downcast().getLock(priv::MtxLockType::unique_locked);!downcast().checkLock(lk)) return ArithmeticTypeDriven(downcast());
      else switch (ArithmeticTypeBase tmp(downcast()); getValType()) {
      case binom::ValType::boolean: return ArithmeticTypeDriven(bool(+bool(downcast())));
      case binom::ValType::ui8: return ArithmeticTypeDriven(ui8(+ui8(downcast())));
      case binom::ValType::si8: return ArithmeticTypeDriven(i8(+i8(downcast())));
      case binom::ValType::ui16: return ArithmeticTypeDriven(ui16(+ui16(downcast())));
      case binom::ValType::si16: return ArithmeticTypeDriven(i16(+i16(downcast())));
      case binom::ValType::ui32: return ArithmeticTypeDriven(ui32(+ui32(downcast())));
      case binom::ValType::si32: return ArithmeticTypeDriven(i32(+i32(downcast())));
      case binom::ValType::f32: return ArithmeticTypeDriven(f32(+f32(downcast())));
      case binom::ValType::ui64: return ArithmeticTypeDriven(ui64(+ui64(downcast())));
      case binom::ValType::si64: return ArithmeticTypeDriven(i64(+i64(downcast())));
      case binom::ValType::f64: return ArithmeticTypeDriven(f64(+f64(downcast())));
      case binom::ValType::invalid_type: return ArithmeticTypeDriven(downcast());
      }
    }
  }

};

template <typename ArithmeticTypeDriven>
class CastableArithmeticTypeBase {

  ArithmeticTypeDriven& downcast() noexcept {return *reinterpret_cast<ArithmeticTypeDriven*>(this);}
  const ArithmeticTypeDriven& downcast() const noexcept {return *reinterpret_cast<const ArithmeticTypeDriven*>(this);}

  inline void setType(ValType type) noexcept {downcast().setTypeImpl(type);}
  inline void reallocate(ValType type) noexcept {downcast().reallocateImpl(type);}
  inline ArithmeticData& getArithmeticData() noexcept {return downcast().getArithmeticDataImpl();}

protected:
  void reallocateImpl([[maybe_unused]] ValType type) noexcept {}
  void setTypeImpl([[maybe_unused]] ValType type) noexcept {}

public:

  ArithmeticTypeDriven& castTo(ValType new_type) noexcept {
    auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
    if(!downcast().checkLock(lk)) return downcast();
    reallocate(new_type);
    switch (downcast().getValType()) {
    case binom::ValType::boolean:
      switch (new_type) {
      case binom::ValType::boolean: return downcast();
      case binom::ValType::ui8: getArithmeticData().ui8_val = static_cast<ui8>(getArithmeticData().bool_val); break;
      case binom::ValType::si8: getArithmeticData().i16_val = static_cast<i8>(getArithmeticData().bool_val); break;
      case binom::ValType::ui16: getArithmeticData().ui16_val = static_cast<ui16>(getArithmeticData().bool_val); break;
      case binom::ValType::si16: getArithmeticData().i16_val = static_cast<i16>(getArithmeticData().bool_val); break;
      case binom::ValType::ui32: getArithmeticData().ui32_val = static_cast<ui32>(getArithmeticData().bool_val); break;
      case binom::ValType::si32: getArithmeticData().i32_val = static_cast<i32>(getArithmeticData().bool_val); break;
      case binom::ValType::f32: getArithmeticData().f32_val = static_cast<f32>(getArithmeticData().bool_val); break;
      case binom::ValType::ui64: getArithmeticData().ui64_val = static_cast<ui64>(getArithmeticData().bool_val); break;
      case binom::ValType::si64: getArithmeticData().i64_val = static_cast<i64>(getArithmeticData().bool_val); break;
      case binom::ValType::f64: getArithmeticData().f64_val = static_cast<f64>(getArithmeticData().bool_val); break;
      case binom::ValType::invalid_type:default: return downcast();
      }
    break;
    case binom::ValType::ui8:
      switch (new_type) {
      case binom::ValType::boolean: getArithmeticData().bool_val = static_cast<bool>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui8: return downcast();
      case binom::ValType::si8: getArithmeticData().i16_val = static_cast<i8>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui16: getArithmeticData().ui16_val = static_cast<ui16>(getArithmeticData().ui8_val); break;
      case binom::ValType::si16: getArithmeticData().i16_val = static_cast<i16>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui32: getArithmeticData().ui32_val = static_cast<ui32>(getArithmeticData().ui8_val); break;
      case binom::ValType::si32: getArithmeticData().i32_val = static_cast<i32>(getArithmeticData().ui8_val); break;
      case binom::ValType::f32: getArithmeticData().f32_val = static_cast<f32>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui64: getArithmeticData().ui64_val = static_cast<ui64>(getArithmeticData().ui8_val); break;
      case binom::ValType::si64: getArithmeticData().i64_val = static_cast<i64>(getArithmeticData().ui8_val); break;
      case binom::ValType::f64: getArithmeticData().f64_val = static_cast<f64>(getArithmeticData().ui8_val); break;
      case binom::ValType::invalid_type:default: return downcast();
      }
    break;
    case binom::ValType::si8:
      switch (new_type) {
      case binom::ValType::boolean: getArithmeticData().bool_val = static_cast<bool>(getArithmeticData().ui8_val);  break;
      case binom::ValType::ui8: getArithmeticData().ui8_val = static_cast<ui8>(getArithmeticData().ui8_val); break;
      case binom::ValType::si8: return downcast();
      case binom::ValType::ui16: getArithmeticData().ui16_val = static_cast<ui16>(getArithmeticData().ui8_val); break;
      case binom::ValType::si16: getArithmeticData().i16_val = static_cast<i16>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui32: getArithmeticData().ui32_val = static_cast<ui32>(getArithmeticData().ui8_val); break;
      case binom::ValType::si32: getArithmeticData().i32_val = static_cast<i32>(getArithmeticData().ui8_val); break;
      case binom::ValType::f32: getArithmeticData().f32_val = static_cast<f32>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui64: getArithmeticData().ui64_val = static_cast<ui64>(getArithmeticData().ui8_val); break;
      case binom::ValType::si64: getArithmeticData().i64_val = static_cast<i64>(getArithmeticData().ui8_val); break;
      case binom::ValType::f64: getArithmeticData().f64_val = static_cast<f64>(getArithmeticData().ui8_val); break;
      case binom::ValType::invalid_type:default: return downcast();
      }
    break;
    case binom::ValType::ui16:
      switch (new_type) {
      case binom::ValType::boolean: getArithmeticData().bool_val = static_cast<bool>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui8: getArithmeticData().ui8_val = static_cast<ui8>(getArithmeticData().ui8_val); break;
      case binom::ValType::si8: getArithmeticData().i16_val = static_cast<i8>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui16: return downcast();
      case binom::ValType::si16: getArithmeticData().i16_val = static_cast<i16>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui32: getArithmeticData().ui32_val = static_cast<ui32>(getArithmeticData().ui8_val); break;
      case binom::ValType::si32: getArithmeticData().i32_val = static_cast<i32>(getArithmeticData().ui8_val); break;
      case binom::ValType::f32: getArithmeticData().f32_val = static_cast<f32>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui64: getArithmeticData().ui64_val = static_cast<ui64>(getArithmeticData().ui8_val); break;
      case binom::ValType::si64: getArithmeticData().i64_val = static_cast<i64>(getArithmeticData().ui8_val); break;
      case binom::ValType::f64: getArithmeticData().f64_val = static_cast<f64>(getArithmeticData().ui8_val); break;
      case binom::ValType::invalid_type:default: return downcast();
      }
    break;
    case binom::ValType::si16:
      switch (new_type) {
      case binom::ValType::boolean: getArithmeticData().bool_val = static_cast<bool>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui8: getArithmeticData().ui8_val = static_cast<ui8>(getArithmeticData().ui8_val); break;
      case binom::ValType::si8: getArithmeticData().i16_val = static_cast<i8>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui16: getArithmeticData().ui16_val = static_cast<ui16>(getArithmeticData().ui8_val); break;
      case binom::ValType::si16: return downcast();
      case binom::ValType::ui32: getArithmeticData().ui32_val = static_cast<ui32>(getArithmeticData().ui8_val); break;
      case binom::ValType::si32: getArithmeticData().i32_val = static_cast<i32>(getArithmeticData().ui8_val); break;
      case binom::ValType::f32: getArithmeticData().f32_val = static_cast<f32>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui64: getArithmeticData().ui64_val = static_cast<ui64>(getArithmeticData().ui8_val); break;
      case binom::ValType::si64: getArithmeticData().i64_val = static_cast<i64>(getArithmeticData().ui8_val); break;
      case binom::ValType::f64: getArithmeticData().f64_val = static_cast<f64>(getArithmeticData().ui8_val); break;
      case binom::ValType::invalid_type:default: return downcast();
      }
    break;
    case binom::ValType::ui32:
      switch (new_type) {
      case binom::ValType::boolean: getArithmeticData().bool_val = static_cast<bool>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui8: getArithmeticData().ui8_val = static_cast<ui8>(getArithmeticData().ui8_val); break;
      case binom::ValType::si8: getArithmeticData().i16_val = static_cast<i8>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui16: getArithmeticData().ui16_val = static_cast<ui16>(getArithmeticData().ui8_val); break;
      case binom::ValType::si16: getArithmeticData().i16_val = static_cast<i16>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui32: return downcast();
      case binom::ValType::si32: getArithmeticData().i32_val = static_cast<i32>(getArithmeticData().ui8_val); break;
      case binom::ValType::f32: getArithmeticData().f32_val = static_cast<f32>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui64: getArithmeticData().ui64_val = static_cast<ui64>(getArithmeticData().ui8_val); break;
      case binom::ValType::si64: getArithmeticData().i64_val = static_cast<i64>(getArithmeticData().ui8_val); break;
      case binom::ValType::f64: getArithmeticData().f64_val = static_cast<f64>(getArithmeticData().ui8_val); break;
      case binom::ValType::invalid_type:default: return downcast();
      }
    break;
    case binom::ValType::si32:
      switch (new_type) {
      case binom::ValType::boolean: getArithmeticData().bool_val = static_cast<bool>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui8: getArithmeticData().ui8_val = static_cast<ui8>(getArithmeticData().ui8_val); break;
      case binom::ValType::si8: getArithmeticData().i16_val = static_cast<i8>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui16: getArithmeticData().ui16_val = static_cast<ui16>(getArithmeticData().ui8_val); break;
      case binom::ValType::si16: getArithmeticData().i16_val = static_cast<i16>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui32: getArithmeticData().ui32_val = static_cast<ui32>(getArithmeticData().ui8_val); break;
      case binom::ValType::si32: return downcast();
      case binom::ValType::f32: getArithmeticData().f32_val = static_cast<f32>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui64: getArithmeticData().ui64_val = static_cast<ui64>(getArithmeticData().ui8_val); break;
      case binom::ValType::si64: getArithmeticData().i64_val = static_cast<i64>(getArithmeticData().ui8_val); break;
      case binom::ValType::f64: getArithmeticData().f64_val = static_cast<f64>(getArithmeticData().ui8_val); break;
      case binom::ValType::invalid_type:default: return downcast();
      }
    break;
    case binom::ValType::f32:
      switch (new_type) {
      case binom::ValType::boolean: getArithmeticData().bool_val = static_cast<bool>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui8: getArithmeticData().ui8_val = static_cast<ui8>(getArithmeticData().ui8_val); break;
      case binom::ValType::si8: getArithmeticData().i16_val = static_cast<i8>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui16: getArithmeticData().ui16_val = static_cast<ui16>(getArithmeticData().ui8_val); break;
      case binom::ValType::si16: getArithmeticData().i16_val = static_cast<i16>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui32: getArithmeticData().ui32_val = static_cast<ui32>(getArithmeticData().ui8_val); break;
      case binom::ValType::si32: getArithmeticData().i32_val = static_cast<i32>(getArithmeticData().ui8_val); break;
      case binom::ValType::f32: return downcast();
      case binom::ValType::ui64: getArithmeticData().ui64_val = static_cast<ui64>(getArithmeticData().ui8_val); break;
      case binom::ValType::si64: getArithmeticData().i64_val = static_cast<i64>(getArithmeticData().ui8_val); break;
      case binom::ValType::f64: getArithmeticData().f64_val = static_cast<f64>(getArithmeticData().ui8_val); break;
      case binom::ValType::invalid_type:default: return downcast();
      }
    break;
    case binom::ValType::ui64:
      switch (new_type) {
      case binom::ValType::boolean: getArithmeticData().bool_val = static_cast<bool>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui8: getArithmeticData().ui8_val = static_cast<ui8>(getArithmeticData().ui8_val); break;
      case binom::ValType::si8: getArithmeticData().i16_val = static_cast<i8>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui16: getArithmeticData().ui16_val = static_cast<ui16>(getArithmeticData().ui8_val); break;
      case binom::ValType::si16: getArithmeticData().i16_val = static_cast<i16>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui32: getArithmeticData().ui32_val = static_cast<ui32>(getArithmeticData().ui8_val); break;
      case binom::ValType::si32: getArithmeticData().i32_val = static_cast<i32>(getArithmeticData().ui8_val); break;
      case binom::ValType::f32: getArithmeticData().f32_val = static_cast<f32>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui64: return downcast();
      case binom::ValType::si64: getArithmeticData().i64_val = static_cast<i64>(getArithmeticData().ui8_val); break;
      case binom::ValType::f64: getArithmeticData().f64_val = static_cast<f64>(getArithmeticData().ui8_val); break;
      case binom::ValType::invalid_type:default: return downcast();
      }
    break;
    case binom::ValType::si64:
      switch (new_type) {
      case binom::ValType::boolean: getArithmeticData().bool_val = static_cast<bool>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui8: getArithmeticData().ui8_val = static_cast<ui8>(getArithmeticData().ui8_val); break;
      case binom::ValType::si8: getArithmeticData().i16_val = static_cast<i8>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui16: getArithmeticData().ui16_val = static_cast<ui16>(getArithmeticData().ui8_val); break;
      case binom::ValType::si16: getArithmeticData().i16_val = static_cast<i16>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui32: getArithmeticData().ui32_val = static_cast<ui32>(getArithmeticData().ui8_val); break;
      case binom::ValType::si32: getArithmeticData().i32_val = static_cast<i32>(getArithmeticData().ui8_val); break;
      case binom::ValType::f32: getArithmeticData().f32_val = static_cast<f32>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui64: getArithmeticData().ui64_val = static_cast<ui64>(getArithmeticData().ui8_val); break;
      case binom::ValType::si64: return downcast();
      case binom::ValType::f64: getArithmeticData().f64_val = static_cast<f64>(getArithmeticData().ui8_val); break;
      case binom::ValType::invalid_type:default: return downcast();
      }
    break;
    case binom::ValType::f64:
      switch (new_type) {
      case binom::ValType::boolean: getArithmeticData().bool_val = static_cast<bool>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui8: getArithmeticData().ui8_val = static_cast<ui8>(getArithmeticData().ui8_val); break;
      case binom::ValType::si8: getArithmeticData().i16_val = static_cast<i8>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui16: getArithmeticData().ui16_val = static_cast<ui16>(getArithmeticData().ui8_val); break;
      case binom::ValType::si16: getArithmeticData().i16_val = static_cast<i16>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui32: getArithmeticData().ui32_val = static_cast<ui32>(getArithmeticData().ui8_val); break;
      case binom::ValType::si32: getArithmeticData().i32_val = static_cast<i32>(getArithmeticData().ui8_val); break;
      case binom::ValType::f32: getArithmeticData().f32_val = static_cast<f32>(getArithmeticData().ui8_val); break;
      case binom::ValType::ui64: getArithmeticData().ui64_val = static_cast<ui64>(getArithmeticData().ui8_val); break;
      case binom::ValType::si64: getArithmeticData().i64_val = static_cast<i64>(getArithmeticData().ui8_val); break;
      case binom::ValType::f64: return downcast();
      case binom::ValType::invalid_type:default: return downcast();
      }
    break;
    case binom::ValType::invalid_type:default:
    break;
    }
    setType(new_type);
    return downcast();
  }

  ArithmeticTypeDriven& reinterpretCastTo(ValType new_type) noexcept {
    auto lk = downcast().getLock(priv::MtxLockType::unique_locked);
    if(!downcast().checkLock(lk)) return downcast();
    reallocate(new_type);
    setType(new_type);
    return downcast();
  }

};

#define USE_ARITHMETIC \
  friend class ArithmeticTypeBase;

#define USE_ARITHMETIC_CAST \
  friend class CastableArithmeticTypeBase;

#define USE_COPYABLE_ARITHMETIC \
  friend class CopyableArithmeticTypeBase;

}

#endif // GENERIC_ARITHMETIC_H
