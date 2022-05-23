#ifndef VARIABLE_H
#define VARIABLE_H

#include "../utils/resource_control.hxx"
#include "generic_value.hxx"

namespace binom {

class Variable {
protected:
  using Link = priv::Link;
  using ResourceData = priv::ResourceData;

  Link resource_link;

protected:

  Variable(ResourceData data) : resource_link(data) {}
  Variable(Link&& link) : resource_link(std::move(link)) {}

public:
  // Null
  Variable() noexcept : Variable(ResourceData{VarType::null, {.pointer = nullptr}}) {}
  Variable(decltype(nullptr)) noexcept : Variable(ResourceData{VarType::null, {.pointer = nullptr}}) {}
  // Number
  Variable(bool value) noexcept : Variable(ResourceData{VarType::boolean, {.bool_val = value}}) {}
  Variable(ui8 value) noexcept : Variable(ResourceData{VarType::ui8, {.ui8_val = value}}) {}
  Variable(i8 value) noexcept : Variable(ResourceData{VarType::si8, {.i8_val = value}}) {}
  Variable(ui16 value) noexcept : Variable(ResourceData{VarType::ui16, {.ui16_val = value}}) {}
  Variable(i16 value) noexcept : Variable(ResourceData{VarType::si16, {.i16_val = value}}) {}
  Variable(ui32 value) noexcept : Variable(ResourceData{VarType::ui32, {.ui32_val = value}}) {}
  Variable(i32 value) noexcept : Variable(ResourceData{VarType::si32, {.i32_val = value}}) {}
  Variable(f32 value) noexcept : Variable(ResourceData{VarType::f32, {.f32_val = value}}) {}
  Variable(ui64 value) noexcept : Variable(ResourceData{VarType::ui64, {.ui64_val = value}}) {}
  Variable(i64 value) noexcept : Variable(ResourceData{VarType::si64, {.i64_val = value}}) {}
  Variable(f64 value) noexcept : Variable(ResourceData{VarType::f64, {.f64_val = value}}) {}
  Variable(const GenericValue& value) noexcept : Variable(value.toResourceData()) {}
  Variable(GenericValue&& value) noexcept : Variable(value.toResourceData()) {}

  // BitArray
  Variable(const literals::bitarr bit_array)
    : Variable(ResourceData{VarType::bit_array, {.bit_array_header = priv::BitArrayHeader::create(bit_array)}}) {}

  // BufferArray
  Variable(const literals::ui8arr ui8_array)
    : Variable(ResourceData{VarType::ui8_array, {.buffer_array_header = priv::BufferArrayHeader::create(ui8_array)}}) {}
  Variable(const literals::i8arr i8_array)
    : Variable(ResourceData{VarType::si8_array, {.buffer_array_header = priv::BufferArrayHeader::create(i8_array)}}) {}
  Variable(const literals::ui16arr ui16_array)
    : Variable(ResourceData{VarType::ui16_array, {.buffer_array_header = priv::BufferArrayHeader::create(ui16_array)}}) {}
  Variable(const literals::i16arr i16_array)
    : Variable(ResourceData{VarType::si16_array, {.buffer_array_header = priv::BufferArrayHeader::create(i16_array)}}) {}
  Variable(const literals::ui32arr ui32_array)
    : Variable(ResourceData{VarType::ui32_array, {.buffer_array_header = priv::BufferArrayHeader::create(ui32_array)}}) {}
  Variable(const literals::i32arr i32_array)
    : Variable(ResourceData{VarType::si32_array, {.buffer_array_header = priv::BufferArrayHeader::create(i32_array)}}) {}
  Variable(const literals::f32arr f32_array)
    : Variable(ResourceData{VarType::f32_array, {.buffer_array_header = priv::BufferArrayHeader::create(f32_array)}}) {}
  Variable(const literals::ui64arr ui64_array)
    : Variable(ResourceData{VarType::ui64_array, {.buffer_array_header = priv::BufferArrayHeader::create(ui64_array)}}) {}
  Variable(const literals::i64arr i64_array)
    : Variable(ResourceData{VarType::si64_array, {.buffer_array_header = priv::BufferArrayHeader::create(i64_array)}}) {}
  Variable(const literals::f64arr f64_array)
    : Variable(ResourceData{VarType::f64_array, {.buffer_array_header = priv::BufferArrayHeader::create(f64_array)}}) {}

  // Array
  Variable(const literals::arr array)
    : Variable(ResourceData{VarType::array, {.array_header = priv::ArrayHeader::create(array)}}) {}


  // Move & Copy
  Variable(Variable&& other) noexcept : resource_link(std::move(other.resource_link)) {}
  Variable(const Variable& other) noexcept : resource_link(Link::cloneResource(other.resource_link)) {}

  Variable getReference() noexcept {return Link(resource_link);}

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
    case binom::VarTypeClass::number: return ui8(getBitWidth());
    case binom::VarTypeClass::bit_array: return 1;
    case binom::VarTypeClass::buffer_array: return ui8(getBitWidth());
    case binom::VarTypeClass::array:
    case binom::VarTypeClass::list: return sizeof (Link);
    case binom::VarTypeClass::map: // TODO
    case binom::VarTypeClass::invalid_type: return 0;
    }
  }

  operator Number& ();
  operator BitArray& ();
  operator BufferArray& ();
  operator Array& ();
  operator List& ();
  operator Map& ();

  inline Number& toNumber() {return self;}
  inline BitArray& toBitArray() {return self;}
  inline BufferArray& toBufferArray() {return self;}
  inline Array& toArray() {return self;}
  inline List& toList() {return self;}
  inline Map& toMap() {return self;}

};

}

#endif // VARIABLE_H
