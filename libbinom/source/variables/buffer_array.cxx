#include "libbinom/include/variables/buffer_array.hxx"

using namespace binom;
using namespace binom::priv;

BufferArrayImplementation*& BufferArray::getData() const noexcept {return resource_link->data.buffer_array_implementation;}

BufferArray::BufferArray(Link&& link) : Variable(std::move(link)) {}

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

void BufferArray::remove(size_t at, size_t count) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    BufferArrayImplementation::remove(getData(), getBitWidth(), at, count);
}

void BufferArray::popBack(size_t count) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    BufferArrayImplementation::popBack(getData(), getBitWidth(), count);
}

void BufferArray::popFront(size_t count) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    BufferArrayImplementation::popFront(getData(), getBitWidth(), count);
}

void BufferArray::clear() {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    BufferArrayImplementation::clear(getData(), getBitWidth());
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

template<typename CharT>
requires extended_type_traits::is_char_v<CharT>
BufferArray::operator std::basic_string<CharT>() const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return std::basic_string<CharT>();

  std::basic_string<CharT> str(getData()->getSize(), '\0');

  auto str_it = str.begin(), str_end = str.end();
  auto buffer_array_it = cbegin(), buffer_array_end = cend();

  for(; str_it != str_end && buffer_array_it != buffer_array_end; (++str_it, ++buffer_array_it))
    *str_it = CharT(*buffer_array_it);

  return str;
}

template BufferArray::operator std::basic_string<char>() const;
template BufferArray::operator std::basic_string<signed char>() const;
template BufferArray::operator std::basic_string<unsigned char>() const;
template BufferArray::operator std::basic_string<wchar_t>() const;
template BufferArray::operator std::basic_string<char8_t>() const;
template BufferArray::operator std::basic_string<char16_t>() const;
template BufferArray::operator std::basic_string<char32_t>() const;
