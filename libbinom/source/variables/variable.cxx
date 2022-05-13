#include "libbinom/include/variables/variable.hxx"
#include "libbinom/include/variables/number.hxx"
#include "libbinom/include/variables/bit_array.hxx"

using namespace binom;
using namespace binom::priv;

SharedResource::~SharedResource() {Variable::destroyResorce(resource_data);}

void Variable::destroyResorce(priv::ResourceData res_data) {
  switch (toTypeClass(res_data.type)) {
  case binom::VarTypeClass::null:
  case binom::VarTypeClass::number: return;
  case binom::VarTypeClass::bit_array: delete res_data.data.bit_array_header; return;
  case binom::VarTypeClass::buffer_array: return; // TODO
  case binom::VarTypeClass::array: return; // TODO
  case binom::VarTypeClass::list: return; // TODO
  case binom::VarTypeClass::map: return; // TODO
  case binom::VarTypeClass::invalid_type: default:
  return;
  }
}

Link Variable::cloneResource(priv::Link resource_link) noexcept {
  switch (toTypeClass(resource_link.getType())) {
  case binom::VarTypeClass::null:
  case binom::VarTypeClass::number:
  return **resource_link;

  case binom::VarTypeClass::bit_array:
  return ResourceData{VarType::bit_array, {.bit_array_header = BitArrayHeader::copy(resource_link->data.bit_array_header)}};

  case binom::VarTypeClass::buffer_array: // TODO
  case binom::VarTypeClass::array: // TODO
  case binom::VarTypeClass::list: // TODO
  case binom::VarTypeClass::map: // TODO
  default:
  case binom::VarTypeClass::invalid_type:
  break;
  }
  return ResourceData{VarType::null, {}};
}

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

Variable::operator Number&() {
  if(getTypeClass() == VarTypeClass::number) return reinterpret_cast<Number&>(self);
  throw Error(ErrorType::binom_invalid_type);
}

Variable::operator BitArray&() {
  if(getTypeClass() == VarTypeClass::bit_array) return reinterpret_cast<BitArray&>(self);
  throw Error(ErrorType::binom_invalid_type);
}

Variable::operator BufferArray&() {
  if(getTypeClass() == VarTypeClass::buffer_array) return reinterpret_cast<BufferArray&>(self);
  throw Error(ErrorType::binom_invalid_type);
}

Variable::operator Array&() {
  if(getTypeClass() == VarTypeClass::array) return reinterpret_cast<Array&>(self);
  throw Error(ErrorType::binom_invalid_type);
}
