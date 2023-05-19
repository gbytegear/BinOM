#include "libbinom/include/binom_impl/bits.hxx"

using namespace binom;
using namespace binom::priv;

Bits::ValueRef Bits::getNullValue() noexcept {return ValueRef();}
Bits::Iterator Bits::getNullIterator() noexcept {return Iterator();}
Bits::ReverseIterator Bits::getNullReverseIterator() noexcept {return ReverseIterator();}
Bits::ValueRef Bits::operator[](ui8 index) const noexcept { return ValueRef(this, index); }
Bits::Iterator Bits::begin() const noexcept {return Iterator(this, 0);}
Bits::Iterator Bits::end() const noexcept {return Iterator(this + 1, 0);}
Bits::ReverseIterator Bits::rbegin() const noexcept {return ReverseIterator(this, 7);}
Bits::ReverseIterator Bits::rend() const noexcept {return ReverseIterator(this - 1, 7);}
Bits::Iterator Bits::getItearatorAt(ui8 index) const noexcept {return Iterator(this, index);}
Bits::ReverseIterator Bits::getReverseIteratorAt(ui8 index) noexcept {return ReverseIterator(this, index);}




Bits::ValueRef::ValueRef(const Bits* bits, ui8 index) : bits(const_cast<Bits*>(bits)), index(index) {}
Bits::ValueRef::ValueRef(const ValueRef& other) : bits(other.bits), index(other.index) {}
Bits::ValueRef::ValueRef(ValueRef&& other) : bits(other.bits), index(other.index) {}

bool Bits::ValueRef::isNull() const noexcept {return !bits;}

binom::priv::Bits::ValueRef& Bits::ValueRef::operator=(ValueRef value) noexcept {if(!bits) return self; return self = bool(value);}

Bits::ValueRef::operator bool() const {
  if(!bits) return false;
  switch (index) {
  case 0: return bits->bit_0;
  case 1: return bits->bit_1;
  case 2: return bits->bit_2;
  case 3: return bits->bit_3;
  case 4: return bits->bit_4;
  case 5: return bits->bit_5;
  case 6: return bits->bit_6;
  case 7: return bits->bit_7;
  default: return false;
  }
}

binom::priv::Bits::ValueRef& Bits::ValueRef::operator=(bool value) noexcept {
  if(!bits) return self;
  switch (index) {
  case 0: bits->bit_0 = value; break;
  case 1: bits->bit_1 = value; break;
  case 2: bits->bit_2 = value; break;
  case 3: bits->bit_3 = value; break;
  case 4: bits->bit_4 = value; break;
  case 5: bits->bit_5 = value; break;
  case 6: bits->bit_6 = value; break;
  case 7: bits->bit_7 = value; break;
  default: break;
  }
  return self;
}




Bits::Iterator::Iterator(const Bits* bits, ui8 index) : bits(const_cast<Bits*>(bits)), index(index) {}

Bits::Iterator::Iterator(const Iterator& other) : bits(other.bits), index(other.index) {}

Bits::Iterator::Iterator(Iterator&& other) : bits(other.bits), index(other.index) {}

bool Bits::Iterator::isNull() const noexcept {return !bits;}

Bits::Iterator::operator bool() const noexcept {return !bits;}

binom::priv::Bits::Iterator& Bits::Iterator::operator=(Iterator value) noexcept {return *new(this) Iterator(std::move(value));}

bool Bits::Iterator::operator==(Iterator other) noexcept {return bits == other.bits && index == other.index;}

bool Bits::Iterator::operator!=(Iterator other) noexcept {return bits != other.bits || index != other.index;}

binom::priv::Bits::Iterator& Bits::Iterator::operator++() noexcept {
  if(isNull()) return self;
  if(index < 7) {++index;}
  else {index = 0; ++bits;}
  return self;
}

binom::priv::Bits::Iterator& Bits::Iterator::operator--() noexcept {
  if(isNull()) return self;
  if(index > 0) {--index;}
  else {index = 7; --bits;}
  return self;
}

binom::priv::Bits::Iterator &Bits::Iterator::operator+=(std::size_t shift) noexcept {
  std::size_t shift_from_byte_start = index + shift;
  bits += shift_from_byte_start / 8;
  index = shift_from_byte_start % 8;
  return self;
}

binom::priv::Bits::Iterator &Bits::Iterator::operator-=(std::size_t shift) noexcept {
  std::size_t shift_from_byte_end = shift + (7 - index);
  bits -= shift_from_byte_end / 8;
  index = 7 - (shift_from_byte_end % 8);
  return self;
}

binom::priv::Bits::Iterator Bits::Iterator::operator+(std::size_t shift) const noexcept {
  return Iterator(self) += shift;
}

binom::priv::Bits::Iterator Bits::Iterator::operator-(std::size_t shift) const noexcept {
  return Iterator(self) -= shift;
}

const binom::priv::Bits::Iterator& Bits::Iterator::operator++() const noexcept {
  if(isNull()) return self;
  if(index < 7) {++index;}
  else {index = 0; ++bits;}
  return self;
}

const binom::priv::Bits::Iterator& Bits::Iterator::operator--() const noexcept {
  if(isNull()) return self;
  if(index > 0) {--index;}
  else {index = 7; --bits;}
  return self;
}

const binom::priv::Bits::Iterator Bits::Iterator::operator++(int) const noexcept {
  Iterator tmp(self);
  if(isNull()) return tmp;
  ++self;
  return tmp;
}

const binom::priv::Bits::Iterator Bits::Iterator::operator--(int) const noexcept {
  Iterator tmp(self);
  if(isNull()) return tmp;
  --self;
  return tmp;
}

binom::priv::Bits::Iterator Bits::Iterator::operator++(int) noexcept {
  Iterator tmp(self);
  if(isNull()) return tmp;
  ++self;
  return tmp;
}

binom::priv::Bits::Iterator Bits::Iterator::operator--(int) noexcept {
  Iterator tmp(self);
  if(isNull()) return tmp;
  --self;
  return tmp;
}

Bits::Iterator::operator ValueRef() noexcept {if(isNull()) return ValueRef(); return ValueRef(bits, index);}

Bits::Iterator::operator const ValueRef() const noexcept {if(isNull()) return ValueRef(); return ValueRef(bits, index);}

Bits::ValueRef Bits::Iterator::operator*() noexcept {if(isNull()) return ValueRef(); return ValueRef(self);}

pseudo_ptr::PseudoPointer<Bits::ValueRef> Bits::Iterator::operator->() noexcept { return Bits::ValueRef(self); }

pseudo_ptr::PseudoPointer<const Bits::ValueRef> Bits::Iterator::operator->() const noexcept { return Bits::ValueRef(self); }

const Bits::ValueRef Bits::Iterator::operator*() const noexcept {if(isNull()) return ValueRef(); return ValueRef(self);}




Bits::ReverseIterator::ReverseIterator(const Bits* bits, ui8 index) : bits(const_cast<Bits*>(bits)), index(index) {}

Bits::ReverseIterator::ReverseIterator(const ReverseIterator& other) : bits(other.bits), index(other.index) {}

Bits::ReverseIterator::ReverseIterator(ReverseIterator&& other) : bits(other.bits), index(other.index) {}

bool Bits::ReverseIterator::isNull() const noexcept {return !bits;}

binom::priv::Bits::ReverseIterator& Bits::ReverseIterator::operator=(ReverseIterator value) noexcept {return *new(this) ReverseIterator(std::move(value));}

bool Bits::ReverseIterator::operator==(ReverseIterator other) const noexcept {return bits == other.bits && index == other.index;}

bool Bits::ReverseIterator::operator!=(ReverseIterator other) const noexcept {return bits != other.bits || index != other.index;}

binom::priv::Bits::ReverseIterator& Bits::ReverseIterator::operator++() noexcept {
  if(isNull()) return self;
  if(index > 0) {--index;}
  else {index = 7; --bits;}
  return self;
}

binom::priv::Bits::ReverseIterator& Bits::ReverseIterator::operator--() noexcept {
  if(isNull()) return self;
  if(index < 7) {++index;}
  else {index = 0; ++bits;}
  return self;
}

const binom::priv::Bits::ReverseIterator& Bits::ReverseIterator::operator++() const noexcept {
  if(isNull()) return self;
  if(index > 0) {--index;}
  else {index = 7; --bits;}
  return self;
}

const binom::priv::Bits::ReverseIterator& Bits::ReverseIterator::operator--() const noexcept {
  if(isNull()) return self;
  if(index < 7) {++index;}
  else {index = 0; ++bits;}
  return self;
}

binom::priv::Bits::ReverseIterator Bits::ReverseIterator::operator++(int) noexcept {
  ReverseIterator tmp(self);
  if(isNull()) return tmp;
  --self;
  return tmp;
}

binom::priv::Bits::ReverseIterator Bits::ReverseIterator::operator--(int) noexcept {
  ReverseIterator tmp(self);
  if(isNull()) return tmp;
  ++self;
  return tmp;
}

const binom::priv::Bits::ReverseIterator Bits::ReverseIterator::operator++(int) const noexcept {
  ReverseIterator tmp(self);
  if(isNull()) return tmp;
  --self;
  return tmp;
}

const binom::priv::Bits::ReverseIterator Bits::ReverseIterator::operator--(int) const noexcept {
  ReverseIterator tmp(self);
  if(isNull()) return tmp;
  ++self;
  return tmp;
}

Bits::ReverseIterator::operator ValueRef() noexcept {if(isNull()) return ValueRef(); return ValueRef(bits, index);}

Bits::ValueRef Bits::ReverseIterator::operator*() noexcept {if(isNull()) return ValueRef(); return ValueRef(self);}

Bits::ReverseIterator::operator const ValueRef() const noexcept {if(isNull()) return ValueRef(); return ValueRef(bits, index);}

const Bits::ValueRef Bits::ReverseIterator::operator*() const noexcept {if(isNull()) return ValueRef(); return ValueRef(self);}
