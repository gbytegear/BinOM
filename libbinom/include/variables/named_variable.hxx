#ifndef NAMED_VARIABLE_HXX
#define NAMED_VARIABLE_HXX

#include "variable.hxx"
#include "../binom_impl/ram_storage_implementation.hxx"

namespace binom {
namespace priv {

template<class Driven>
class NamedVariableBase {

  Driven& downcast() noexcept {return *reinterpret_cast<Driven*>(this);}
  const Driven& downcast() const noexcept {return *reinterpret_cast<const Driven*>(this);}

public:
  Variable operator*() noexcept {return downcast().getVariableRef().move();}
  Variable* operator->() noexcept {return &downcast().getVariableRef();}

  const Variable operator*() const noexcept {return downcast().getVariableRef().move();}
  const Variable* operator->() const noexcept {return &downcast().getVariableRef();}

  Variable getVariable() noexcept {return downcast().getVariableRef().move();}
  Variable getKey() noexcept {return downcast().getKeyRef();}

  const Variable getVariable() const noexcept {return downcast().getVariableRef().move();}
  const Variable getKey() const noexcept {return downcast().getKeyRef();}
};

}

class NamedVariable : public priv::NamedVariableBase<NamedVariable> {
  template<typename Driven>
  friend class priv::NamedVariableBase;
  friend class priv::MultiMapImplementation;
  KeyValue key;
  Variable variable;

  inline Variable& getVariableRef() noexcept {return variable;}
  inline KeyValue& getKeyRef() noexcept {return key;}
  inline const Variable& getVariableRef() const noexcept {return variable;}
  inline const KeyValue& getKeyRef() const noexcept {return key;}

public:
  NamedVariable(KeyValue key, Variable variable)
    : key(std::move(key)), variable(variable.move()) {}
  NamedVariable(const NamedVariable&& named_variable)
    : key(std::move(const_cast<NamedVariable&&>(named_variable).key)), variable(const_cast<NamedVariable&&>(named_variable).variable.move()) {}
  template<class NamedVariableDriven>
  requires extended_type_traits::is_crtp_base_of_v<NamedVariableBase, NamedVariableDriven>
  NamedVariable(const NamedVariableDriven& other)
    : key(other.getKeyRef()), variable(other.getVariableRef()) {}
  template<class NamedVariableDriven>
  requires extended_type_traits::is_crtp_base_of_v<NamedVariableBase, NamedVariableDriven>
  NamedVariable(NamedVariableDriven&& other)
    : key(std::move(other.getKeyRef())), variable(std::move(other.getVariableRef())) {}
};

class MapNodeRef : public priv::NamedVariableBase<MapNodeRef> {
  template<typename Driven>
  friend class priv::NamedVariableBase;
  friend class priv::MapImplementation;
  friend class priv::MultiMapImplementation;
  typedef std::pair<const KeyValue, Variable> KeyVariablePair;

  KeyVariablePair& pair;

  inline Variable& getVariableRef() noexcept {return pair.second;}
  inline const Variable& getVariableRef() const noexcept {return pair.second;}
  inline const KeyValue& getKeyRef() const noexcept {return pair.first;}

  friend class priv::MapImplementation;
  friend class priv::MapImplementation::Iterator;
  friend class priv::MapImplementation::ReverseIterator;
  MapNodeRef(const KeyVariablePair& pair) : pair(const_cast<KeyVariablePair&>(pair)) {}
public:
  MapNodeRef(const MapNodeRef& other) : pair(const_cast<KeyVariablePair&>(other.pair)) {}
  MapNodeRef(MapNodeRef& other) : pair(other.pair) {}
};

}

#endif // NAMED_VARIABLE_HXX
