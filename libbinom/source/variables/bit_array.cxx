#include "libbinom/include/variables/bit_array.hxx"

using namespace binom;
using namespace binom::priv;

BitArrayImplementation*& BitArray::getData() const noexcept {return resource_link->data.bit_array_implementation;}
BitArray::BitArray(priv::Link&& link) : Variable(std::move(link)) {}
BitArray::BitArray() : Variable(literals::bitarr{}) {}
BitArray::BitArray(const literals::bitarr bit_array) : Variable(bit_array) {}
BitArray::BitArray(const BitArray& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
BitArray::BitArray(const BitArray&& other) noexcept : Variable(dynamic_cast<const Variable&&>(other)) {}

BitArray BitArray::move() noexcept {return Link(resource_link);}
const BitArray BitArray::move() const noexcept {return Link(resource_link);}

size_t BitArray::getElementCount() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->getBitSize();
}

size_t BitArray::getSize() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->getByteSize();
}

size_t BitArray::getCapacity() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->getCapacity();
}

BitArray::ValueRef BitArray::operator[](size_t index) const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return priv::Bits::getNullValue();
  return (*getData())[index];
}

BitArray::ValueRef BitArray::operator+=(bool value) {return pushBack(value);}
BitArray::Iterator BitArray::operator+=(const literals::bitarr value_list) {return pushBack(value_list);}

BitArray::ValueRef BitArray::pushBack(bool value) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return priv::Bits::getNullValue();
  return priv::BitArrayImplementation::pushBack(getData(), value);
}

BitArray::Iterator BitArray::pushBack(const literals::bitarr value_list) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return priv::Bits::getNullIterator();
  return priv::BitArrayImplementation::pushBack(getData(), value_list);
}

BitArray::ValueRef BitArray::pushFront(bool value) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return priv::Bits::getNullIterator();
  return priv::BitArrayImplementation::pushFront(getData(), value);
}

BitArray::Iterator BitArray::pushFront(const literals::bitarr value_list) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return priv::Bits::getNullIterator();
  return priv::BitArrayImplementation::pushFront(getData(), value_list);
}

BitArray::ValueRef BitArray::insert(size_t at, bool value) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return priv::Bits::getNullIterator();
  return priv::BitArrayImplementation::insert(getData(), at, value);
}

BitArray::Iterator BitArray::insert(size_t at, const literals::bitarr value_list) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return priv::Bits::getNullIterator();
  return priv::BitArrayImplementation::insert(getData(), at, value_list);
}

void BitArray::popBack(size_t size) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;
  priv::BitArrayImplementation::popBack(getData(), size);
}

void BitArray::popFront(size_t size) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;
  priv::BitArrayImplementation::removeBits(getData(), 0, size);
}

void BitArray::remove(size_t at, size_t size) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;
  priv::BitArrayImplementation::removeBits(getData(), at, size);
}

BitArray::Iterator BitArray::begin() const {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return priv::Bits::getNullIterator();
  return getData()->begin();
}

BitArray::Iterator BitArray::end() const {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return priv::Bits::getNullIterator();
  return getData()->end();
}

BitArray::ReverseIterator BitArray::rbegin() const {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return priv::Bits::getNullReverseIterator();
  return getData()->rbegin();
}

BitArray::ReverseIterator BitArray::rend() const {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return priv::Bits::getNullReverseIterator();
  return getData()->rend();
}

BitArray& BitArray::operator=(const BitArray& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

BitArray& BitArray::operator=(BitArray&& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

BitArray& BitArray::changeLink(const BitArray& other) {
  if(this == &other) return self;
  this->~BitArray();
  return *new(this) BitArray(other);
}

BitArray& BitArray::changeLink(BitArray&& other) {
  if(this == &other) return self;
  this->~BitArray();
  return *new(this) BitArray(std::move(other));
}
