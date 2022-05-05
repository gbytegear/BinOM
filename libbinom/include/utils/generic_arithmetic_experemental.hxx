#ifndef GENERIC_ARITHMETIC_EXPEREMENTAL_H
#define GENERIC_ARITHMETIC_EXPEREMENTAL_H

/* NOTE: Два амперсанда в функциях(T&&) это x-value, а не r-value, еблан дремучий!
 * Ты блять знаешь это и вместо того чтобы писать номальный код 2 часа себе голову ебёшь!
 */

#include "types.hxx"
#include "shared_recursive_mutex_wrapper.hxx"
#include <cmath>

namespace binom::arithmetic::experemental {

template <class T>
struct remove_cvref : std::remove_cv<std::remove_reference_t<T>> {};

template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <template<typename T> class CRTP_Base, class CRTP_Driven>
struct is_crtp_base_of : std::is_base_of<CRTP_Base<remove_cvref_t<CRTP_Driven>>, remove_cvref_t<CRTP_Driven>> {};

template <template<typename T> class CRTP_Base, class CRTP_Driven>
inline constexpr bool is_crtp_base_of_v = is_crtp_base_of<CRTP_Base, CRTP_Driven>::value;

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
  operator T() const {
    static_assert (std::is_arithmetic<T>::value);
    if constexpr (std::is_arithmetic<T>::value) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getValType()) {
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
  }

  template<typename T>
  bool operator==(const T&& value) const {
    static_assert (is_crtp_base_of_v<ArithmeticTypeBase, T> || is_arithmetic_without_cvref_v<T>);
    if constexpr (is_crtp_base_of_v<ArithmeticTypeBase, T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getNumberType()) {
        case binom::VarNumberType::unsigned_integer:
          switch (value.getNumberType()) {
            case binom::VarNumberType::unsigned_integer: return ui64(*this) == ui64(value);
            case binom::VarNumberType::signed_integer: return i64(value) >= 0 ? ui64(*this) == ui64(value) : false;
            case binom::VarNumberType::float_point: return ui64(*this) == f64(value);
            default:
            case binom::VarNumberType::invalid_type: return false;
          }
        case binom::VarNumberType::signed_integer:
          switch (value.getNumberType()) {
            case binom::VarNumberType::unsigned_integer: return i64(*this) >= 0 ? ui64(*this) == ui64(value) : false;
            case binom::VarNumberType::signed_integer: return i64(*this) == i64(value);
            case binom::VarNumberType::float_point: return i64(*this) == f64(value);
            default:
            case binom::VarNumberType::invalid_type: return false;
          }
        case binom::VarNumberType::float_point:
          switch (value.getNumberType()) {
            case binom::VarNumberType::unsigned_integer: return f64(*this) == ui64(value);
            case binom::VarNumberType::signed_integer: return f64(*this) == ui64(value);
            case binom::VarNumberType::float_point: return f64(*this) == f64(value);
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
      case binom::VarNumberType::unsigned_integer: return ui64(*this) == value;
      case binom::VarNumberType::signed_integer: return i64(*this) >= 0 ? ui64(*this) == value : false;
      case binom::VarNumberType::float_point: return f64(*this) == value;
      case binom::VarNumberType::invalid_type: return false;
      }
    } else if constexpr (is_signed_without_cvref_v<T>) {
      auto lk = downcast().getLock(priv::MtxLockType::shared_locked);
      if(!downcast().checkLock(lk)) return false;
      switch (getNumberType()) {
      case binom::VarNumberType::unsigned_integer: return value >= 0 ? ui64(*this) == value : false;
      case binom::VarNumberType::signed_integer: return ui64(*this) == value;
      case binom::VarNumberType::float_point: return f64(*this) == value;
      case binom::VarNumberType::invalid_type: return false;
      }
    }
  }

  template<typename T>
  bool operator!=(const T&& value) const {

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


};


}

#endif // GENERIC_ARITHMETIC_EXPEREMENTAL_H
