#include "libbinom/include/variables/bit_array.hxx"

using namespace binom;
using namespace binom::priv;


BitIterator BitArray::insertBits(priv::BitArrayHeader*& header, size_t at, size_t count) {
  const size_t shift_start_byte = at / 8;
  const size_t shift_start_bit_in_byte = at % 8;
  const size_t shift_end_byte = (at + count) / 8;
  const size_t shift_end_bit_in_byte = (at + count) % 8;

  priv::BitArrayHeader::increaseSize(header, count);
  ui8* data = header->getDataAs<ui8>();

  if(shift_start_byte <= header->getByteSize() - 1) {
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
