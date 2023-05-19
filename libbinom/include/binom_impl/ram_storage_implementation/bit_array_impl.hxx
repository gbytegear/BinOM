#ifndef BIT_ARRAY_IMPL_HXX
#define BIT_ARRAY_IMPL_HXX

#include "../bits.hxx"

namespace binom::priv {

class BitArrayImplementation {
  ui64 bit_size = 0;
  size_t capacity = 0;

  BitArrayImplementation(const literals::bitarr& bit_array_data);
  BitArrayImplementation(const BitArrayImplementation& other);

  static BitIterator increaseSize(BitArrayImplementation*& implementation, size_t bit_count);
  static void reduceSize(BitArrayImplementation*& implementation, size_t bit_count);
  static BitIterator insertBits(priv::BitArrayImplementation*& implementation, size_t at, size_t count);
public:
  static BitArrayImplementation* create(const literals::bitarr& bit_array_data);
//  static BitArrayImplementation* create()
  static BitArrayImplementation* copy(const BitArrayImplementation* other);

  static size_t calculateByteSize(size_t bit_count) noexcept;
  static size_t calculateCapacity(size_t bit_count) noexcept;

  static BitValueRef pushBack(BitArrayImplementation*& implementation, bool value);
  static BitIterator pushBack(BitArrayImplementation*& implementation, const literals::bitarr& value_list);

  static BitValueRef pushFront(BitArrayImplementation*& implementation, bool value);
  static BitIterator pushFront(BitArrayImplementation*& implementation, const literals::bitarr& value_list);

  static BitValueRef insert(BitArrayImplementation*& implementation, size_t at, bool value);
  static BitIterator insert(BitArrayImplementation*& implementation, size_t at, const literals::bitarr value_list);

  static void popBack(BitArrayImplementation*& implementation, size_t size);
  static void popFront(BitArrayImplementation*& implementation, size_t size);
  static void removeBits(BitArrayImplementation*& implementation, size_t at, size_t count);
  static void clear(BitArrayImplementation*& implementation);

  static void shrinkToFit(BitArrayImplementation*& implementation);

  size_t getCapacity() const noexcept;
  size_t getBitSize() const noexcept;
  size_t getByteSize() const noexcept;
  Bits* getData() const noexcept;
  const void* getDataPointer() const noexcept;

  template<typename T>
  inline T* getDataAs() const noexcept {
    return reinterpret_cast<T*>(reinterpret_cast<ui8*>(const_cast<BitArrayImplementation*>(this)) + sizeof(BitArrayImplementation));
  }

  void operator delete(void* ptr);

  BitValueRef operator[](size_t index) const noexcept;

  BitIterator begin() const noexcept;
  BitIterator end() const noexcept;
  BitReverseIterator rbegin() const noexcept;
  BitReverseIterator rend() const noexcept;
};


}

#endif // BIT_ARRAY_IMPL_HXX
