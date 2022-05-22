#include "libbinom/include/variables/variable.hxx"
#include "libbinom/include/variables/number.hxx"
#include "libbinom/include/variables/bit_array.hxx"
#include "libbinom/include/variables/buffer_array.hxx"
#include "libbinom/include/variables/array.hxx"
#include "libbinom/include/variables/list.hxx"
#include "libbinom/include/variables/map.hxx"

using namespace binom;
using namespace binom::priv;

size_t Variable::getElementCount() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  switch (getTypeClass()) {
  case binom::VarTypeClass::null: return 0;
  case binom::VarTypeClass::number: return 1;
  case binom::VarTypeClass::bit_array: return BitArray(const_cast<Variable&>(self)).getSize();
  case binom::VarTypeClass::buffer_array:
  break;
  case binom::VarTypeClass::array:
  break;
  case binom::VarTypeClass::list:
  break;
  case binom::VarTypeClass::map:
  break;
  case binom::VarTypeClass::invalid_type:
  break;
  }
  return -1;
}

Number Variable::createRefVariable(const Number& other) noexcept {return Link(other.resource_link);}

Variable::operator Number&() {
  if(getTypeClass() == VarTypeClass::number) return reinterpret_cast<Number&>(self);
  throw Error(ErrorType::binom_invalid_type);
}

BitArray Variable::createRefVariable(const BitArray& other) noexcept {return Link(other.resource_link);}

Variable::operator BitArray&() {
  if(getTypeClass() == VarTypeClass::bit_array) return reinterpret_cast<BitArray&>(self);
  throw Error(ErrorType::binom_invalid_type);
}

BufferArray Variable::createRefVariable(const BufferArray& other) noexcept {return Link(other.resource_link);}

Variable::operator BufferArray&() {
  if(getTypeClass() == VarTypeClass::buffer_array) return reinterpret_cast<BufferArray&>(self);
  throw Error(ErrorType::binom_invalid_type);
}

Array Variable::createRefVariable(const Array& other) noexcept {/* TODO */}

Variable::operator Array&() {
  if(getTypeClass() == VarTypeClass::array) return reinterpret_cast<Array&>(self);
  throw Error(ErrorType::binom_invalid_type);
}

List Variable::createRefVariable(const List& other) noexcept {/* TODO */}

Variable::operator List&() {
  if(getTypeClass() == VarTypeClass::list) return reinterpret_cast<List&>(self);
  throw Error(ErrorType::binom_invalid_type);
}

Map Variable::createRefVariable(const Map& other) noexcept {/* TODO */}

Variable::operator Map&() {
  if(getTypeClass() == VarTypeClass::map) return reinterpret_cast<Map&>(self);
  throw Error(ErrorType::binom_invalid_type);
}

