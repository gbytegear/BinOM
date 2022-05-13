#ifndef VARIABLE_MEMORY_HXX
#define VARIABLE_MEMORY_HXX

#include "types.hxx"
#include "util_functions.hxx"
#include "bits.hxx"
#include <iterator>
#include <cmath>
#include <new>


namespace binom::priv {

class BitArrayHeader {
  ui64 bit_size = 0;
  size_t capacity = 0;

  BitArrayHeader(const literals::bitarr& bit_array_data)
    : bit_size(bit_array_data.size()), capacity(calculateCapacity(bit_array_data.size())) {
    auto data_it = bit_array_data.begin();
    for(auto it = begin(), _end = end(); it != _end; ++it, ++data_it)
      (*it) = *data_it;
  }

  BitArrayHeader(const BitArrayHeader& other)
    : bit_size(other.bit_size), capacity(other.capacity) {
    memcpy(getData(), other.getData(), getByteSize());
  }

public:
  static BitArrayHeader* create(const literals::bitarr& bit_array_data) {
    return new(new byte[ sizeof(BitArrayHeader) + calculateCapacity(bit_array_data.size()) ]) BitArrayHeader(bit_array_data);
  }

  static BitArrayHeader* copy(const BitArrayHeader* other) {
    return new(new byte[ sizeof(BitArrayHeader) + other->capacity ]) BitArrayHeader(*other);
  }

  static inline constexpr size_t calculateByteSize(size_t bit_count) noexcept {
    return size_t(std::ceil(llf_t(bit_count)/ 8));
  }

  static BitIterator increaseSize(BitArrayHeader*& header, size_t bit_count) {
    size_t new_bit_size = header->bit_size + bit_count;
    size_t new_capacity = calculateCapacity(new_bit_size);
    if(header->capacity != new_capacity) {
      size_t old_bit_size = header->bit_size;
      BitArrayHeader* old_pointer = header;
      header = new(new byte[ sizeof(BitArrayHeader) + new_capacity ]) BitArrayHeader(*old_pointer);
      header->bit_size = new_bit_size;
      header->capacity = new_capacity;
      delete old_pointer;
      return header->getData()[old_bit_size / 8].getItearatorAt(old_bit_size % 8);
    } else {
      BitIterator it = header->end();
      header->bit_size = new_bit_size;
      return it;
    }
  }

  static void reduceSize(BitArrayHeader*& header, size_t bit_count) {
    if(bit_count <= header->bit_size) {
      header->bit_size -= bit_count;
      return;
    } else return;
  }

  static BitIterator insertBits(priv::BitArrayHeader*& header, size_t at, size_t count) {
    const size_t shift_start_byte = at / 8;
    const size_t shift_start_bit_in_byte = at % 8;
    const size_t shift_end_byte = (at + count) / 8;
    const size_t shift_end_bit_in_byte = (at + count) % 8;

    const size_t old_byte_size = header->getByteSize();
    priv::BitArrayHeader::increaseSize(header, count);
    ui8* data = header->getDataAs<ui8>();

    if(shift_start_byte < old_byte_size - 1) {
      utilfunc::doLeftShift(data + shift_start_byte + 1, header->getByteSize() - shift_start_byte - 1, count);
      if((8 - shift_start_bit_in_byte) > (8 - shift_end_bit_in_byte)) {
        data[shift_end_byte + 1] |= data[shift_start_byte] >> (8 - count % 8);
        data[shift_end_byte] |= data[shift_start_byte] >> shift_start_bit_in_byte << (shift_start_bit_in_byte + count % 8);
      } else {
        data[shift_end_byte] |= data[shift_start_byte] >> (8 - count % 8);
      }
    }

    return header->getData()[shift_start_byte].getItearatorAt(shift_start_bit_in_byte);
  }

  static void removeBits(priv::BitArrayHeader*& header, size_t at, size_t count) {
    if(at >= header->bit_size) return;
    if(at + count >= header->bit_size)
      return priv::BitArrayHeader::reduceSize(header, count - (count - (header->bit_size - at)));

    const size_t shift_start_byte = at / 8;
    const size_t shift_start_bit_in_byte = at % 8;
    const size_t shift_end_byte = (at + count) / 8;
    const size_t shift_end_bit_in_byte = (at + count) % 8;
    ui8* data = header->getDataAs<ui8>();

    if(!shift_start_bit_in_byte)
      utilfunc::doRightShift(data + shift_start_byte, header->getByteSize() - shift_start_byte, count);
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
      utilfunc::doRightShift(data + shift_start_byte + 1, header->getByteSize() - shift_start_byte - 1, count);
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
      utilfunc::doRightShift(data + shift_end_byte + 1, header->getByteSize() - shift_end_byte - 1, count);
    } else {
      /* Example:
       * [abcdefgh]...[ijklmnop] rm 6 at 4 => [abcd####]...[##efghjk] (end_byte = n, end_bit_in_byte = 2)
       * [11111111] >> (8 - 4) = [11110000], [abcd####] & [11110000] = [abcd0000]
       * [11111111] << 2 = [00111111], [##efghjk] & [00111111] = [00efghjk], [00efghjk] << (4 - 2) = [0000efgh]
       * [abcd0000] | [0000efgh] = [abcdefgh]
      */

      data[shift_start_byte] =
          (data[shift_start_byte] & (0xFF >> (8 - shift_start_bit_in_byte))) |
          ((data[shift_end_byte] & (0xFF << shift_end_bit_in_byte)) << (shift_start_bit_in_byte - shift_end_bit_in_byte));
      utilfunc::doRightShift(data + shift_end_byte, header->getByteSize() - shift_end_byte, count);
    }

    return priv::BitArrayHeader::reduceSize(header, count);
  }

  static void shrinkToFit(BitArrayHeader*& header) {
    if(header->getByteSize() == header->capacity) return;
    header->capacity = header->getByteSize();
    BitArrayHeader* old_pointer = header;
    header = new(new byte[ sizeof(BitArrayHeader) + header->capacity ]) BitArrayHeader(*old_pointer);
    delete old_pointer;
  }

  static inline constexpr size_t calculateCapacity(size_t bit_count) noexcept {
    return utilfunc::getNearestPow2(sizeof(BitArrayHeader) + calculateByteSize(bit_count));
  }

  inline size_t getCapacity() const noexcept {return capacity;}
  inline size_t getBitSize() const noexcept {return bit_size;}
  inline size_t getByteSize() const noexcept {return calculateByteSize(bit_size);}
  inline Bits* getData() const noexcept {
    return reinterpret_cast<Bits*>(const_cast<BitArrayHeader*>(this)) + sizeof(BitArrayHeader);
  }

  template<typename T>
  inline T* getDataAs() const noexcept {
    return reinterpret_cast<T*>(reinterpret_cast<ui8*>(const_cast<BitArrayHeader*>(this)) + sizeof(BitArrayHeader));
  }

  inline void operator delete(void* ptr) {return ::delete [] reinterpret_cast<byte*>(ptr);}

  BitValueRef operator[](size_t index) const noexcept {return getData()[index / 8][index % 8];}

  BitIterator begin() const noexcept {return getData()->begin();}
  BitIterator end() const noexcept {return getData()[bit_size / 8].getItearatorAt(bit_size % 8);}
  BitReverseIterator rbegin() const noexcept {if(!bit_size) return rend(); return getData()[(bit_size - 1) / 8].getReverseIteratorAt((bit_size - 1) % 8);}
  BitReverseIterator rend() const noexcept {return getData()->rend();}
};


}




#endif // VARIABLE_MEMORY_HXX
