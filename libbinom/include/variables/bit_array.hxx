#ifndef BIT_ARRAY_HXX
#define BIT_ARRAY_HXX

#include "variable.hxx"

namespace binom {

class BitArray : public Variable {

  static priv::BitIterator insertBits(priv::BitArrayHeader*& header, size_t at, size_t count);

public:
  typedef priv::BitIterator iterator;

  BitArray(const literals::bitarr bit_array) : Variable(bit_array) {}
  BitArray(const BitArray& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
  BitArray(BitArray&& other) noexcept : Variable(dynamic_cast<Variable&&>(other)) {}


};

}

#endif // BIT_ARRAY_HXX
