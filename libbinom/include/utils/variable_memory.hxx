#ifndef VARIABLE_MEMORY_HXX
#define VARIABLE_MEMORY_HXX

#include "types.hxx"
#include "util_functions.hxx"
#include "bits.hxx"
#include "extended_type_traits.hxx"
#include <iterator>
#include <cmath>
#include <new>


namespace binom::priv {
using namespace extended_type_traits;

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
    return new(new byte[ calculateCapacity(bit_array_data.size()) ]) BitArrayHeader(bit_array_data);
  }

  static BitArrayHeader* copy(const BitArrayHeader* other) {
    return new(new byte[ other->capacity ]) BitArrayHeader(*other);
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
    using namespace util_functions;
    const size_t shift_start_byte = at / 8;
    const size_t shift_end_byte = (at + count) / 8;
    const ui8 shift_start_bit_in_byte = at % 8;
    const ui8 shift_end_bit_in_byte = (at + count) % 8;

    priv::BitArrayHeader::increaseSize(header, count);
    ui8* data = header->getDataAs<ui8>();

    if(!shift_start_bit_in_byte && !shift_end_bit_in_byte) {
      doLeftShift(data, header->getByteSize() - shift_start_byte, count);
      return header->getData()[shift_start_byte].begin();
    }

    if(shift_start_byte != shift_end_byte || shift_start_byte != header->getByteSize() - 1) {
      doLeftShift(data + shift_start_byte + 1, header->getByteSize() - shift_start_byte - 1, count);
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
    return header->getData()[shift_start_byte].getItearatorAt(shift_start_bit_in_byte);
  }

  static void removeBits(priv::BitArrayHeader*& header, size_t at, size_t count) {
    using namespace util_functions;
    if(at >= header->bit_size) return;
    if(at + count >= header->bit_size)
      return priv::BitArrayHeader::reduceSize(header, count - (count - (header->bit_size - at)));

    {
      const size_t shift_start_byte = at / 8;
      const size_t shift_start_bit_in_byte = at % 8;
      const size_t shift_end_byte = (at + count) / 8;
      const size_t shift_end_bit_in_byte = (at + count) % 8;
      ui8* data = header->getDataAs<ui8>();

      if(!shift_start_bit_in_byte)
        doRightShift(data + shift_start_byte, header->getByteSize() - shift_start_byte, count);
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
        doRightShift(data + shift_start_byte + 1, header->getByteSize() - shift_start_byte - 1, count);
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
        doRightShift(data + shift_start_byte + 1, header->getByteSize() - shift_start_byte - 1, count);
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
        doRightShift(data + shift_start_byte + 1, header->getByteSize() - shift_start_byte - 1, count);
      }

      return priv::BitArrayHeader::reduceSize(header, count);
    }
  }

  static void shrinkToFit(BitArrayHeader*& header) {
    if(header->getByteSize() == header->capacity) return;
    header->capacity = header->getByteSize();
    BitArrayHeader* old_pointer = header;
    header = new(new byte[ sizeof(BitArrayHeader) + header->capacity ]) BitArrayHeader(*old_pointer);
    delete old_pointer;
  }

  static inline constexpr size_t calculateCapacity(size_t bit_count) noexcept {
    return util_functions::getNearestPow2(sizeof(BitArrayHeader) + calculateByteSize(bit_count));
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


class BufferArrayHeader {
  size_t size = 0;
  size_t capacity = util_functions::getNearestPow2(sizeof(BufferArrayHeader));

  BufferArrayHeader(const literals::ui8arr& value_list)
    : size(value_list.size()), capacity(calculateCapacity(size)) {std::memcpy(getData(), value_list.begin(), size);}

  template<typename T>
  BufferArrayHeader(const std::initializer_list<T>& value_list)
    : size(value_list.size() * sizeof(T)), capacity(calculateCapacity(size)) {
    static_assert (std::is_arithmetic_v<T>, "T isn't arithmetic type:"
                                            " as argument requires const std::initializer_list<T>&"
                                            " where assertion std::is_arithmetic<T>::value is true");
    std::memcpy(getData(), value_list.begin(), size);
  }

  BufferArrayHeader(const BufferArrayHeader& other)
    : size(other.size), capacity(other.capacity) {
    memcpy(getData(), other.getData(), size);
  }

  static inline constexpr size_t calculateCapacity(size_t size) noexcept {
    return util_functions::getNearestPow2(sizeof(BufferArrayHeader) + size);
  }

public:
  template<typename T>
  static BufferArrayHeader* create(const std::initializer_list<T>& value_list) {
    return new(new byte[ calculateCapacity(value_list.size() * sizeof(T)) ]) BufferArrayHeader(value_list);
  }

  static BufferArrayHeader* copy(const BufferArrayHeader* other) {
    return new(new byte[ sizeof(BufferArrayHeader) + other->capacity ]) BufferArrayHeader(*other);
  }

  void* getData() { return reinterpret_cast<void*>(this + 1); }
  const void* getData() const { return reinterpret_cast<const void*>(this + 1); }

  template<typename T>
  T* getDataAs() const {return reinterpret_cast<T*>(this + 1);}

  inline size_t getSize() const noexcept {return size;}
  inline size_t getCapacity() const noexcept {return capacity;}

  inline void operator delete(void* ptr) {return ::delete [] reinterpret_cast<byte*>(ptr);}

};

}




#endif // VARIABLE_MEMORY_HXX
