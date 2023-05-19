#ifndef PROPERTY_HXX
#define PROPERTY_HXX

#include <utility>

namespace prop {

template <typename ValueTypeT>
struct DefaultAccessors {
  ValueTypeT set(ValueTypeT& stored_value, const ValueTypeT& new_value) {
    return stored_value = new_value;
  }

  ValueTypeT set(ValueTypeT& stored_value, ValueTypeT&& new_value) {
    return stored_value = std::move(new_value);
  }

  ValueTypeT get(ValueTypeT& stored_value) {
    return stored_value;
  }
};


template <typename Driven>
class PropertyBase {
protected:
  Driven& downcast() noexcept {return *reinterpret_cast<Driven*>(this);}
  const Driven& downcast() const noexcept {return *reinterpret_cast<const Driven*>(this);}
public:
  typedef Driven::ValueType ValueType;
  typedef Driven::Accessors Accessors;

  operator ValueType () const {
    return Accessors().get(downcast().getValueRef());
  }

  ValueType operator = (const ValueType& new_value) {
    return Accessors().set(downcast().getValueRef(), new_value);
  }

  ValueType operator = (ValueType&& new_value) {
    return Accessors().set(downcast().getValueRef(), std::move(new_value));
  }

  ValueType operator += (const ValueType& new_value) {
    return Accessors().set(downcast().getValueRef(), ValueType(downcast().getValueRef()) + new_value);
  }

  ValueType operator += (ValueType&& new_value) {
    return Accessors().set(downcast().getValueRef(), ValueType(downcast().getValueRef()) + new_value);
  }

  ValueType operator -= (const ValueType& new_value) {
    return Accessors().set(downcast().getValueRef(), ValueType(downcast().getValueRef()) - new_value);
  }

  ValueType operator -= (ValueType&& new_value) {
    return Accessors().set(downcast().getValueRef(), ValueType(downcast().getValueRef()) - new_value);
  }

  ValueType operator *= (const ValueType& new_value) {
    return Accessors().set(downcast().getValueRef(), ValueType(downcast().getValueRef()) * new_value);
  }

  ValueType operator *= (ValueType&& new_value) {
    return Accessors().set(downcast().getValueRef(), ValueType(downcast().getValueRef()) * new_value);
  }

  ValueType operator /= (const ValueType& new_value) {
    return Accessors().set(downcast().getValueRef(), ValueType(downcast().getValueRef()) / new_value);
  }

  ValueType operator /= (ValueType&& new_value) {
    return Accessors().set(downcast().getValueRef(), ValueType(downcast().getValueRef()) / new_value);
  }

};


template <typename ValueType, typename Accessors = DefaultAccessors>
class ReferenceProperty : public PropertyBase<ReferenceProperty<ValueType, Accessors>> {
protected:
  ValueType& reference;

  inline ValueType& getValueRef() { return reference; }
  inline const ValueType& getValueRef() const { return reference; }
public:
  typedef PropertyBase<ReferenceProperty<ValueType, Accessors>> Base;
  ReferenceProperty(ValueType& reference) : reference(reference) {}
  ReferenceProperty(ReferenceProperty& other) : reference(other.reference) {}
  ReferenceProperty(ReferenceProperty&& other) : reference(other.reference) {}
  template <typename T>
  ReferenceProperty(ReferenceProperty<ValueType, T>& other) : reference(other.reference) {}
  template <typename T>
  ReferenceProperty(ReferenceProperty<ValueType, T>&& other) : reference(other.reference) {}
  using Base::operator =;
  using Base::operator ValueType;
};



template <typename ValueType, typename Accessors = DefaultAccessors>
class Property : public PropertyBase<Property<ValueType, Accessors>>  {
protected:
  ValueType value;

  inline ValueType& getValueRef() { return value; }
  inline const ValueType& getValueRef() const { return value; }
public:
  typedef PropertyBase<ReferenceProperty<ValueType, Accessors>> Base;
  Property(const ValueType& value) : value(value) {}
  Property(ValueType&& value) : value(std::move(value)) {}
  template <typename T>
  Property(ReferenceProperty<ValueType, T>& other) : value(other.reference) {}
  template <typename T>
  Property(ReferenceProperty<ValueType, T>&& other) : value(other.reference) {}
  template <typename T>
  Property(Property<ValueType, T>& other) : value(other.value) {}
  template <typename T>
  Property(Property<ValueType, T>&& other) : value(std::move(other.value)) {}
  using Base::operator =;
  using Base::operator ValueType;
};

}

#endif // PROPERTY_HXX
