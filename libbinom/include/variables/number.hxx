#ifndef NUMBER_HXX
#define NUMBER_HXX

#include "../utils/resource_control.hxx"
#include "generic_value.hxx"

namespace binom {

class Number {
  priv::Link res_link;
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

  VarType getType() const noexcept;
  VarBitWidth getBitWidth() const noexcept;
  VarNumberType getNumberType() const noexcept;

  ui8 getMemebrSize() const noexcept;


};

}

#endif // NUMBER_HXX
