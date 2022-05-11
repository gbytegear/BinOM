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
    return new(new byte[ sizeof(BitArrayHeader) + calculateCapacity(bit_array_data.size()) ]) BitArrayHeader(bit_array_data);;
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

  BitValue operator[](size_t index) const noexcept {return getData()[index / 8][index % 8];}

  BitIterator begin() const noexcept {return getData()->begin();}
  BitIterator end() const noexcept {return getData()[bit_size / 8].getItearatorAt(bit_size % 8);}

};


}




#endif // VARIABLE_MEMORY_HXX
