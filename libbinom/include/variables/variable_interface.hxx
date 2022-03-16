#ifndef VARIABLE_INTERFACE_HXX
#define VARIABLE_INTERFACE_HXX

#include "../utils/resource_controll.hxx"

namespace binom::priv {

class VariableBase {
protected:
  typedef priv::Link Link;
  typedef priv::Resource Resource;
  Link node_link;

  Resource* getResource() const noexcept {return *node_link;}
  LinkType getLinkType() const noexcept {return node_link.getLinkType();}

};

}

#endif // VARIABLE_INTERFACE_HXX
