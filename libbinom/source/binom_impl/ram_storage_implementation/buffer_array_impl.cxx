#include "libbinom/include/binom_impl/ram_storage_implementation/buffer_array_impl.hxx"
#include "libbinom/include/utils/util_functions.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;


BufferArrayImplementation::BufferArrayImplementation(const BufferArrayImplementation& other)
  : size(other.size), capacity(other.capacity) {
  memcpy(getData(), other.getData(), size);
}

size_t BufferArrayImplementation::calculateCapacity(size_t size) noexcept {
  return util_functions::getNearestPow2(sizeof(BufferArrayImplementation) + size);
}

BufferArrayImplementation* BufferArrayImplementation::copy(const BufferArrayImplementation* other) {
  return new(new byte[ sizeof(BufferArrayImplementation) + other->capacity ]) BufferArrayImplementation(*other);
}

void BufferArrayImplementation::popBack(BufferArrayImplementation*& implementation, VarBitWidth type, size_t count) {
  return remove(implementation, type, implementation->size - count, count);
}

void BufferArrayImplementation::popFront(BufferArrayImplementation*& implementation, VarBitWidth type, size_t count) {
  return remove(implementation, type, 0, count);
}

void* BufferArrayImplementation::getData() const { return const_cast<void*>(reinterpret_cast<const void*>(this + 1)); }

size_t BufferArrayImplementation::getSize() const noexcept {return size;}

size_t BufferArrayImplementation::getElementCount(VarBitWidth type) const noexcept {return size_t(std::ceil(llf_t(size)/ size_t(type)));}

size_t BufferArrayImplementation::getCapacity() const noexcept {return capacity;}

void* BufferArrayImplementation::increaseSize(BufferArrayImplementation*& implementation, VarBitWidth type, size_t count) {
  const size_t new_size = implementation->size + count * size_t(type);
  const size_t old_size = implementation->size;
  const size_t new_capacity = calculateCapacity(new_size);
  if(new_capacity == implementation->capacity) {
    BufferArrayImplementation* new_implementation = new(new byte[ new_capacity ]) BufferArrayImplementation(*implementation);
    new_implementation->size = new_size;
    new_implementation->capacity = new_capacity;
    delete implementation;
    implementation = new_implementation;
  } else {
    implementation->size = new_size;
  }
  return implementation->getDataAs<byte>() + old_size;
}

void BufferArrayImplementation::reduceSize(BufferArrayImplementation*& implementation, VarBitWidth type, size_t count) {
  const size_t size = count * size_t(type);
  if(size <= implementation->size) {
    implementation->size -= size;
    return;
  } else {
    implementation->size = 0;
    return;
  }
}

void BufferArrayImplementation::shrinkToFit(BufferArrayImplementation*& implementation) {
  if(implementation->size == implementation->capacity) return;
  implementation->capacity = sizeof(BufferArrayImplementation) + implementation->size;
  BufferArrayImplementation* old_implementation = implementation;
  implementation = new(new byte[ implementation->capacity ]) BufferArrayImplementation(*old_implementation);
  delete old_implementation;
}

void* BufferArrayImplementation::insertBlock(BufferArrayImplementation*& implementation, VarBitWidth type, size_t at, size_t count) {
  size_t old_size = implementation->size;
  size_t from = at * size_t(type);
  if(from >= old_size) return increaseSize(implementation, type, count);
  increaseSize(implementation, type, count);
  memmove(implementation->getDataAs<byte>() + from + count * size_t(type),
          implementation->getDataAs<byte>() + from,
          old_size - from);
  return implementation->getDataAs<byte>() + from;
}

void BufferArrayImplementation::remove(BufferArrayImplementation*& implementation, VarBitWidth type, size_t at, size_t count) {
  size_t rm_size = count * size_t(type);
  size_t from = at * size_t(type);
  if(from >= implementation->size) return;
  if(from + rm_size >= implementation->size)
    return reduceSize(implementation, VarBitWidth::byte, implementation->size - from);
  size_t old_size = implementation->size;
  memmove(implementation->getDataAs<byte>() + from,
          implementation->getDataAs<byte>() + from + rm_size,
          old_size - from - rm_size);
  return reduceSize(implementation, type, count);
}

void BufferArrayImplementation::clear(BufferArrayImplementation*& implementation, VarBitWidth type) {
  return remove(implementation, type, 0, implementation->size);
}

void* BufferArrayImplementation::get(VarBitWidth type, size_t at) const {
  switch (type) {
  case VarBitWidth::byte: return getDataAs<byte>() + at;
  case VarBitWidth::word: return getDataAs<word>() + at;
  case VarBitWidth::dword: return getDataAs<dword>() + at;
  case VarBitWidth::qword: return getDataAs<qword>() + at;
  case VarBitWidth::invalid_type:
  default: return nullptr;
  }
}

GenericValueRef BufferArrayImplementation::get(ValType type, size_t at) {return GenericValueRef(type, get(toBitWidth(type), at));}

const GenericValueRef BufferArrayImplementation::get(ValType type, size_t at) const {return GenericValueRef(type, get(toBitWidth(type), at));}

GenericValueIterator BufferArrayImplementation::begin(ValType type) {return GenericValueIterator(type, getData());}

GenericValueIterator BufferArrayImplementation::end(ValType type) {return GenericValueIterator(type, getDataAs<byte>() + getElementCount(toBitWidth(type)) * size_t(toBitWidth(type)));}

const GenericValueIterator BufferArrayImplementation::begin(ValType type) const {return GenericValueIterator(type, getData());}

const GenericValueIterator BufferArrayImplementation::end(ValType type) const {return GenericValueIterator(type, getDataAs<byte>() + getElementCount(toBitWidth(type)) * size_t(toBitWidth(type)));}

ReverseGenericValueIterator BufferArrayImplementation::rbegin(ValType type) {return ReverseGenericValueIterator(type, getDataAs<byte>() + (i64(getElementCount(toBitWidth(type))) - 1) * size_t(toBitWidth(type)));}

ReverseGenericValueIterator BufferArrayImplementation::rend(ValType type) {return ReverseGenericValueIterator(type, getDataAs<byte>() - size_t(toBitWidth(type)));}

const ReverseGenericValueIterator BufferArrayImplementation::rbegin(ValType type) const {return ReverseGenericValueIterator(type, getDataAs<byte>() + (i64(getElementCount(toBitWidth(type))) - 1) * size_t(toBitWidth(type)));}

const ReverseGenericValueIterator BufferArrayImplementation::rend(ValType type) const {return ReverseGenericValueIterator(type, getDataAs<byte>() - size_t(toBitWidth(type)));}

void BufferArrayImplementation::operator delete(void* ptr) {return ::delete [] reinterpret_cast<byte*>(ptr);}
