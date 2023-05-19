#include "libbinom/include/binom_impl/ram_storage_implementation/bit_array_impl.hxx"
#include "libbinom/include/utils/util_functions.hxx"
#include <cmath>
#include <cstring>

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;


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

void BitArrayImplementation::clear(BitArrayImplementation*& implementation) {
  reduceSize(implementation, implementation->bit_size);
  shrinkToFit(implementation);
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

const void* BitArrayImplementation::getDataPointer() const noexcept {
  return this + 1;
}

void BitArrayImplementation::operator delete(void* ptr) {return ::delete [] reinterpret_cast<byte*>(ptr);}

BitValueRef BitArrayImplementation::operator[](size_t index) const noexcept {return getData()[index / 8][index % 8];}

BitIterator BitArrayImplementation::begin() const noexcept {return getData()->begin();}

BitIterator BitArrayImplementation::end() const noexcept {return getData()[bit_size / 8].getItearatorAt(bit_size % 8);}

BitReverseIterator BitArrayImplementation::rbegin() const noexcept {if(!bit_size) return rend(); return getData()[(bit_size - 1) / 8].getReverseIteratorAt((bit_size - 1) % 8);}

BitReverseIterator BitArrayImplementation::rend() const noexcept {return getData()->rend();}

