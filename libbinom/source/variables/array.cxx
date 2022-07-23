#include "libbinom/include/variables/array.hxx"

using namespace binom;
using namespace binom::priv;

ArrayImplementation*& Array::getData() const noexcept {return resource_link->data.array_implementation;}

Array::Array(priv::Link&& link) : Variable(std::move(link)) {}

Array::Array()
  : Variable(ResourceData{VarType::array, {.array_implementation = priv::ArrayImplementation::create(literals::arr{})}}) {}

Array::Array(const literals::arr array)
  : Variable(ResourceData{VarType::array, {.array_implementation = priv::ArrayImplementation::create(array)}}) {}

Array::Array(const Array& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
Array::Array(const Array&& other) noexcept : Variable(dynamic_cast<const Variable&&>(other)) {}

Array Array::move() noexcept {return Link(resource_link);}

size_t Array::getElementCount() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->getElementCount();
}

size_t Array::getCapacity() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->getCapacity();
}

size_t Array::getSize() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->getSize();
}

Variable Array::pushBack(Variable variable) {
  if(auto lk = getLock(MtxLockType::unique_locked);lk) return priv::ArrayImplementation::pushBack(getData(), variable.move());
  else return nullptr;
}

Array::Iterator Array::pushBack(const literals::arr variable_list) {
  if(auto lk = getLock(MtxLockType::unique_locked);lk) return priv::ArrayImplementation::pushBack(getData(), variable_list);
  else return nullptr;
}

Variable Array::pushFront(Variable variable) {
  if(auto lk = getLock(MtxLockType::unique_locked);lk) return priv::ArrayImplementation::pushFront(getData(), variable.move());
  else return nullptr;
}

Array::Iterator Array::pushFront(const literals::arr variable_list) {
  if(auto lk = getLock(MtxLockType::unique_locked);lk) return priv::ArrayImplementation::pushFront(getData(), variable_list);
  else return nullptr;
}

Variable Array::insert(size_t at, Variable variable) {
  if(auto lk = getLock(MtxLockType::unique_locked);lk) return priv::ArrayImplementation::insert(getData(), at, variable.move());
  else return nullptr;
}

Array::Iterator Array::insert(size_t at, literals::arr variable_list) {
  if(auto lk = getLock(MtxLockType::unique_locked);lk) return priv::ArrayImplementation::insert(getData(), at, variable_list);
  else return nullptr;
}

void Array::popBack(size_t count) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;

  priv::ArrayImplementation::popBack(getData(), count);
}

void Array::popFront(size_t count) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;

  priv::ArrayImplementation::popFront(getData(), count);
}

void Array::remove(size_t at, size_t count) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;

  priv::ArrayImplementation::remove(getData(), at, count);
}

void Array::clear() {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;

  priv::ArrayImplementation::clear(getData());
}

Variable Array::operator[](size_t index) noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return nullptr;
  return (*getData())[index];
}

const Variable Array::operator[](size_t index) const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return nullptr;
  return (*getData())[index];
}

Array& Array::operator+=(Variable variable) {pushBack(std::move(variable)); return self;}
Array& Array::operator+=(literals::arr variable_list) {pushBack(std::move(variable_list)); return self;}
Array& Array::operator+=(const Array variable_array) {pushBack(std::move(variable_array)); return self;}

Array::Iterator Array::begin() {return getData()->begin();}

Array::Iterator Array::end() {return getData()->end();}

Array::ReverseIterator Array::rbegin() {return getData()->rbegin();}

Array::ReverseIterator Array::rend() {return getData()->rend();}

Array::ConstIterator Array::cbegin() const {return getData()->cbegin();}

Array::ConstIterator Array::cend() const {return getData()->cend();}

Array::ConstReverseIterator Array::crbegin() const {return getData()->crbegin();}

Array::ConstReverseIterator Array::crend() const {return getData()->crend();}

Array& Array::operator=(const Array& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

Array& Array::operator=(Array&& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

Array& Array::changeLink(const Array& other) {
  if(this == &other) return self;
  this->~Array();
  return *new(this) Array(other);
}

Array& Array::changeLink(Array&& other) {
  if(this == &other) return self;
  this->~Array();
  return *new(this) Array(std::move(other));
}
