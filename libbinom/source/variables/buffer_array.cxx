#include "libbinom/include/variables/buffer_array.hxx"

using namespace binom;
using namespace binom::priv;

BufferArrayImplementation*& BufferArray::getData() const noexcept {return resource_link->data.buffer_array_implementation;}

BufferArray::BufferArray(priv::Link&& link) : Variable(std::move(link)) {}

BufferArray::BufferArray() noexcept : Variable(literals::ui8arr{}) {}
BufferArray::BufferArray(const BufferArray& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
BufferArray::BufferArray(const BufferArray&& other) noexcept : Variable(dynamic_cast<const Variable&&>(other)) {}

BufferArray BufferArray::move() noexcept {return Link(resource_link);}
const BufferArray BufferArray::move() const noexcept {return Link(resource_link);}

size_t BufferArray::getElementCount() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->getElementCount(getBitWidth());
  else return 0;
}

size_t BufferArray::getSize() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->getSize();
}

size_t BufferArray::getCapacity() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->getCapacity();
}

BufferArray::ValueRef BufferArray::operator[](size_t index) noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ValueRef(ValType::invalid_type, nullptr);
  return getData()->get(getValType(), index);
}

const BufferArray::ValueRef BufferArray::operator[](size_t index) const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ValueRef(ValType::invalid_type, nullptr);
  return getData()->get(getValType(), index);
}

BufferArray::Iterator BufferArray::begin() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator(ValType::invalid_type, nullptr);
  return getData()->begin(getValType());
}

BufferArray::Iterator BufferArray::end() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator(ValType::invalid_type, nullptr);
  return getData()->end(getValType());
}

BufferArray::ReverseIterator BufferArray::rbegin() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ReverseIterator(ValType::invalid_type, nullptr);
  return getData()->rbegin(getValType());
}

BufferArray::ReverseIterator BufferArray::rend() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ReverseIterator(ValType::invalid_type, nullptr);
  return getData()->rend(getValType());
}

const BufferArray::Iterator BufferArray::cbegin() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator(ValType::invalid_type, nullptr);
  return getData()->begin(getValType());
}

const BufferArray::Iterator BufferArray::cend() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator(ValType::invalid_type, nullptr);
  return getData()->end(getValType());
}

const BufferArray::ReverseIterator BufferArray::crbegin() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ReverseIterator(ValType::invalid_type, nullptr);
  return getData()->rbegin(getValType());
}

const BufferArray::ReverseIterator BufferArray::crend() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ReverseIterator(ValType::invalid_type, nullptr);
  return getData()->rend(getValType());
}

BufferArray& BufferArray::operator=(const BufferArray& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

BufferArray& BufferArray::operator=(BufferArray&& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

BufferArray& BufferArray::changeLink(const BufferArray& other) {
  if(this == &other) return self;
  this->~BufferArray();
  return *new(this) BufferArray(other);
}

BufferArray& BufferArray::changeLink(BufferArray&& other) {
  if(this == &other) return self;
  this->~BufferArray();
  return *new(this) BufferArray(std::move(other));
}
