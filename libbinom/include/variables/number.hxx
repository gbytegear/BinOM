#ifndef NUMBER_HPP
#define NUMBER_HPP

#include "variable_interface.hxx"
#include "generic_value.hxx"

namespace binom {

class Number {
  using Link = priv::Link;
  Link node_link;

  priv::Resource* getResource() const noexcept {return *node_link;}
public:
  Number() noexcept : node_link() {}
  Number(bool value) noexcept : node_link(priv::Resource{VarType::boolean, value}) {}
  Number(ui8 value) noexcept : node_link(priv::Resource{VarType::ui8, value}) {}
  Number(i8 value) noexcept : node_link(priv::Resource{VarType::si8, value}) {}
  Number(ui16 value) noexcept : node_link(priv::Resource{VarType::ui16, value}) {}
  Number(i16 value) noexcept : node_link(priv::Resource{VarType::si16, value}) {}
  Number(ui32 value) noexcept : node_link(priv::Resource{VarType::ui32, value}) {}
  Number(i32 value) noexcept : node_link(priv::Resource{VarType::si32, value}) {}
  Number(f32 value) noexcept : node_link(priv::Resource{VarType::f32, value}) {}
  Number(ui64 value) noexcept : node_link(priv::Resource{VarType::ui64, value}) {}
  Number(i64 value) noexcept : node_link(priv::Resource{VarType::si64, value}) {}
  Number(f64 value) noexcept : node_link(priv::Resource{VarType::f64, value}) {}
  Number(const Number& other, LinkType type = LinkType::invalid_link) noexcept {
    switch (type) {
    case binom::LinkType::soft_link:
    case binom::LinkType::hard_link:
      node_link.changeLink(other.node_link, type);
    return;
    case binom::LinkType::invalid_link:
      new(&node_link) Link(priv::Resource{other.getType(), other.getResource()->data.ui64_val});
    return;
    }
  }
  Number(Number&& other) noexcept;
  Number(GenericValue value) noexcept : node_link(priv::Resource{getVarType(value.getType()), ui64(value)}) {}

  VarType getType() const noexcept {return node_link.getResourceType();}
  LinkType getLinkType() const noexcept {return node_link.getLinkType();}

};

}


#endif // NUMBER_HPP
