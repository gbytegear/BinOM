#ifndef NUMBER_HXX
#define NUMBER_HXX

#include "variable.hxx"

namespace binom {

class Number :
    public arithmetic::ArithmeticTypeBase<Number>,
    arithmetic::EnableCopyableArithmetic,
    public arithmetic::CastableArithmeticTypeBase<Number>,
    public Variable {
  USE_ARITHMETIC
  USE_ARITHMETIC_CAST

  operator Number& () = delete;
  operator BitArray& () = delete;
  operator BufferArray& () = delete;
  operator Array& () = delete;
  operator List& () = delete;
  operator Map& () = delete;

  inline Number& toNumber() = delete;
  inline BitArray& toBitArray() = delete;
  inline BufferArray& toBufferArray() = delete;
  inline Array& toArray() = delete;
  inline List& toList() = delete;
  inline Map& toMap() = delete;

  arithmetic::ArithmeticData& getArithmeticDataImpl() const {return *reinterpret_cast<arithmetic::ArithmeticData*>(&resource_link->data);}
  ValType getValTypeImpl() const {return getValType();}

  static bool checkLock(const OptionalSharedRecursiveLock& lock) noexcept {return lock.has_value();}

  void setTypeImpl(ValType new_type) noexcept {
    resource_link->type = toVarType(new_type);
  }

  friend class Variable;
  Number(priv::Link&& link) : Variable(std::move(link)) {}

public:
  using Variable::getLock;
  using Variable::getValType;
  using Variable::getBitWidth;
  using Variable::getNumberType;

  Number() noexcept :             Variable(int(0)) {}
  Number(bool value) noexcept :   Variable(value) {}
  Number(ui8 value) noexcept :    Variable(value) {}
  Number(i8 value) noexcept :     Variable(value) {}
  Number(ui16 value) noexcept :   Variable(value) {}
  Number(i16 value) noexcept :    Variable(value) {}
  Number(ui32 value) noexcept :   Variable(value) {}
  Number(i32 value) noexcept :    Variable(value) {}
  Number(f32 value) noexcept :    Variable(value) {}
  Number(ui64 value) noexcept :   Variable(value) {}
  Number(i64 value) noexcept :    Variable(value) {}
  Number(f64 value) noexcept :    Variable(value) {}

  Number(const Number& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
  Number(Number&& other) noexcept : Variable(dynamic_cast<Variable&&>(other)) {}

  Number(const GenericValue& value) noexcept : Variable(value) {}
  Number(GenericValue&& value) noexcept : Variable(std::move(value)) {}

  using arithmetic::ArithmeticTypeBase<Number>::operator=;
};

}

#endif // NUMBER_HXX
