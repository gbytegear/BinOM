#include "libbinom/include/binom_impl/ram_storage_implementation.hxx"

#include "libbinom/include/variables/variable.hxx"
#include "libbinom/include/utils/util_functions.hxx"
#include "libbinom/include/binom_impl/avl_tree.hxx"
#include <cmath>

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;
using namespace extended_type_traits;


//////////////////////////////////////////////////////////// BitArrayImplementation ////////////////////////////////////////////////////////



BitArrayImplementation::BitArrayImplementation(const literals::bitarr& bit_array_data)
  : bit_size(bit_array_data.size()), capacity(calculateCapacity(bit_array_data.size())) {
  auto data_it = bit_array_data.begin();
  for(auto it = begin(), _end = end(); it != _end; ++it, ++data_it)
    (*it) = *data_it;
}

BitArrayImplementation::BitArrayImplementation(const BitArrayImplementation& other)
  : bit_size(other.bit_size), capacity(other.capacity) {
  memcpy(getData(), other.getData(), getByteSize());
}

BitArrayImplementation* BitArrayImplementation::create(const literals::bitarr& bit_array_data) {
  return new(new byte[ calculateCapacity(bit_array_data.size()) ]) BitArrayImplementation(bit_array_data);
}

BitArrayImplementation* BitArrayImplementation::copy(const BitArrayImplementation* other) {
  return new(new byte[ other->capacity ]) BitArrayImplementation(*other);
}

size_t BitArrayImplementation::calculateByteSize(size_t bit_count) noexcept {
  return size_t(std::ceil(llf_t(bit_count)/ 8));
}

BitIterator BitArrayImplementation::increaseSize(BitArrayImplementation*& implementation, size_t bit_count) {
  const size_t new_bit_size = implementation->bit_size + bit_count;
  const size_t new_capacity = calculateCapacity(new_bit_size);
  if(implementation->capacity != new_capacity) {
    const size_t old_bit_size = implementation->bit_size;
    BitArrayImplementation* old_pointer = implementation;
    implementation = new(new byte[ sizeof(BitArrayImplementation) + new_capacity ]) BitArrayImplementation(*old_pointer);
    implementation->bit_size = new_bit_size;
    implementation->capacity = new_capacity;
    delete old_pointer;
    return implementation->getData()[old_bit_size / 8].getItearatorAt(old_bit_size % 8);
  } else {
    BitIterator it = implementation->end();
    implementation->bit_size = new_bit_size;
    return it;
  }
}

void BitArrayImplementation::reduceSize(BitArrayImplementation*& implementation, size_t bit_count) {
  if(bit_count <= implementation->bit_size) {
    implementation->bit_size -= bit_count;
    return;
  } else {
    implementation->bit_size = 0;
    return;
  }
}

BitIterator BitArrayImplementation::insertBits(BitArrayImplementation*& implementation, size_t at, size_t count) {
  using namespace util_functions;
  const size_t shift_start_byte = at / 8;
  const size_t shift_end_byte = (at + count) / 8;
  const ui8 shift_start_bit_in_byte = at % 8;
  const ui8 shift_end_bit_in_byte = (at + count) % 8;

  priv::BitArrayImplementation::increaseSize(implementation, count);
  ui8* data = implementation->getDataAs<ui8>();

  if(!shift_start_bit_in_byte && !shift_end_bit_in_byte) {
    doLeftShift(data, implementation->getByteSize() - shift_start_byte, count);
    return implementation->getData()[shift_start_byte].begin();
  }

  if(shift_start_byte != shift_end_byte || shift_start_byte != implementation->getByteSize() - 1) {
    doLeftShift(data + shift_start_byte + 1, implementation->getByteSize() - shift_start_byte - 1, count);
    const ui8 shift_loss_size = 8 - shift_start_bit_in_byte;

    if(shift_start_bit_in_byte >= shift_end_bit_in_byte) {
      data[shift_end_byte] =
          set0Before(data[shift_end_byte], shift_end_bit_in_byte + shift_loss_size) |
          (data[shift_start_byte] >> (8 - shift_end_bit_in_byte - shift_loss_size));
    } else {
      const size_t next_byte_shift_loss = shift_loss_size - (8 - shift_end_bit_in_byte);
      data[shift_end_byte + 1] =
          set0Before(data[shift_end_byte], next_byte_shift_loss) |
          data[shift_start_byte] >> (8 - next_byte_shift_loss);
      data[shift_end_byte] =
          set0From(data[shift_start_byte], shift_start_bit_in_byte) |
          (data[shift_start_byte] << (shift_end_bit_in_byte - shift_start_bit_in_byte));
    }

  } else {
    // If bit inserting only in last byte, without allocations new bytes after last byte
    data[shift_start_byte] =
        set0From(data[shift_start_byte], shift_start_bit_in_byte) |
        set0Before(data[shift_end_byte] << count, shift_end_bit_in_byte);
  }
  return implementation->getData()[shift_start_byte].getItearatorAt(shift_start_bit_in_byte);
}

void BitArrayImplementation::removeBits(BitArrayImplementation*& implementation, size_t at, size_t count) {
  using namespace util_functions;
  if(at >= implementation->bit_size) return;
  if(at + count >= implementation->bit_size)
    return priv::BitArrayImplementation::reduceSize(implementation, count - (count - (implementation->bit_size - at)));

  {
    const size_t shift_start_byte = at / 8;
    const size_t shift_start_bit_in_byte = at % 8;
    const size_t shift_end_byte = (at + count) / 8;
    const size_t shift_end_bit_in_byte = (at + count) % 8;
    ui8* data = implementation->getDataAs<ui8>();

    if(!shift_start_bit_in_byte)
      doRightShift(data + shift_start_byte, implementation->getByteSize() - shift_start_byte, count);
    elif(8 - shift_start_bit_in_byte >= count) {
      /*if(8 - shift_start_bit_in_byte > count)
       * [abcdefgh][ijklmnop] rm 3 at 1 => [a###bcde][fghijklm]
       * [11111111] >> (8 - 1) = [10000000], [a###bcde] & [10000000] = [a0000000]
       * [a###bcde] >> 3 = [#bcde000], [11111111] << 1 = [01111111], [#bcde000] & [01111111] = [0bcde000]
       * [fghijklm] << (8 - 3) = [00000fgh]
       * [a0000000] | [0bcde000] = [abcde000], [abcde000] | [00000fgh] = [abcdefgh] - result
       if(8 - shift_start_bit_in_byte == count)
       * [abcdefgh][ijklmnop] rm 4 at 4 => [abcd####][efghijkl]
       * [11111111] >> (8 - 4) = [11110000], [abcd####] & [11110000] = [abcd0000]
       * [abcd####] >> 4 = [####0000], [11111111] << 4 = [00001111], [####0000] & [00001111] = [00000000]
       * [efghijkl] << (8 - 4) = [0000efgh]
       * [abcd0000] | [00000000] = [abcd0000], [abcd0000] | [0000efgh] = [abcdefgh] - result
       */

      data[shift_start_byte] =
          (data[shift_start_byte] & (0xFF >> (8 - shift_start_bit_in_byte))) |
          ((data[shift_start_byte] >> count) & (0xFF << shift_start_bit_in_byte)) |
          data[shift_start_byte + 1] << (8 - count);
      doRightShift(data + shift_start_byte + 1, implementation->getByteSize() - shift_start_byte - 1, count);
    } elif((8 - shift_start_bit_in_byte) > (8 - shift_end_bit_in_byte)) {
      /* Example:
       * [abcdefgh]...[ijklmnop] rm 10 at 4 => [abcd####]...[######ef][ghijklmn] (end_byte = n, end_bit_in_byte = 6)
       * [11111111] >> (8 - 4) = [11110000], [abcd####] & [11110000] = [abcd0000]
       * [11111111] << 6 = [00000011], [######ef] & [00000011] = [000000ef], [000000ef] >> (6 - 4) = [0000ef00]
       * [ghijklmn] << (8 - (6 - 4)) = [000000gh]
       * [abcd0000] | [0000ef00] = [abcdef00], [abcdef00] | [000000gh] = [abcdefgh]
      */

      data[shift_start_byte] =
          (data[shift_start_byte] & (0xFF >> (8 - shift_start_bit_in_byte))) |
          ((data[shift_end_byte] & (0xFF << shift_end_bit_in_byte)) >> (shift_end_bit_in_byte - shift_start_bit_in_byte)) |
          data[shift_end_byte + 1] << (8 - (shift_end_bit_in_byte - shift_start_bit_in_byte));
      doRightShift(data + shift_start_byte + 1, implementation->getByteSize() - shift_start_byte - 1, count);
    } else {
      /* Example:
       * [abcdefgh]...[ijklmnop] rm 6 at 4 => [abcd####]...[##efghjk] (end_byte = n, end_bit_in_byte = 2)
       * [11111111] >> (8 - 4) = [11110000], [abcd####] & [11110000] = [abcd0000]
       * [11111111] << 2 = [00111111], [##efghjk] & [00111111] = [00efghjk], [00efghjk] << (4 - 2) = [0000efgh]
       * [abcd0000] | [0000efgh] = [abcdefgh]
      */

      data[shift_start_byte] =
          set0From(data[shift_start_byte], shift_start_bit_in_byte) |
          (set0Before(data[shift_end_byte], shift_end_bit_in_byte) << (shift_start_bit_in_byte - shift_end_bit_in_byte));
      doRightShift(data + shift_start_byte + 1, implementation->getByteSize() - shift_start_byte - 1, count);
    }

    return priv::BitArrayImplementation::reduceSize(implementation, count);
  }
}

void BitArrayImplementation::shrinkToFit(BitArrayImplementation*& implementation) {
  if(implementation->getByteSize() == implementation->capacity) return;
  implementation->capacity = sizeof(BitArrayImplementation) + implementation->getByteSize();
  BitArrayImplementation* old_pointer = implementation;
  implementation = new(new byte[ sizeof(BitArrayImplementation) + implementation->capacity ]) BitArrayImplementation(*old_pointer);
  delete old_pointer;
}

size_t BitArrayImplementation::calculateCapacity(size_t bit_count) noexcept {
  return util_functions::getNearestPow2(sizeof(BitArrayImplementation) + calculateByteSize(bit_count));
}

BitValueRef BitArrayImplementation::pushBack(BitArrayImplementation*& implementation, bool value) {
  auto it = increaseSize(implementation, 1);
  return (*it) = value;
}

BitIterator BitArrayImplementation::pushBack(BitArrayImplementation*& implementation, const literals::bitarr& value_list) {
  auto it = priv::BitArrayImplementation::increaseSize(implementation, value_list.size());
  { auto data_it = it;
    for(auto value_it = value_list.begin(), value_end = value_list.end(); value_it != value_end; ++value_it, ++data_it)
      (*data_it) = *value_it;
  }
  return it;
}

BitValueRef BitArrayImplementation::pushFront(BitArrayImplementation*& implementation, bool value) {
  auto it = insertBits(implementation, 0, 1);
  return (*it) = value;
}

BitIterator BitArrayImplementation::pushFront(BitArrayImplementation*& implementation, const literals::bitarr& value_list) {
  auto it = priv::BitArrayImplementation::insertBits(implementation, 0, value_list.size());
  { auto data_it = it;
    for(auto value_it = value_list.begin(), value_end = value_list.end(); value_it != value_end; ++value_it, ++data_it)
      (*data_it) = *value_it;
  }
  return it;
}

BitValueRef BitArrayImplementation::insert(BitArrayImplementation*& implementation, size_t at, bool value) {
  auto it = BitArrayImplementation::insertBits(implementation, at, 1);
  return (*it) = value;
}

BitIterator BitArrayImplementation::insert(BitArrayImplementation*& implementation, size_t at, const literals::bitarr value_list) {
  auto it = BitArrayImplementation::insertBits(implementation, at, value_list.size());
  { auto data_it = it;
    for(auto value_it = value_list.begin(), value_end = value_list.end(); value_it != value_end; ++value_it, ++data_it)
      (*data_it) = *value_it;
  }
  return it;
}

void BitArrayImplementation::popBack(BitArrayImplementation*& implementation, size_t size) {
  priv::BitArrayImplementation::reduceSize(implementation, size >= implementation->getBitSize() ? implementation->getBitSize() : size);
}

void BitArrayImplementation::popFront(BitArrayImplementation*& implementation, size_t size) {
  BitArrayImplementation::removeBits(implementation, 0, size);
}

size_t BitArrayImplementation::getCapacity() const noexcept {return capacity;}

size_t BitArrayImplementation::getBitSize() const noexcept {return bit_size;}

size_t BitArrayImplementation::getByteSize() const noexcept {return calculateByteSize(bit_size);}

Bits* BitArrayImplementation::getData() const noexcept {
  return reinterpret_cast<Bits*>(const_cast<BitArrayImplementation*>(this)) + sizeof(BitArrayImplementation);
}

void BitArrayImplementation::operator delete(void* ptr) {return ::delete [] reinterpret_cast<byte*>(ptr);}

BitValueRef BitArrayImplementation::operator[](size_t index) const noexcept {return getData()[index / 8][index % 8];}

BitIterator BitArrayImplementation::begin() const noexcept {return getData()->begin();}

BitIterator BitArrayImplementation::end() const noexcept {return getData()[bit_size / 8].getItearatorAt(bit_size % 8);}

BitReverseIterator BitArrayImplementation::rbegin() const noexcept {if(!bit_size) return rend(); return getData()[(bit_size - 1) / 8].getReverseIteratorAt((bit_size - 1) % 8);}

BitReverseIterator BitArrayImplementation::rend() const noexcept {return getData()->rend();}



//////////////////////////////////////////////////////////// BufferArrayImplementation ////////////////////////////////////////////////////////


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



//////////////////////////////////////////////////////////// ArrayImplementation ////////////////////////////////////////////////////////



ArrayImplementation::ArrayImplementation(const arr& value_list)
  : count(value_list.getSize()), capacity(calculateCapacity(count)) {
  auto it = begin();
  for(const auto& value : value_list)
    new(it++) Variable(value);
}

ArrayImplementation::ArrayImplementation(const ArrayImplementation& other)
  : count(other.count), capacity(other.capacity) {
  auto it = begin();
  for(const auto& value : other)
    new(it++) Variable(value);
}

size_t ArrayImplementation::calculateCapacity(size_t count) noexcept {
  return util_functions::getNearestPow2(sizeof(ArrayImplementation) + count * sizeof(Link));
}

ArrayImplementation* ArrayImplementation::create(const literals::arr& value_list) {
  return new(new byte[calculateCapacity(value_list.getSize())]) ArrayImplementation(value_list);
}

ArrayImplementation* ArrayImplementation::copy(const ArrayImplementation* other) {
  return new(new byte[other->capacity]) ArrayImplementation(*other);
}

size_t ArrayImplementation::getElementCount() const noexcept {return count;}
size_t ArrayImplementation::getCapacity() const noexcept {return capacity;}

size_t ArrayImplementation::getSize() const noexcept {return count * sizeof (Link);}

Variable* ArrayImplementation::getData() const { return reinterpret_cast<Variable*>(const_cast<ArrayImplementation*>(this + 1)); }

Variable ArrayImplementation::pushBack(ArrayImplementation*& implementation, Variable variable) {
  auto allocated_memory = increaseSize(implementation, 1);
  new(allocated_memory) Variable(std::move(variable));
  return allocated_memory->move();
}

ArrayImplementation::Iterator ArrayImplementation::pushBack(ArrayImplementation*& implementation, const literals::arr variable_list) {
  auto allocated_memory = increaseSize(implementation, variable_list.getSize());
  auto it = allocated_memory;
  for(const auto& variable : variable_list)
    new(it++) Variable(std::move(variable));
  return allocated_memory;
}

Variable ArrayImplementation::pushFront(ArrayImplementation*& implementation, Variable variable) {
  auto allocated_memory = insert(implementation, 0, 1);
  new(allocated_memory) Variable(std::move(variable));
  return allocated_memory->move();
}

ArrayImplementation::Iterator ArrayImplementation::pushFront(ArrayImplementation*& implementation, const literals::arr& variable_list) {
  auto allocated_memory = insert(implementation, 0, variable_list.getSize());
  auto it = allocated_memory;
  for(const auto& variable : variable_list)
    new(it++) Variable(std::move(variable));
  return allocated_memory;
}

Variable ArrayImplementation::insert(ArrayImplementation*& implementation, size_t at, Variable variable) {
  auto allocated_memory = insert(implementation, at, 1);
  new(allocated_memory) Variable(std::move(variable));
  return allocated_memory->move();
}

ArrayImplementation::Iterator ArrayImplementation::insert(ArrayImplementation*& implementation, size_t at, const literals::arr& variable_list) {
  auto allocated_memory = insert(implementation, at, variable_list.getSize());
  auto it = allocated_memory;
  for(const auto& variable : variable_list)
    new(it++) Variable(std::move(variable));
  return allocated_memory;
}

ArrayImplementation::Iterator ArrayImplementation::increaseSize(ArrayImplementation*& implementation, size_t count) {
  const size_t new_count = implementation->count + count;
  const size_t old_count = implementation->count;
  const size_t new_capacity = calculateCapacity(new_count);
  if(new_capacity == implementation->capacity) {
    ArrayImplementation* new_implementation = new(new byte[ new_capacity ]) ArrayImplementation(*implementation);
    new_implementation->count = new_count;
    new_implementation->capacity = new_capacity;
    delete implementation;
    implementation = new_implementation;
  } else {
    implementation->count = new_count;
  }
  return implementation->getData() + old_count;
}

void ArrayImplementation::reduceSize(ArrayImplementation*& implementation, size_t count) {
  if(count <= implementation->count) {
    implementation->count -= count;
    return;
  } else {
    implementation->count = 0;
    return;
  }
}

void ArrayImplementation::popBack(ArrayImplementation*& implementation, size_t count) {
  if(count <= implementation->count)
    for(auto it = implementation->getData() + implementation->count - count,
             end = implementation->end(); it != end; ++it)
      it->~Variable();
  else
    for(auto element : *implementation) element.~Variable();
  reduceSize(implementation, count);
}

void ArrayImplementation::popFront(ArrayImplementation*& implementation, size_t count) {remove(implementation, 0, count);}

ArrayImplementation::Iterator ArrayImplementation::insert(ArrayImplementation*& implementation, size_t at, size_t count) {
  size_t old_count = implementation->count;
  if(at >= old_count) return increaseSize(implementation, count);
  increaseSize(implementation, count);
  memmove(implementation->getData() + at + count,
          implementation->getData() + at,
          (old_count - at) * sizeof (Link));
  return implementation->getData() + at;
}

void ArrayImplementation::remove(ArrayImplementation*& implementation, size_t at, size_t count) {
  if(at >= implementation->count) return;
  if(at + count >= implementation->count)
    return popBack(implementation, implementation->count - at);

  for(auto it = implementation->getData() + at, end = implementation->getData() + at + count;
      it != end; ++it) it->~Variable();

  size_t old_count = implementation->count;
  memmove(implementation->getData() + at,
          implementation->getData() + at + count,
          (old_count - at - count) * sizeof (Link));
  return reduceSize(implementation, count);
}

void ArrayImplementation::clear(ArrayImplementation*& implementation) {popBack(implementation, implementation->getSize());}

Variable ArrayImplementation::operator[](size_t index) {
  if(index < getElementCount())
    return getData()[index].move();
  else return nullptr;
}

ArrayImplementation::Iterator ArrayImplementation::begin() {return getData();}
ArrayImplementation::Iterator ArrayImplementation::end() {return getData() + count;}
ArrayImplementation::ReverseIterator ArrayImplementation::rbegin() {return ArrayImplementation::ReverseIterator(getData() + count - 1);}
ArrayImplementation::ReverseIterator ArrayImplementation::rend() {return ArrayImplementation::ReverseIterator(getData() - 1);}
ArrayImplementation::ConstIterator ArrayImplementation::cbegin() const {return getData();}
ArrayImplementation::ConstIterator ArrayImplementation::cend() const {return getData() + count;}
ArrayImplementation::ConstReverseIterator ArrayImplementation::crbegin() const {return ArrayImplementation::ReverseIterator(getData() + count - 1);}
ArrayImplementation::ConstReverseIterator ArrayImplementation::crend() const {return ArrayImplementation::ReverseIterator(getData() - 1);}

void ArrayImplementation::operator delete(void* ptr) {
  for(auto element : *reinterpret_cast<ArrayImplementation*>(ptr)) element.~Variable();
  return ::delete [] reinterpret_cast<byte*>(ptr);
}

//////////////////////////////////////////////////////////// SinglyLinkedListImplementation ////////////////////////////////////////////////////////

#include "libbinom/include/variables/singly_linked_list.hxx"

SinglyLinkedListImplementation::SinglyLinkedListImplementation(const sllist& value_list) {pushBack(value_list);}

SinglyLinkedListImplementation::SinglyLinkedListImplementation(const SinglyLinkedListImplementation& other) {for(const auto& value : other) pushBack(value);}

SinglyLinkedListImplementation::~SinglyLinkedListImplementation() {clear();}

bool SinglyLinkedListImplementation::isEmpty() {return !first;}

void SinglyLinkedListImplementation::clear() {
  auto it = begin(), _end = end();
  while(it != _end) {
    Node* node = it.node;
    ++it;
    delete node;
  }
  first = last = nullptr;
}

Variable SinglyLinkedListImplementation::pushBack(Variable var) {
  if(!last) {
    first = last = new Node{std::move(var), nullptr};
  } else {
    last = last->next = new Node{std::move(var), nullptr};
  }
  return last->value.move();
}

SinglyLinkedListImplementation::Iterator SinglyLinkedListImplementation::pushBack(const literals::sllist& value_list) {
  Iterator result(nullptr, nullptr);

  auto it = value_list.begin();

  if(!last) {
    result.node = first = last = new Node{std::move(*it), nullptr};
  } else {
    result.prev = last;
    result.node = last = last->next = new Node{std::move(*it), nullptr};
  }

  for(auto end = value_list.end(); it != end; ++it)
      last = last->next = new Node{std::move(*it), nullptr};

  return result;
}

Variable SinglyLinkedListImplementation::pushFront(Variable var) {
  if(!first) {
    first = last = new Node{std::move(var), nullptr};
  } else {
    first = new Node{std::move(var), first};
  }
  return first->value.move();
}

SinglyLinkedListImplementation::Iterator SinglyLinkedListImplementation::pushFront(const literals::sllist& value_list) {
  Node* last_first = first;
  Node** ptr_it = &first; // Pointer at first or Node::next

  for(const auto& value : value_list) {
    (*ptr_it) = new Node{std::move(value), nullptr};
    ptr_it = &(*ptr_it)->next;
  }

  (*ptr_it)->next = last_first;

  return first;
}

SinglyLinkedListImplementation::Iterator SinglyLinkedListImplementation::insert(Iterator it, Variable var) {
  if(!it.node && !it.prev) return it;
  elif(!it.node && it.prev == last) {
    last = it.prev = it.prev->next = new Node{std::move(var), it.node};
  } elif(it.prev) {
    it.prev->next = new Node{std::move(var), it.node};
    it.node = it.prev->next;
  } elif (first == it.node) {
    first = new Node{std::move(var), it.node};
    it.node = first;
  }
  return it;
}

void SinglyLinkedListImplementation::remove(Iterator it) {
  if(!it.node) return;
  Node* removable_node = it.node;
  delete removable_node;
}

SinglyLinkedListImplementation::Iterator SinglyLinkedListImplementation::begin() const {return Iterator(first);}
SinglyLinkedListImplementation::Iterator SinglyLinkedListImplementation::end() const {return Iterator(nullptr, last);}

//////////////////////////////////////////////////////////// ArrayImplementation ////////////////////////////////////////////////////////

#include "libbinom/include/variables/doubly_linked_list.hxx"

DoublyLinkedListImplementation::DoublyLinkedListImplementation(const literals::dllist& value_list) {pushBack(value_list);}
DoublyLinkedListImplementation::DoublyLinkedListImplementation(const DoublyLinkedListImplementation& other) {for(const auto& value : other) pushBack(value);}

DoublyLinkedListImplementation::~DoublyLinkedListImplementation() {clear();}

bool DoublyLinkedListImplementation::isEmpty() {return !first;}

void DoublyLinkedListImplementation::clear() {
  auto it = begin(), _end = end();
  while(it != _end) {
    Node* node = it.node;
    ++it;
    delete node;
  }
  first = last = nullptr;
}

Variable DoublyLinkedListImplementation::pushBack(Variable var) {
  if(!last) {
    first = last = new Node{std::move(var), nullptr, nullptr};
  } else {
    last = last->next = new Node{std::move(var), nullptr, last};
  }
  return last->value.move();
}

DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::pushBack(const literals::dllist& value_list) {
  Iterator result(nullptr);

  auto it = value_list.begin();

  if(!last) {
    result.node = first = last = new Node{std::move(*it), nullptr, last};
  } else {
    result.node = last = last->next = new Node{std::move(*it), nullptr, last};
  }

  for(auto end = value_list.end(); it != end; ++it)
      last = last->next = new Node{std::move(*it), nullptr, last};

  return result;
}

Variable DoublyLinkedListImplementation::pushFront(Variable var) {
  if(!first) {
    first = last = new Node{std::move(var), nullptr, nullptr};
  } else {
    first = first->prev = new Node{std::move(var), first, nullptr};
  }
  return first->value.move();
}

DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::pushFront(const literals::dllist& value_list) {
  Node* last_first = first;
  Node** ptr_it = &first; // Pointer at first or Node::next
  Node* prev_node = nullptr;

  for(const auto& value : value_list) {
    prev_node = (*ptr_it) = new Node{std::move(value), nullptr, prev_node};
    ptr_it = &(*ptr_it)->next;
  }

  (*ptr_it)->next = last_first;
  last_first->prev = prev_node;

  return first;
}

DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::insert(Iterator it, Variable var) {
  if(!it.node) return it;
  elif(it.node == nullptr) {
    it.node = last = last->next = new Node{std::move(var), nullptr, last};
  } elif(it.node == first) {
    it.node = first = first->prev = new Node{std::move(var), first, nullptr};
  } else {
    it.node = it.node->prev->next = it.node->prev = new Node{std::move(var), it.node, it.node->prev};
  }
  return it;
}

void DoublyLinkedListImplementation::popBack() {
  if(!last) return;
  Node* prev_last = last;
  last = last->prev;
  if(first == prev_last) first = nullptr;
  delete prev_last;
}

void DoublyLinkedListImplementation::popFront() {
  if(!first) return;
  Node* prev_first = first;
  first = first->next;
  if(last == prev_first) last = nullptr;
  delete prev_first;
}

void DoublyLinkedListImplementation::remove(Iterator it) {
  if(it.node == first) return popFront();
  elif(it.node == last) return popBack();
}

DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::begin() const {return Iterator(first);}
DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::end() const {return Iterator(nullptr);}

DoublyLinkedListImplementation::ReverseIterator DoublyLinkedListImplementation::rbegin() const {return ReverseIterator(last);}
DoublyLinkedListImplementation::ReverseIterator DoublyLinkedListImplementation::rend() const {return ReverseIterator(nullptr);}


//////////////////////////////////////////////////////////// MapImplementation ////////////////////////////////////////////////////////


NamedVariable* MapImplementation::convert(AVLNode* node) {
  return reinterpret_cast<NamedVariable*>(
        reinterpret_cast<byte*>(node) + offsetof(NamedVariable, node)
                                );
}

const NamedVariable* MapImplementation::convert(const AVLNode* node) {
  return reinterpret_cast<const NamedVariable*>(
        reinterpret_cast<const byte*>(node) + offsetof(NamedVariable, node)
        );
}

MapImplementation::MapImplementation(const literals::map& map) {
  size = map.getSize();
  for(auto& element : map) {
    NamedVariable* named_variable = new NamedVariable(std::move(element));
    avl_tree.insert(&named_variable->node);
  }
}

MapImplementation::MapImplementation(const MapImplementation& other) {
  const AVLTree& tree = other.avl_tree;

//  NamedVariable*

//  for (const AVLNode& node : tree) {

//  }

}

bool MapImplementation::isEmpty() const noexcept {return avl_tree.isEmpty();}

size_t MapImplementation::getSize() const noexcept {return size;}

err::ProgressReport<NamedVariable> MapImplementation::insert(KeyValue key, Variable variable) {
  NamedVariable* named_variable = new NamedVariable(std::move(key), variable.move());
  if(!avl_tree.insert(&named_variable->node)) return err::ErrorType::binom_key_unique_error;
  ++size;
  return named_variable;
}

err::Error MapImplementation::remove(KeyValue key) {
  NamedVariable* named_variable = convert(avl_tree.extract(std::move(key)));
  if(!named_variable) return err::ErrorType::binom_out_of_range;
  delete named_variable;
  --size;
  return err::ErrorType::no_error;
}

err::ProgressReport<NamedVariable> MapImplementation::rename(KeyValue old_key, KeyValue new_key) {
  NamedVariable* named_variable = convert(avl_tree.extract(old_key));
  if(!named_variable) return err::ErrorType::binom_out_of_range;
  named_variable->node.getKey() = std::move(new_key);
  if(!avl_tree.insert(&named_variable->node)){
    named_variable->node.getKey() = std::move(old_key);
    avl_tree.insert(&named_variable->node);
    return err::ProgressReport(err::ErrorType::binom_key_unique_error, named_variable);
  }
  return named_variable;
}

NamedVariable& MapImplementation::getOrInsertNamedVariable(KeyValue key) {
  NamedVariable* named_variable = convert(avl_tree.get(key));
  if(!named_variable) return *insert(std::move(key), nullptr);
  return *named_variable;
}

Variable MapImplementation::getVariable(KeyValue key) {
  NamedVariable* named_variable = convert(avl_tree.get(key));
  if(!named_variable) return nullptr;
  return named_variable->getVariable();
}

MapImplementation::Iterator::Iterator(AVLTree::Iterator iterator) : iterator(std::move(iterator)) {}

MapImplementation::Iterator::Iterator(const Iterator& iterator) : iterator(std::move(iterator.iterator)) {}

MapImplementation::Iterator::Iterator(Iterator&& iterator) : iterator(std::move(iterator.iterator)) {}

MapImplementation::Iterator& MapImplementation::Iterator::operator=(Iterator& other) noexcept {return *new(this) Iterator(other);}

MapImplementation::Iterator& MapImplementation::Iterator::operator=(Iterator&& other) noexcept {return *new(this) Iterator(std::move(other));}

MapImplementation::Iterator& MapImplementation::Iterator::operator=(AVLNode* node) noexcept {return *new(this) Iterator(node);}

MapImplementation::Iterator& MapImplementation::Iterator::operator=(AVLNode& node) noexcept {return *new(this) Iterator(&node);}

MapImplementation::Iterator& MapImplementation::Iterator::operator++() { ++iterator; return self; }

MapImplementation::Iterator& MapImplementation::Iterator::operator--() { --iterator; return self; }

MapImplementation::Iterator MapImplementation::Iterator::operator++(int) {Iterator tmp(self); ++self; return tmp;}

MapImplementation::Iterator MapImplementation::Iterator::operator--(int) {Iterator tmp(self); --self; return tmp;}

const MapImplementation::Iterator& MapImplementation::Iterator::operator++() const { ++iterator; return self; }

const MapImplementation::Iterator& MapImplementation::Iterator::operator--() const { --iterator; return self; }

const MapImplementation::Iterator MapImplementation::Iterator::operator++(int) const {Iterator tmp(self); ++self; return tmp;}

const MapImplementation::Iterator MapImplementation::Iterator::operator--(int) const {Iterator tmp(self); --self; return tmp;}

bool MapImplementation::Iterator::operator==(const Iterator other) const noexcept {return iterator == other.iterator;}

bool MapImplementation::Iterator::operator!=(const Iterator other) const noexcept {return iterator != other.iterator;}

NamedVariable& MapImplementation::Iterator::operator*() {return *convert(&*iterator);}

NamedVariable* MapImplementation::Iterator::operator->() {return convert(&*iterator);}

const NamedVariable& MapImplementation::Iterator::operator*() const {return *convert(&*iterator);}

const NamedVariable* MapImplementation::Iterator::operator->() const {return convert(&*iterator);}




MapImplementation::ReverseIterator::ReverseIterator(AVLTree::ReverseIterator iterator) : iterator(std::move(iterator)) {}

MapImplementation::ReverseIterator::ReverseIterator(const ReverseIterator& iterator) : iterator(std::move(iterator.iterator)) {}

MapImplementation::ReverseIterator::ReverseIterator(ReverseIterator&& iterator) : iterator(std::move(iterator.iterator)) {}

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator=(ReverseIterator& other) noexcept {return *new(this) ReverseIterator(other);}

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator=(ReverseIterator&& other) noexcept {return *new(this) ReverseIterator(std::move(other));}

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator=(AVLNode* node) noexcept {return *new(this) ReverseIterator(node);}

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator=(AVLNode& node) noexcept {return *new(this) ReverseIterator(&node);}

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator++() { ++iterator; return self; }

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator--() { --iterator; return self; }

MapImplementation::ReverseIterator MapImplementation::ReverseIterator::operator++(int) {ReverseIterator tmp(self); ++self; return tmp;}

MapImplementation::ReverseIterator MapImplementation::ReverseIterator::operator--(int) {ReverseIterator tmp(self); --self; return tmp;}

const MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator++() const { ++iterator; return self; }

const MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator--() const { --iterator; return self; }

const MapImplementation::ReverseIterator MapImplementation::ReverseIterator::operator++(int) const {ReverseIterator tmp(self); ++self; return tmp;}

const MapImplementation::ReverseIterator MapImplementation::ReverseIterator::operator--(int) const {ReverseIterator tmp(self); --self; return tmp;}

bool MapImplementation::ReverseIterator::operator==(const ReverseIterator other) const noexcept {return iterator == other.iterator;}

bool MapImplementation::ReverseIterator::operator!=(const ReverseIterator other) const noexcept {return iterator != other.iterator;}

NamedVariable& MapImplementation::ReverseIterator::operator*() {return *convert(&*iterator);}

NamedVariable* MapImplementation::ReverseIterator::operator->() {return convert(&*iterator);}

const NamedVariable& MapImplementation::ReverseIterator::operator*() const {return *convert(&*iterator);}

const NamedVariable* MapImplementation::ReverseIterator::operator->() const {return convert(&*iterator);}
