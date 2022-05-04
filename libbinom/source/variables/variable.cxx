#include "libbinom/include/variables/variable.hxx"

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
  case binom::VarTypeClass::invalid_type:
  break;
  }
}
