#include "libbinom/include/variables/array.hxx"

using namespace binom;
using namespace binom::priv;

ArrayHeader*& Array::getData() const noexcept {return resource_link->data.array_header;}

Array::Array(priv::Link&& link) : Variable(std::move(link)) {}

Array::Array()
  : Variable(ResourceData{VarType::array, {.array_header = priv::ArrayHeader::create(literals::arr{})}}) {}

Array::Array(const literals::arr array)
  : Variable(ResourceData{VarType::array, {.array_header = priv::ArrayHeader::create(array)}}) {}

Array Array::getReference() noexcept {return Link(resource_link);}

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
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return nullptr;

  auto allocated_memory = priv::ArrayHeader::increaseSize(getData(), 1);
  new(allocated_memory) Variable(std::move(variable));

  return allocated_memory->getReference();
}

Array::Iterator Array::pushBack(literals::arr variable_list) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return nullptr;

  auto allocated_memory = priv::ArrayHeader::increaseSize(getData(), variable_list.getSize());
  auto it = allocated_memory;
  for(const auto& variable : variable_list)
    new(it++) Variable(std::move(variable));

  return allocated_memory;
}

Variable Array::pushFront(Variable variable) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return nullptr;

  auto allocated_memory = priv::ArrayHeader::insert(getData(), 0, 1);
  new(allocated_memory) Variable(std::move(variable));

  return allocated_memory->getReference();
}

Array::Iterator Array::pushFront(literals::arr variable_list) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return nullptr;

  auto allocated_memory = priv::ArrayHeader::insert(getData(), 0, variable_list.getSize());
  auto it = allocated_memory;
  for(const auto& variable : variable_list)
    new(it++) Variable(std::move(variable));

  return allocated_memory;
}

Variable Array::insert(size_t at, Variable variable) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return nullptr;

  auto allocated_memory = priv::ArrayHeader::insert(getData(), at, 1);
  new(allocated_memory) Variable(std::move(variable));

  return allocated_memory->getReference();
}

Array::Iterator Array::insert(size_t at, literals::arr variable_list) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return nullptr;

  auto allocated_memory = priv::ArrayHeader::insert(getData(), at, variable_list.getSize());
  auto it = allocated_memory;
  for(const auto& variable : variable_list)
    new(it++) Variable(std::move(variable));

  return allocated_memory;
}

void Array::popBack(size_t count) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;

  priv::ArrayHeader::popBack(getData(), count);
}

void Array::popFront(size_t count) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;

  priv::ArrayHeader::remove(getData(), 0, count);
}

void Array::remove(size_t at, size_t count) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;

  priv::ArrayHeader::remove(getData(), at, count);
}

void Array::clear() {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;

  priv::ArrayHeader::popBack(getData(), getElementCount());
}

Variable Array::operator[](size_t index) noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return nullptr;
  return (*getData())[index];
}

Variable Array::operator+=(Variable variable) {return pushBack(std::move(variable));}

Variable Array::operator+=(literals::arr variable_list) {return pushBack(std::move(variable_list));}

Array::Iterator Array::begin() const {return getData()->begin();}

Array::Iterator Array::end() const {return getData()->end();}

Array::ReverseIterator Array::rbegin() const {return getData()->rbegin();}

Array::ReverseIterator Array::rend() const {return getData()->rend();}
