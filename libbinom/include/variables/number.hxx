#ifndef NUMBER_HXX
#define NUMBER_HXX

#include "vaiable_base.hxx"
#include "generic_value.hxx"

namespace binom {

class Number :
    public arithmetic::ArithmeticTypeBase<Number, priv::OptionalSharedRecursiveLock>,
    public arithmetic::CopyableArithmeticTypeBase<Number, priv::OptionalSharedRecursiveLock>,
    public priv::VariableBase<Number> {

public:
  Number(VarType type) noexcept;
  Number(bool value) noexcept;
  Number(ui8 value) noexcept;
  Number(i8 value) noexcept;
  Number(ui16 value) noexcept;
  Number(i16 value) noexcept;
  Number(ui32 value) noexcept;
  Number(i32 value) noexcept;
  Number(f32 value) noexcept;
  Number(ui64 value) noexcept;
  Number(f64 value) noexcept;

  ~Number();

};

}

#endif // NUMBER_HXX
