#include "libbinom/include/variables/number.hxx"

using namespace binom;
using namespace binom::priv;

arithmetic::ArithmeticData& Number::getArithmeticDataImpl() const {return *reinterpret_cast<arithmetic::ArithmeticData*>(&resource_link->data);}
ValType Number::getValTypeImpl() const {return getValType();}
bool Number::checkLock(const OptionalSharedRecursiveLock& lock) noexcept {return lock.has_value();}

void Number::setTypeImpl(ValType new_type) noexcept {
  resource_link->type = toVarType(new_type);
}

Number::Number(priv::Link&& link) : Variable(std::move(link)) {}
Number::Number() noexcept :             Variable(int(0)) {}
Number::Number(bool value) noexcept :   Variable(value) {}
Number::Number(ui8 value) noexcept :    Variable(value) {}
Number::Number(i8 value) noexcept :     Variable(value) {}
Number::Number(ui16 value) noexcept :   Variable(value) {}
Number::Number(i16 value) noexcept :    Variable(value) {}
Number::Number(ui32 value) noexcept :   Variable(value) {}
Number::Number(i32 value) noexcept :    Variable(value) {}
Number::Number(f32 value) noexcept :    Variable(value) {}
Number::Number(ui64 value) noexcept :   Variable(value) {}
Number::Number(i64 value) noexcept :    Variable(value) {}
Number::Number(f64 value) noexcept :    Variable(value) {}
Number::Number(const Number& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
Number::Number(const Number&& other) noexcept : Variable(dynamic_cast<const Variable&&>(other)) {}
Number::Number(const GenericValue& value) noexcept : Variable(value) {}
Number::Number(const GenericValue&& value) noexcept : Variable(std::move(value)) {}

Number Number::getReference() noexcept {return Link(resource_link);}

Number& Number::changeLink(const Number& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  this->~Number();
  return *new(this) Number(other);
}

Number& Number::changeLink(Number&& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  this->~Number();
  return *new(this) Number(std::move(other));
}
