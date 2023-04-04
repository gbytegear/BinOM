#include "libbinom/include/variables/field.hxx"
#include "libbinom/include/binom_impl/ram_storage_implementation/index_impl.hxx"

using namespace binom;
using namespace binom::priv;

Variable &FieldInit::getVariableRef() noexcept {return variable;}

KeyValue &FieldInit::getKeyRef() noexcept {return key;}

const Variable &FieldInit::getVariableRef() const noexcept {return variable;}

const KeyValue &FieldInit::getKeyRef() const noexcept {return key;}

Variable FieldInit::setValueImpl(Variable value) noexcept {return variable = value.move();}

KeyValue FieldInit::setKeyImpl(KeyValue key) noexcept {return self.key = std::move(key);}

FieldInit::FieldInit(KeyValue key, Variable variable)
    : key(std::move(key)), variable(variable.move()) {}

FieldInit::FieldInit(const FieldInit &&named_variable)
    : key(std::move(const_cast<FieldInit&&>(named_variable).key)), variable(const_cast<FieldInit&&>(named_variable).variable.move()) {}



Variable FieldRef::getVariableRef() noexcept {return data ? data->getValue().move() : nullptr;}

const Variable FieldRef::getVariableRef() const noexcept {return data ? data->getValue().move() : nullptr;}

const KeyValue FieldRef::getKeyRef() const noexcept {return data ? data->getKey() : KeyValue();}

Variable FieldRef::setValueImpl(Variable value) noexcept {return data ? data->setValue(std::move(value)) : nullptr;}

KeyValue FieldRef::setKeyImpl(KeyValue key) noexcept {return data ? data->setKey(std::move(key)) : KeyValue();}

FieldRef::FieldRef(const index::Field &data) : data(const_cast<index::Field*>(&data)) {}

FieldRef::FieldRef(decltype (nullptr)) : data(nullptr) {}

FieldRef::FieldRef(const FieldRef &other) : data(const_cast<index::Field*>(other.data)) {}

FieldRef::FieldRef(FieldRef &other) : data(other.data) {}

Variable FieldRef::getOwner() { return data->getOwner(); }

const Variable FieldRef::getOwner() const { return data->getOwner(); }

bool FieldRef::isIndexed() const { return data->isIndexed(); }
