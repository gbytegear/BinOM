#ifndef NAMED_VARIABLE_HXX
#define NAMED_VARIABLE_HXX

#include "variable.hxx"
#include "../binom_impl/types.hxx"

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

  Variable& getVariableRef() noexcept;
  KeyValue& getKeyRef() noexcept;
  const Variable& getVariableRef() const noexcept;
  const KeyValue& getKeyRef() const noexcept;
  Variable setValueImpl(Variable value) noexcept;
  KeyValue setKeyImpl(KeyValue key) noexcept;

public:
  FieldInit(KeyValue key, Variable variable);
  FieldInit(const FieldInit&& named_variable);
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

  Variable getVariableRef() noexcept;
  const Variable getVariableRef() const noexcept;
  const KeyValue getKeyRef() const noexcept;
  Variable setValueImpl(Variable value) noexcept;
  KeyValue setKeyImpl(KeyValue key) noexcept;

  FieldRef(const index::Field& data);
public:
  FieldRef(decltype (nullptr));
  FieldRef(const FieldRef& other);
  FieldRef(FieldRef& other);

  Variable getOwner();
  const Variable getOwner() const;

  bool isIndexed() const;

  using priv::FieldBase<FieldRef>::operator=;
};

}

#endif // NAMED_VARIABLE_HXX
