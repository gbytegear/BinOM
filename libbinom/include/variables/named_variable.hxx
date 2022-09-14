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
  friend class NamedVariableBase;
  friend class priv::MapImplementation::NamedVariable;
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

class binom::priv::MapImplementation::NamedVariable
    : public priv::NamedVariableBase<binom::priv::MapImplementation::NamedVariable> {
  template<typename Driven>
  friend class priv::NamedVariableBase;
  typedef std::pair<const KeyValue, Variable> VariablePair;
  VariablePair& pair;
  inline Variable& getVariableRef() noexcept {return pair.second;}
//  inline KeyValue& getKeyRef() noexcept {return pair.first;}
  inline const Variable& getVariableRef() const noexcept {return pair.second;}
  inline const KeyValue& getKeyRef() const noexcept {return pair.first;}

  friend class MapImplementation;
  friend class MapImplementation::Iterator;
  friend class MapImplementation::ReverseIterator;
  NamedVariable(const VariablePair& pair) : pair(const_cast<VariablePair&>(pair)) {}
public:
  NamedVariable(const NamedVariable& other) : pair(const_cast<VariablePair&>(other.pair)) {}
  NamedVariable(NamedVariable& other) : pair(other.pair) {}
};

class priv::MultiMapImplementation::NamedVariable
    : public priv::NamedVariableBase<priv::MultiMapImplementation::NamedVariable> {
  friend class NamedVariableBase;
  friend class priv::MultiMapImplementation;
  friend class MultiMap;
  KeyValue* key;
  Variable variable;

  inline Variable& getVariableRef() noexcept {return variable;}
  inline KeyValue& getKeyRef() noexcept {return *key;}
  inline const Variable& getVariableRef() const noexcept {return variable;}
  inline const KeyValue& getKeyRef() const noexcept {return *key;}

  NamedVariable(const KeyValue& key, const Variable variable)
    : key(const_cast<KeyValue*>(&key)), variable(variable.move()) {}
public:
  NamedVariable()
    : key(nullptr), variable(nullptr) {}
  NamedVariable(const NamedVariable& other)
    : key(other.key),
      variable(other.variable.move()) {}

  NamedVariable(NamedVariable&& other)
    : key(other.key),
      variable(other.variable.move()) {}
};

}

#endif // NAMED_VARIABLE_HXX
