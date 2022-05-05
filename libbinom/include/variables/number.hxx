#ifndef NUMBER_HXX
#define NUMBER_HXX

#include "variable.hxx"

namespace binom {

class Number :
    public arithmetic::ArithmeticTypeBase<Number, OptionalSharedRecursiveLock>,
    public arithmetic::CopyableArithmeticTypeBase<Number, OptionalSharedRecursiveLock>,
    public arithmetic::CastableArithmeticTypeBase<Number, OptionalSharedRecursiveLock>,
    public Variable {
  USE_ARITHMETIC
  USE_ARITHMETIC_CAST

  arithmetic::ArithmeticData& getArithmeticDataImpl() const {return *reinterpret_cast<arithmetic::ArithmeticData*>(&resource_link->data);}
  ValType getValTypeImpl() const {return getValType();}

  void setTypeImpl(ValType new_type) noexcept {
    resource_link->type = toVarType(new_type);
  }

public:
  using Variable::getLock;
  using Variable::getValType;
  using Variable::getBitWidth;
  using Variable::getNumberType;

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

  Number& operator=(bool value) noexcept {return ArithmeticTypeBase::operator=(value);}
  Number& operator=(ui8 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
  Number& operator=(i8 value) noexcept   {return ArithmeticTypeBase::operator=(value);}
  Number& operator=(ui16 value) noexcept {return ArithmeticTypeBase::operator=(value);}
  Number& operator=(i16 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
  Number& operator=(ui32 value) noexcept {return ArithmeticTypeBase::operator=(value);}
  Number& operator=(i32 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
  Number& operator=(f32 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
  Number& operator=(ui64 value) noexcept {return ArithmeticTypeBase::operator=(value);}
  Number& operator=(i64 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
  Number& operator=(f64 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
  Number& operator=(const Number& value) noexcept {return ArithmeticTypeBase::operator=(value);}
  Number& operator=(Number&& value) noexcept      {return ArithmeticTypeBase::operator=(std::move(value));}

  operator GenericValue() {
    if(auto lk = getLock(MtxLockType::shared_locked); lk)
      return GenericValue(getValType(), getArithmeticDataImpl());
    else return GenericValue();
  }

  operator GenericValueRef() {
    if(auto lk = getLock(MtxLockType::shared_locked); lk)
      return GenericValueRef(toValueType(resource_link->type), &resource_link->data, this);
    throw Error(ErrorType::binom_resource_not_available);
  }
};

}

#endif // NUMBER_HXX
