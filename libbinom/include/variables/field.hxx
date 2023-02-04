#ifndef NAMED_VARIABLE_HXX
#define NAMED_VARIABLE_HXX

#include "variable.hxx"
#include "../binom_impl/ram_storage_implementation.hxx"

namespace binom {
namespace priv {

template<class Driven>
class FieldBase {

  Driven& downcast() noexcept {return *reinterpret_cast<Driven*>(this);}
  const Driven& downcast() const noexcept {return *reinterpret_cast<const Driven*>(this);}

public:
  Variable operator*() noexcept {return downcast().getVariableRef().move();}
  Variable* operator->() noexcept {return &downcast().getVariableRef();}

  const Variable operator*() const noexcept {return downcast().getVariableRef().move();}
  const Variable* operator->() const noexcept {return &downcast().getVariableRef();}

  Variable getValue() noexcept {return downcast().getVariableRef().move();}
  Variable getKey() noexcept {return downcast().getKeyRef();}

  const Variable getValue() const noexcept {return downcast().getVariableRef().move();}
  const Variable getKey() const noexcept {return downcast().getKeyRef();}

  Variable setValue(Variable value) noexcept {return downcast().setValueImpl(value.move());}
  KeyValue setKey(KeyValue key) noexcept {return downcast().setKeyImpl(std::move(key));}
  Variable operator=(Variable value) noexcept {return downcast().setValueImpl(value.move());}
};

}

class FieldInit : public priv::FieldBase<FieldInit> {
  template<typename Driven>
  friend class priv::FieldBase;
  friend class priv::MultiMapImplementation;

  KeyValue key;
  Variable variable;

  inline Variable& getVariableRef() noexcept {return variable;}
  inline KeyValue& getKeyRef() noexcept {return key;}
  inline const Variable& getVariableRef() const noexcept {return variable;}
  inline const KeyValue& getKeyRef() const noexcept {return key;}
  inline Variable setValueImpl(Variable value) noexcept {return variable = value.move();}
  inline KeyValue setKeyImpl(KeyValue key) noexcept {return self.key = std::move(key);}

public:
  FieldInit(KeyValue key, Variable variable)
    : key(std::move(key)), variable(variable.move()) {}
  FieldInit(const FieldInit&& named_variable)
    : key(std::move(const_cast<FieldInit&&>(named_variable).key)), variable(const_cast<FieldInit&&>(named_variable).variable.move()) {}
  template<class FieldInitDriven>
  requires extended_type_traits::is_crtp_base_of_v<FieldBase, FieldInitDriven>
  FieldInit(const FieldInitDriven& other)
    : key(other.getKeyRef()), variable(other.getVariableRef()) {}
  template<class FieldInitDriven>
  requires extended_type_traits::is_crtp_base_of_v<FieldBase, FieldInitDriven>
  FieldInit(FieldInitDriven&& other)
    : key(std::move(other.getKeyRef())), variable(std::move(other.getVariableRef())) {}
};

class FieldRef : public priv::FieldBase<FieldRef> {
  template<typename Driven> friend class priv::FieldBase;
  friend class priv::MapImplementation;
  friend class priv::MultiMapImplementation;
  friend class priv::TableImplementation;

  friend class index::Index;
  friend class index::Iterator;
  friend class index::ConstIterator;
  friend class index::ReverseIterator;
  friend class index::ConstReverseIterator;

  index::Field* data;

  inline Variable getVariableRef() noexcept {return data ? data->getValue().move() : nullptr;}
  inline const Variable getVariableRef() const noexcept {return data ? data->getValue().move() : nullptr;}
  inline const KeyValue getKeyRef() const noexcept {return data ? data->getKey() : KeyValue();}
  inline Variable setValueImpl(Variable value) noexcept {return data ? data->setValue(std::move(value)) : nullptr;}
  inline KeyValue setKeyImpl(KeyValue key) noexcept {return data ? data->setKey(std::move(key)) : KeyValue();}

  FieldRef(const index::Field& data) : data(const_cast<index::Field*>(&data)) {}
public:
  FieldRef(decltype (nullptr)) : data(nullptr) {}
  FieldRef(const FieldRef& other) : data(const_cast<index::Field*>(other.data)) {}
  FieldRef(FieldRef& other) : data(other.data) {}

  Variable getOwner() { return data->getOwner(); }
  const Variable getOwner() const { return data->getOwner(); }

  bool isIndexed() const { return data->isIndexed(); }

  using priv::FieldBase<FieldRef>::operator=;
};

}

#endif // NAMED_VARIABLE_HXX
