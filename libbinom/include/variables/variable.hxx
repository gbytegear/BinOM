#ifndef VARIABLE_H
#define VARIABLE_H

#include "../utils/resource_control.hxx"
#include "generic_value.hxx"

namespace binom {

class Variable {
protected:

  priv::Link resource_link;

private:
  friend struct priv::SharedResource;
  static void destroyResorce(priv::ResourceData res_data);
  static priv::Link cloneResource(priv::Link resource_link) noexcept; // TODO...

  Variable(priv::ResourceData data) : resource_link(data) {}

protected:
  Variable(priv::Link&& link) : resource_link(std::move(link)) {}

public:
  Variable() noexcept : Variable(priv::ResourceData{VarType::null, {.pointer = nullptr}}) {}
  Variable(decltype(nullptr)) noexcept : Variable(priv::ResourceData{VarType::null, {.pointer = nullptr}}) {}
  Variable(bool value) noexcept : Variable(priv::ResourceData{VarType::boolean, {.bool_val = value}}) {}
  Variable(ui8 value) noexcept : Variable(priv::ResourceData{VarType::ui8, {.ui8_val = value}}) {}
  Variable(i8 value) noexcept : Variable(priv::ResourceData{VarType::si8, {.i8_val = value}}) {}
  Variable(ui16 value) noexcept : Variable(priv::ResourceData{VarType::ui16, {.ui16_val = value}}) {}
  Variable(i16 value) noexcept : Variable(priv::ResourceData{VarType::si16, {.i16_val = value}}) {}
  Variable(ui32 value) noexcept : Variable(priv::ResourceData{VarType::ui32, {.ui32_val = value}}) {}
  Variable(i32 value) noexcept : Variable(priv::ResourceData{VarType::si32, {.i32_val = value}}) {}
  Variable(f32 value) noexcept : Variable(priv::ResourceData{VarType::f32, {.f32_val = value}}) {}
  Variable(ui64 value) noexcept : Variable(priv::ResourceData{VarType::ui64, {.ui64_val = value}}) {}
  Variable(i64 value) noexcept : Variable(priv::ResourceData{VarType::si64, {.i64_val = value}}) {}
  Variable(f64 value) noexcept : Variable(priv::ResourceData{VarType::f64, {.f64_val = value}}) {}
  Variable(const GenericValue& value) noexcept : Variable(value.toResourceData()) {}
  Variable(GenericValue&& value) noexcept : Variable(value.toResourceData()) {}

  Variable(Variable&& other) noexcept : resource_link(std::move(other.resource_link)) {}
  Variable(const Variable& other) noexcept : resource_link(cloneResource(other.resource_link)) {}
  static Variable createRefVariable(const Variable& other) noexcept {return priv::Link(other.resource_link);}
  static Number createRefVariable(const Number& other) noexcept;

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept {
    return resource_link.getLock(lock_type);
  }

  bool isResourceExist() const noexcept {
    if(auto lk = getLock(MtxLockType::shared_locked); lk)
      return true;
    else
      return false;
  }

  VarType getType() const noexcept {
    if(auto lk = getLock(MtxLockType::shared_locked); lk)
      return resource_link.getType();
    else
      return VarType::null;
  }

  VarTypeClass getTypeClass() const noexcept {
    if(auto lk = getLock(MtxLockType::shared_locked); lk)
      return toTypeClass(resource_link.getType());
    else
      return VarTypeClass::null;
  }

  ValType getValType() const noexcept {
    if(auto lk = getLock(MtxLockType::shared_locked); lk)
      return toValueType(resource_link.getType());
    else
      return ValType::invalid_type;
  }

  VarBitWidth getBitWidth() const noexcept {
    if(auto lk = getLock(MtxLockType::shared_locked); lk)
      return toBitWidth(resource_link.getType());
    else
      return VarBitWidth::invalid_type;
  }

  VarNumberType getNumberType() const noexcept {
    if(auto lk = getLock(MtxLockType::shared_locked); lk)
      return toNumberType(resource_link.getType());
    else
      return VarNumberType::invalid_type;
  }

  size_t getElementCount() const noexcept;

  ui8 getMemebrSize() const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return 0;
    switch (getTypeClass()) {
    case binom::VarTypeClass::null: return 0;
    case binom::VarTypeClass::number:
    case binom::VarTypeClass::buffer_array: return ui8(getBitWidth());
    case binom::VarTypeClass::array:
    case binom::VarTypeClass::list: return sizeof (priv::Link);
    case binom::VarTypeClass::map: // TODO
    case binom::VarTypeClass::invalid_type: return 0;
    }
  }

  operator Number& ();

};


}

#endif // VARIABLE_H
