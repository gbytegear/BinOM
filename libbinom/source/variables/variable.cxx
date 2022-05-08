#include "libbinom/include/variables/variable.hxx"
#include "libbinom/include/variables/number.hxx"

using namespace binom;
using namespace binom::priv;

SharedResource::~SharedResource() {Variable::destroyResorce(resource_data);}

void Variable::destroyResorce(priv::ResourceData res_data) {
  switch (toTypeClass(res_data.type)) {
  case binom::VarTypeClass::null:
  case binom::VarTypeClass::number: return;
  case binom::VarTypeClass::buffer_array: // TODO
  break;
  case binom::VarTypeClass::array: // TODO
  break;
  case binom::VarTypeClass::list: // TODO
  break;
  case binom::VarTypeClass::map: // TODO
  break;
  default:
  case binom::VarTypeClass::invalid_type:
  break;

  }
}

Link Variable::cloneResource(priv::Link resource_link) noexcept {
  switch (toTypeClass(resource_link.getType())) {
  case binom::VarTypeClass::null:
  case binom::VarTypeClass::number: return **resource_link;
  case binom::VarTypeClass::buffer_array: // TODO
  break;
  case binom::VarTypeClass::array: // TODO
  break;
  case binom::VarTypeClass::list: // TODO
  break;
  case binom::VarTypeClass::map: // TODO
  break;
  default:
  case binom::VarTypeClass::invalid_type:
  break;
  }
}

size_t Variable::getElementCount() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  switch (getTypeClass()) {
  case binom::VarTypeClass::null: return 0;
  case binom::VarTypeClass::number: return 1;
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
  return reinterpret_cast<Number&>(self);
}
