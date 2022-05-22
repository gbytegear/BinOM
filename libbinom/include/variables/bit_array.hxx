#ifndef BIT_ARRAY_HXX
#define BIT_ARRAY_HXX

#include "variable.hxx"

namespace binom {

class BitArray : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator BufferArray& () = delete;
  operator Array& () = delete;
  operator List& () = delete;
  operator Map& () = delete;

  inline Number& toNumber() = delete;
  inline BitArray& toBitArray() = delete;
  inline BufferArray& toBufferArray() = delete;
  inline Array& toArray() = delete;
  inline List& toList() = delete;
  inline Map& toMap() = delete;

  inline priv::BitArrayHeader*& getData() const noexcept {return resource_link->data.bit_array_header;}

  friend class Variable;
  BitArray(priv::Link&& link) : Variable(std::move(link)) {}

public:
  typedef BitIterator Iterator;
  typedef BitValueRef ValueRef;
  typedef BitReverseIterator ReverseIterator;

  BitArray() : Variable(literals::bitarr{}) {}
  BitArray(const literals::bitarr bit_array) : Variable(bit_array) {}
  BitArray(const BitArray& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
  BitArray(BitArray&& other) noexcept : Variable(dynamic_cast<Variable&&>(other)) {}

  size_t getSize() const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return 0;
    return getData()->getBitSize();
  }

  ValueRef operator[] (size_t index) const {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return priv::Bits::getNullValue();
    return (*getData())[index];
  }

  inline ValueRef operator+=(bool value) {return pushBack(value);}
  inline Iterator operator+=(const literals::bitarr value_list) {return pushBack(value_list);}
  inline Iterator operator+=(BitArray&& other) {return pushBack(static_cast<BitArray&&>(other));}

  ValueRef pushBack(bool value) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullValue();
    auto it = priv::BitArrayHeader::increaseSize(getData(), 1);
    return (*it) = value;
  }

  Iterator pushBack(const literals::bitarr value_list) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullIterator();
    auto it = priv::BitArrayHeader::increaseSize(getData(), value_list.size());
    { auto data_it = it;
      for(auto value_it = value_list.begin(), value_end = value_list.end(); value_it != value_end; ++value_it, ++data_it)
        (*data_it) = *value_it;
    }
    return it;
  }

  Iterator pushBack(BitArray&& other) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullIterator();
    auto it = priv::BitArrayHeader::increaseSize(getData(), other.getSize());
    { auto data_it = it;
      for(auto value_it = other.begin(), value_end = other.end(); value_it != value_end; ++value_it, ++data_it)
        (*data_it) = *value_it;
    }
    return it;
  }

  ValueRef pushFront(bool value) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullIterator();
    auto it = priv::BitArrayHeader::insertBits(getData(), 0, 1);
    return (*it) = value;
  }

  Iterator pushFront(const literals::bitarr value_list) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullIterator();
    auto it = priv::BitArrayHeader::insertBits(getData(), 0, value_list.size());
    { auto data_it = it;
      for(auto value_it = value_list.begin(), value_end = value_list.end(); value_it != value_end; ++value_it, ++data_it)
        (*data_it) = *value_it;
    }
    return it;
  }

  Iterator pushFront(BitArray&& other) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullIterator();
    auto it = priv::BitArrayHeader::insertBits(getData(), 0, other.getSize());
    { auto data_it = it;
      for(auto value_it = other.begin(), value_end = other.end(); value_it != value_end; ++value_it, ++data_it)
        (*data_it) = *value_it;
    }
    return it;
  }

  ValueRef insert(size_t at, bool value) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullIterator();
    auto it = priv::BitArrayHeader::insertBits(getData(), at, 1);
    return (*it) = value;
  }

  Iterator insert(size_t at, const literals::bitarr value_list) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullIterator();
    auto it = priv::BitArrayHeader::insertBits(getData(), at, value_list.size());
    { auto data_it = it;
      for(auto value_it = value_list.begin(), value_end = value_list.end(); value_it != value_end; ++value_it, ++data_it)
        (*data_it) = *value_it;
    }
    return it;
  }

  Iterator insert(size_t at, BitArray&& other) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullIterator();
    auto it = priv::BitArrayHeader::insertBits(getData(), at, other.getSize());
    { auto data_it = it;
      for(auto value_it = other.begin(), value_end = other.end(); value_it != value_end; ++value_it, ++data_it)
        (*data_it) = *value_it;
    }
    return it;
  }

  void popBack(size_t size) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return;
    priv::BitArrayHeader::reduceSize(getData(), size >= getData()->getBitSize() ? getData()->getBitSize() : size);
  }

  void popFront(size_t size) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return;
    priv::BitArrayHeader::removeBits(getData(), 0, size);
  }

  void remove(size_t at, size_t size) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return;
    priv::BitArrayHeader::removeBits(getData(), at, size);
  }



  Iterator begin() const {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullIterator();
    return getData()->begin();
  }

  Iterator end() const {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullIterator();
    return getData()->end();
  }

  ReverseIterator rbegin() const {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullReverseIterator();
    return getData()->rbegin();
  }

  ReverseIterator rend() const {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullReverseIterator();
    return getData()->rend();
  }

};

}

#endif // BIT_ARRAY_HXX
