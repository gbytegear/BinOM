#include "libbinom/include/variables/buffer_array.hxx"

using namespace binom;
using namespace binom::priv;

BufferArrayHeader*& BufferArray::getData() const noexcept {return resource_link->data.buffer_array_header;}

BufferArray::BufferArray(priv::Link&& link) : Variable(std::move(link)) {}
BufferArray::BufferArray(const BufferArray& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
BufferArray::BufferArray(const BufferArray&& other) noexcept : Variable(dynamic_cast<const Variable&&>(other)) {}

BufferArray BufferArray::getReference() noexcept {return Link(resource_link);}
const BufferArray BufferArray::getReference() const noexcept {return Link(resource_link);}

size_t BufferArray::getElementCount() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->getElementCount(getBitWidth());
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
  if(!lk) return ValueRef(resource_link, 0);
  return ValueRef(resource_link, index);
}

BufferArray::Iterator BufferArray::begin() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator(resource_link, 0);
  return Iterator(resource_link, 0);
}

BufferArray::Iterator BufferArray::end() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator(resource_link, 0);
  return Iterator(resource_link, getData()->getSize() / size_t(getBitWidth()));
}

BufferArray::ReverseIterator BufferArray::rbegin() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ReverseIterator(resource_link, 0);
  return ReverseIterator(resource_link, (getData()->getSize() / size_t(getBitWidth())) - 1);
}

BufferArray::ReverseIterator BufferArray::rend() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ReverseIterator(resource_link, 0);
  return ReverseIterator(resource_link, -1);
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
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  this->~BufferArray();
  return *new(this) BufferArray(other);
}

BufferArray& BufferArray::changeLink(BufferArray&& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  this->~BufferArray();
  return *new(this) BufferArray(std::move(other));
}
