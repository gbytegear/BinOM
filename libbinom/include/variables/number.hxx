#ifndef NUMBER_HXX
#define NUMBER_HXX

#include "variable.hxx"
#include "generic_value.hxx"

namespace binom {

class Number :
    public arithmetic::ArithmeticTypeBase<Number, OptionalSharedRecursiveLock>,
    public arithmetic::CopyableArithmeticTypeBase<Number, OptionalSharedRecursiveLock>,
    public arithmetic::CastableArithmeticTypeBase<Number, OptionalSharedRecursiveLock>,
    public Variable {
  USE_ARITHMETIC
  USE_ARITHMETIC_CAST
  using Variable::getLock;
  using Variable::getValType;

  arithmetic::ArithmeticData& getArithmeticDataImpl() const {return *reinterpret_cast<arithmetic::ArithmeticData*>(&resource_link->data);}
  ValType getValTypeImpl() const {return getValType();}

public:
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

};

}

#endif // NUMBER_HXX
