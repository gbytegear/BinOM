#include "libbinom/include/variables/variable.hxx"
#include "libbinom/include/variables/number.hxx"
#include "libbinom/include/variables/bit_array.hxx"
#include "libbinom/include/variables/buffer_array.hxx"
#include "libbinom/include/variables/array.hxx"
#include "libbinom/include/variables/singly_linked_list.hxx"
#include "libbinom/include/variables/map.hxx"

using namespace binom;
using namespace binom::priv;

Variable::Variable(ResourceData data) : resource_link(data) {}
Variable::Variable(Link&& link) : resource_link(std::move(link)) {}
Variable::Variable() noexcept : Variable(ResourceData{VarType::null, {.pointer = nullptr}}) {}
Variable::Variable(decltype(nullptr)) noexcept : Variable(ResourceData{VarType::null, {.pointer = nullptr}}) {}
Variable::Variable(bool value) noexcept : Variable(ResourceData{VarType::boolean, {.bool_val = value}}) {}
Variable::Variable(ui8 value) noexcept : Variable(ResourceData{VarType::ui8, {.ui8_val = value}}) {}
Variable::Variable(i8 value) noexcept : Variable(ResourceData{VarType::si8, {.i8_val = value}}) {}
Variable::Variable(ui16 value) noexcept : Variable(ResourceData{VarType::ui16, {.ui16_val = value}}) {}
Variable::Variable(i16 value) noexcept : Variable(ResourceData{VarType::si16, {.i16_val = value}}) {}
Variable::Variable(ui32 value) noexcept : Variable(ResourceData{VarType::ui32, {.ui32_val = value}}) {}
Variable::Variable(i32 value) noexcept : Variable(ResourceData{VarType::si32, {.i32_val = value}}) {}
Variable::Variable(f32 value) noexcept : Variable(ResourceData{VarType::f32, {.f32_val = value}}) {}
Variable::Variable(ui64 value) noexcept : Variable(ResourceData{VarType::ui64, {.ui64_val = value}}) {}
Variable::Variable(i64 value) noexcept : Variable(ResourceData{VarType::si64, {.i64_val = value}}) {}
Variable::Variable(f64 value) noexcept : Variable(ResourceData{VarType::f64, {.f64_val = value}}) {}
Variable::Variable(const GenericValue& value) noexcept : Variable(value.toResourceData()) {}
Variable::Variable(GenericValue&& value) noexcept : Variable(value.toResourceData()) {}

Variable::Variable(const literals::bitarr bit_array)
  : Variable(ResourceData{VarType::bit_array, {.bit_array_header = priv::BitArrayHeader::create(bit_array)}}) {}

Variable::Variable(const literals::ui8arr ui8_array)
  : Variable(ResourceData{VarType::ui8_array, {.buffer_array_header = priv::BufferArrayHeader::create(ui8_array)}}) {}
Variable::Variable(const literals::i8arr i8_array)
  : Variable(ResourceData{VarType::si8_array, {.buffer_array_header = priv::BufferArrayHeader::create(i8_array)}}) {}
Variable::Variable(const literals::ui16arr ui16_array)
  : Variable(ResourceData{VarType::ui16_array, {.buffer_array_header = priv::BufferArrayHeader::create(ui16_array)}}) {}
Variable::Variable(const literals::i16arr i16_array)
  : Variable(ResourceData{VarType::si16_array, {.buffer_array_header = priv::BufferArrayHeader::create(i16_array)}}) {}
Variable::Variable(const literals::ui32arr ui32_array)
  : Variable(ResourceData{VarType::ui32_array, {.buffer_array_header = priv::BufferArrayHeader::create(ui32_array)}}) {}
Variable::Variable(const literals::i32arr i32_array)
  : Variable(ResourceData{VarType::si32_array, {.buffer_array_header = priv::BufferArrayHeader::create(i32_array)}}) {}
Variable::Variable(const literals::f32arr f32_array)
  : Variable(ResourceData{VarType::f32_array, {.buffer_array_header = priv::BufferArrayHeader::create(f32_array)}}) {}
Variable::Variable(const literals::ui64arr ui64_array)
  : Variable(ResourceData{VarType::ui64_array, {.buffer_array_header = priv::BufferArrayHeader::create(ui64_array)}}) {}
Variable::Variable(const literals::i64arr i64_array)
  : Variable(ResourceData{VarType::si64_array, {.buffer_array_header = priv::BufferArrayHeader::create(i64_array)}}) {}
Variable::Variable(const literals::f64arr f64_array)
  : Variable(ResourceData{VarType::f64_array, {.buffer_array_header = priv::BufferArrayHeader::create(f64_array)}}) {}

Variable::Variable(const literals::arr array)
  : Variable(ResourceData{VarType::array, {.array_header = priv::ArrayHeader::create(array)}}) {}

Variable::Variable(const literals::sllist singly_linked_list)
  : Variable(ResourceData{VarType::singly_linked_list, {.single_linked_list_header = new priv::SinglyLinkedListHeader(singly_linked_list)}}) {}

Variable::Variable(const Variable&& other) noexcept : resource_link(std::move(other.resource_link)) {}
Variable::Variable(const Variable& other) noexcept : resource_link(Link::cloneResource(other.resource_link)) {}

Variable Variable::getReference() const noexcept {return Link(resource_link);}

OptionalSharedRecursiveLock Variable::getLock(priv::MtxLockType lock_type) const noexcept {
  return resource_link.getLock(lock_type);
}

bool Variable::isResourceExist() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return true;
  else
    return false;
}

VarType Variable::getType() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return resource_link.getType();
  else
    return VarType::null;
}

VarTypeClass Variable::getTypeClass() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return toTypeClass(resource_link.getType());
  else
    return VarTypeClass::null;
}

ValType Variable::getValType() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return toValueType(resource_link.getType());
  else
    return ValType::invalid_type;
}

VarBitWidth Variable::getBitWidth() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return toBitWidth(resource_link.getType());
  else
    return VarBitWidth::invalid_type;
}

VarNumberType Variable::getNumberType() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return toNumberType(resource_link.getType());
  else
    return VarNumberType::invalid_type;
}

size_t Variable::getElementCount() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  switch (getTypeClass()) {
  case VarTypeClass::null: return 0;
  case VarTypeClass::number: return 1;
  case VarTypeClass::bit_array: return const_cast<Variable&>(self).toBitArray().getElementCount();
  case VarTypeClass::buffer_array: return const_cast<Variable&>(self).toBufferArray().getElementCount();
  case VarTypeClass::array: return const_cast<Variable&>(self).toArray().getElementCount();
  case VarTypeClass::singly_linked_list: // TODO
  break;
  case VarTypeClass::doubly_linked_list: // TODO
  break;
  case VarTypeClass::map:
  break;
  case VarTypeClass::invalid_type:
  break;
  }
  return -1;
}

size_t Variable::getElementSize() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  switch (getTypeClass()) {
  case binom::VarTypeClass::null: return 0;
  case binom::VarTypeClass::number: return size_t(getBitWidth());
  case binom::VarTypeClass::bit_array: return 1;
  case binom::VarTypeClass::buffer_array: return size_t(getBitWidth());
  case binom::VarTypeClass::array:
  case binom::VarTypeClass::singly_linked_list:
  case binom::VarTypeClass::doubly_linked_list: return sizeof (Link);
  case binom::VarTypeClass::map: // TODO
  case binom::VarTypeClass::invalid_type: return 0;
  }
}

Variable::operator Number&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::number) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<Number&>(const_cast<Variable&>(self));
}

Variable::operator BitArray&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::bit_array) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<BitArray&>(const_cast<Variable&>(self));
}

Variable::operator BufferArray&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::buffer_array) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<BufferArray&>(const_cast<Variable&>(self));
}

Variable::operator Array&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::array) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<Array&>(const_cast<Variable&>(self));
}

Variable::operator SinglyLinkedList&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::singly_linked_list) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<SinglyLinkedList&>(const_cast<Variable&>(self));
}

Variable::operator DoublyLinkedList&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::doubly_linked_list) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<DoublyLinkedList&>(const_cast<Variable&>(self));
}

Variable::operator Map&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::map) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<Map&>(const_cast<Variable&>(self));
}

Number& Variable::toNumber() const {return self;}
BitArray& Variable::toBitArray() const {return self;}
BufferArray& Variable::toBufferArray() const {return self;}
Array& Variable::toArray() const {return self;}
SinglyLinkedList& Variable::toSinglyLinkedList() const {return self;}
DoublyLinkedList& Variable::toDoublyLinkedList() const {return self;}
Map& Variable::toMap() const {return self;}

Variable& Variable::operator=(const Variable& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

Variable& Variable::operator=(Variable&& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

Variable& Variable::changeLink(const Variable& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  this->~Variable();
  return *new(this) Variable(other);
}

Variable& Variable::changeLink(Variable&& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  this->~Variable();
  return *new(this) Variable(std::move(other));
}
