#ifndef BIT_ARRAY_HXX
#define BIT_ARRAY_HXX

#include "variable.hxx"

namespace binom {

class BitArray : public Variable {
public:
  typedef BitIterator iterator;

  BitArray(const literals::bitarr bit_array) : Variable(bit_array) {}
  BitArray(const BitArray& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
  BitArray(BitArray&& other) noexcept : Variable(dynamic_cast<Variable&&>(other)) {}

  BitValue operator[] (size_t index) const {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return priv::Bits::getNullValue();
    return (*resource_link->data.bit_array_header)[index];
  }

  inline BitValue operator+=(bool value) {return pushBack(value);}

  BitValue pushBack(bool value) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullValue();
    auto it = priv::BitArrayHeader::increaseSize(resource_link->data.bit_array_header, 1);
    return (*it) = value;
  }


  BitIterator pushBack(std::initializer_list<bool> value_list) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullIterator();
    auto it = priv::BitArrayHeader::increaseSize(resource_link->data.bit_array_header, value_list.size());
    { auto data_it = it;
      for(auto value_it = value_list.begin(), value_end = value_list.end(); value_it != value_end; ++value_it, ++data_it)
        (*data_it) = *value_it;
    }
    return it;
  }

  BitValue pushFront(bool value) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullIterator();
    auto it = priv::BitArrayHeader::insertBits(resource_link->data.bit_array_header, 0, 1);
    return (*it) = value;
  }

  BitIterator pushFront(std::initializer_list<bool> value_list) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return priv::Bits::getNullIterator();
    auto it = priv::BitArrayHeader::insertBits(resource_link->data.bit_array_header, 0, value_list.size());
    { auto data_it = it;
      for(auto value_it = value_list.begin(), value_end = value_list.end(); value_it != value_end; ++value_it, ++data_it)
        (*data_it) = *value_it;
    }
    return it;
  }

  BitValue insert(bool value) {

  }



};

}

#endif // BIT_ARRAY_HXX
