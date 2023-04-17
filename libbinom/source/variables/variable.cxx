#include "libbinom/include/variables/variable.hxx"
#include "libbinom/include/variables/number.hxx"
#include "libbinom/include/variables/bit_array.hxx"
#include "libbinom/include/variables/buffer_array.hxx"
#include "libbinom/include/variables/array.hxx"
#include "libbinom/include/variables/list.hxx"
#include "libbinom/include/variables/map.hxx"
#include "libbinom/include/variables/multi_map.hxx"
#include "libbinom/include/binom_impl/ram_storage_implementation.hxx"

#include <set>

using namespace binom;
using namespace binom::priv;


Link createMap(const literals::map& map) {
  SharedResource* shared_resource = new SharedResource();
  shared_resource->resource_data.data.map_implementation = new MapImplementation(*shared_resource, map);
  shared_resource->resource_data.type = VarType::map;
  Link link(*shared_resource);
  shared_resource->link_counter = 1;
  return link;
}

Link createMultiMap(const literals::multimap& multimap) {
  SharedResource* shared_resource = new SharedResource();
  shared_resource->resource_data.data.multi_map_implementation = new MultiMapImplementation(*shared_resource, multimap);
  shared_resource->resource_data.type = VarType::multimap;
  Link link(*shared_resource);
  shared_resource->link_counter = 1;
  return link;
}

Variable::Variable(ResourceData data) : resource_link(data) {}
Variable::Variable(Link&& link) : resource_link(std::move(link)) {}

Variable::Variable() noexcept : Variable(nullptr) {}
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
  : Variable(ResourceData{VarType::bit_array, {.bit_array_implementation = priv::BitArrayImplementation::create(bit_array)}}) {}

template<typename CharT>
requires extended_type_traits::is_char_v<CharT>
Variable::Variable(const std::basic_string_view<CharT> string_view)
  : Variable(ResourceData{to_buffer_array_type<CharT>,
{.buffer_array_implementation = priv::BufferArrayImplementation::create(string_view)}}) {}

template Variable::Variable(const std::basic_string_view<char>);
template Variable::Variable(const std::basic_string_view<signed char>);
template Variable::Variable(const std::basic_string_view<unsigned char>);
template Variable::Variable(const std::basic_string_view<wchar_t>);
template Variable::Variable(const std::basic_string_view<char8_t>);
template Variable::Variable(const std::basic_string_view<char16_t>);
template Variable::Variable(const std::basic_string_view<char32_t>);

Variable::Variable(const literals::ui8arr ui8_array)
  : Variable(ResourceData{VarType::ui8_array, {.buffer_array_implementation = priv::BufferArrayImplementation::create(ui8_array)}}) {}
Variable::Variable(const literals::i8arr i8_array)
  : Variable(ResourceData{VarType::si8_array, {.buffer_array_implementation = priv::BufferArrayImplementation::create(i8_array)}}) {}
Variable::Variable(const literals::ui16arr ui16_array)
  : Variable(ResourceData{VarType::ui16_array, {.buffer_array_implementation = priv::BufferArrayImplementation::create(ui16_array)}}) {}
Variable::Variable(const literals::i16arr i16_array)
  : Variable(ResourceData{VarType::si16_array, {.buffer_array_implementation = priv::BufferArrayImplementation::create(i16_array)}}) {}
Variable::Variable(const literals::ui32arr ui32_array)
  : Variable(ResourceData{VarType::ui32_array, {.buffer_array_implementation = priv::BufferArrayImplementation::create(ui32_array)}}) {}
Variable::Variable(const literals::i32arr i32_array)
  : Variable(ResourceData{VarType::si32_array, {.buffer_array_implementation = priv::BufferArrayImplementation::create(i32_array)}}) {}
Variable::Variable(const literals::f32arr f32_array)
  : Variable(ResourceData{VarType::f32_array, {.buffer_array_implementation = priv::BufferArrayImplementation::create(f32_array)}}) {}
Variable::Variable(const literals::ui64arr ui64_array)
  : Variable(ResourceData{VarType::ui64_array, {.buffer_array_implementation = priv::BufferArrayImplementation::create(ui64_array)}}) {}
Variable::Variable(const literals::i64arr i64_array)
  : Variable(ResourceData{VarType::si64_array, {.buffer_array_implementation = priv::BufferArrayImplementation::create(i64_array)}}) {}
Variable::Variable(const literals::f64arr f64_array)
  : Variable(ResourceData{VarType::f64_array, {.buffer_array_implementation = priv::BufferArrayImplementation::create(f64_array)}}) {}

Variable::Variable(const literals::arr array)
  : Variable(ResourceData{VarType::array, {.array_implementation = priv::ArrayImplementation::create(array)}}) {}

Variable::Variable(const literals::list list)
  : Variable(ResourceData{VarType::list, {.list_implementation = new priv::ListImplementation(list)}}) {}

Variable::Variable(const literals::map map) : Variable(createMap(map)) {}

Variable::Variable(const literals::multimap multimap, NewNodePosition pos) : Variable(createMultiMap(multimap)) {}

binom::Variable::Variable(const literals::table table)
    : Variable(ResourceData{VarType::table, {.table_implementation = new priv::TableImplementation(table)}}) {}

//Variable::Variable(const Number &other) : Variable(other.upcast()) {}
//Variable::Variable(const BitArray &other) : Variable(other.upcast()) {}
//Variable::Variable(const BufferArray &other) : Variable(other.upcast()) {}
//Variable::Variable(const Array &other) : Variable(other.upcast()) {}
//Variable::Variable(const List &other) : Variable(other.upcast()) {}
//Variable::Variable(const Map &other) : Variable(other.upcast()) {}
//Variable::Variable(const MultiMap &other) : Variable(other.upcast()) {}

//Variable::Variable(Number&& other) : Variable(std::move(other.upcast())) {}
//Variable::Variable(BitArray&& other) : Variable(std::move(other.upcast())) {}
//Variable::Variable(BufferArray&& other) : Variable(std::move(other.upcast())) {}
//Variable::Variable(Array&& other) : Variable(std::move(other.upcast())) {}
//Variable::Variable(List&& other) : Variable(std::move(other.upcast())) {}
//Variable::Variable(Map&& other) : Variable(std::move(other.upcast())) {}
//Variable::Variable(MultiMap&& other) : Variable(std::move(other.upcast())) {}

Variable::Variable(Variable&& other) noexcept : resource_link(std::move(other.resource_link)) {}
Variable::Variable(const Variable& other) noexcept : resource_link(Link::cloneResource(other.resource_link)) {}

Variable Variable::move() noexcept {return Link(resource_link);}
const Variable Variable::move() const noexcept {return Link(resource_link);}

OptionalSharedRecursiveLock Variable::getLock(MtxLockType lock_type) const noexcept {
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
  case VarTypeClass::bit_array: return toBitArray().getElementCount();
  case VarTypeClass::buffer_array: return toBufferArray().getElementCount();
  case VarTypeClass::array: return toArray().getElementCount();
  case VarTypeClass::list: return toList().getElementCount();
  case VarTypeClass::map: return toMap().getElementCount();
  case VarTypeClass::multimap: return toMultiMap().getElementCount();
  case VarTypeClass::table:
    // TODO
  break;
  case VarTypeClass::invalid_type:
  default: break;
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
  case binom::VarTypeClass::list: return sizeof (Link);
  case binom::VarTypeClass::map: return sizeof (KeyValue) + sizeof (Link);
  default:
  case binom::VarTypeClass::invalid_type: return 0;
  }
}

ui64 Variable::getLinkCount() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return resource_link.getLinkCount();
  else return 0;
}

Variable::operator Number&() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::number) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<Number&>(self);
}

Variable::operator BitArray&() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::bit_array) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<BitArray&>(self);
}

Variable::operator BufferArray&() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::buffer_array) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<BufferArray&>(self);
}

Variable::operator Array&() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::array) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<Array&>(self);
}

Variable::operator List&() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::list) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<List&>(self);
}

Variable::operator Map&() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::map) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<Map&>(self);
}

Variable::operator MultiMap&() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::multimap) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<MultiMap&>(self);
}

Variable::operator const Number&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::number) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<const Number&>(self);
}

Variable::operator const BitArray&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::bit_array) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<const BitArray&>(self);
}

Variable::operator const BufferArray&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::buffer_array) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<const BufferArray&>(self);
}

Variable::operator const Array&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::array) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<const Array&>(self);
}

Variable::operator const List&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::list) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<const List&>(self);
}

Variable::operator const Map&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::map) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<const Map&>(self);
}

Variable::operator const MultiMap&() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) throw Error(ErrorType::binom_resource_not_available);
  if(getTypeClass() != VarTypeClass::multimap) throw Error(ErrorType::binom_invalid_type);
  return reinterpret_cast<const MultiMap&>(self);
}

Number& Variable::toNumber() {return self;}
BitArray& Variable::toBitArray() {return self;}
BufferArray& Variable::toBufferArray() {return self;}
Array& Variable::toArray() {return self;}
List& Variable::toList() {return self;}
Map& Variable::toMap() {return self;}
MultiMap& Variable::toMultiMap() {return self;}

const Number& Variable::toNumber() const {return self;}
const BitArray& Variable::toBitArray() const {return self;}
const BufferArray& Variable::toBufferArray() const {return self;}
const Array& Variable::toArray() const {return self;}
const List& Variable::toList() const {return self;}
const Map& Variable::toMap() const {return self;}
const MultiMap& Variable::toMultiMap() const {return self;}

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
  this->~Variable();
  return *new(this) Variable(other);
}

Variable& Variable::changeLink(Variable&& other) {
  if(this == &other) return self;
  this->~Variable();
  return *new(this) Variable(std::move(other));
}

shared_recursive_mtx::TransactionLock binom::Variable::makeTransaction(std::initializer_list<const Variable> variables, shared_recursive_mtx::MtxLockType lock_type) {
  std::set<std::shared_mutex*> mtx_set;
  for(const auto& variable : variables) mtx_set.emplace(&variable.getMutex());
  return shared_recursive_mtx::TransactionLock(std::move(mtx_set), lock_type);
}
