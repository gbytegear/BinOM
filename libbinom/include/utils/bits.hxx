#ifndef BITS_HXX
#define BITS_HXX

#include "types.hxx"

namespace binom::priv {

struct Bits {
  bool bit_0:1;
  bool bit_1:1;
  bool bit_2:1;
  bool bit_3:1;
  bool bit_4:1;
  bool bit_5:1;
  bool bit_6:1;
  bool bit_7:1;

  class ValueRef;
  class Iterator;
  class ReverseIterator;

  static ValueRef getNullValue() noexcept;
  static Iterator getNullIterator() noexcept;
  static ReverseIterator getNullReverseIterator() noexcept;

  ValueRef operator[](ui8 index) const noexcept;

  Iterator begin() const noexcept;
  Iterator end() const noexcept;
  ReverseIterator rbegin() const noexcept;
  ReverseIterator rend() const noexcept;
  Iterator getItearatorAt(ui8 index) const noexcept;
  ReverseIterator getReverseIteratorAt(ui8 index) noexcept;
};




class Bits::ValueRef {
  friend struct Bits;
  Bits* bits = nullptr;
  ui8 index = 0;
  ValueRef(const Bits* bits, ui8 index) : bits(const_cast<Bits*>(bits)), index(index) {}
  ValueRef() = default;
public:
  ValueRef(const ValueRef& other) : bits(other.bits), index(other.index) {}
  ValueRef(ValueRef&& other) : bits(other.bits), index(other.index) {}

  bool isNull() const noexcept {return !bits;}

  inline ValueRef& operator=(ValueRef value) noexcept {if(!bits) return self; return self = bool(value);}

  operator bool() const {
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

  ValueRef& operator=(bool value) noexcept {
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
};


class Bits::Iterator {
  friend struct Bits;
  Bits* bits = nullptr;
  ui8 index = 0;
  Iterator() = default;
  Iterator(const Bits* bits, ui8 index) : bits(const_cast<Bits*>(bits)), index(index) {}
public:
  Iterator(const Iterator& other) : bits(other.bits), index(other.index) {}
  Iterator(Iterator&& other) : bits(other.bits), index(other.index) {}

  bool isNull() const noexcept {return !bits;}

  Iterator& operator=(Iterator value) noexcept {return *new(this) Iterator(std::move(value));}
  bool operator==(Iterator other) noexcept {return bits == other.bits && index == other.index;}
  bool operator!=(Iterator other) noexcept {return bits != other.bits || index != other.index;}

  Iterator& operator++() noexcept {
    if(isNull()) return self;
    if(index < 7) {++index;}
    else {index = 0; ++bits;}
    return self;
  }

  Iterator& operator--() noexcept {
    if(isNull()) return self;
    if(index > 0) {--index;}
    else {index = 7; --bits;}
    return self;
  }

  Iterator operator++(int) noexcept {
    Iterator tmp(self);
    if(isNull()) return tmp;
    ++self;
    return tmp;
  }

  Iterator operator--(int) noexcept {
    Iterator tmp(self);
    if(isNull()) return tmp;
    --self;
    return tmp;
  }

  inline operator ValueRef() const noexcept {if(isNull()) return ValueRef(); return ValueRef(bits, index);}
  ValueRef operator*() const noexcept {if(isNull()) return ValueRef(); return ValueRef(self);}

};


class Bits::ReverseIterator {
  friend struct Bits;
  Bits* bits = nullptr;
  ui8 index = 0;
  ReverseIterator() = default;
  ReverseIterator(const Bits* bits, ui8 index) : bits(const_cast<Bits*>(bits)), index(index) {}
public:
  ReverseIterator(const ReverseIterator& other) : bits(other.bits), index(other.index) {}
  ReverseIterator(ReverseIterator&& other) : bits(other.bits), index(other.index) {}

  bool isNull() const noexcept {return !bits;}

  ReverseIterator& operator=(ReverseIterator value) noexcept {return *new(this) ReverseIterator(std::move(value));}
  bool operator==(ReverseIterator other) noexcept {return bits == other.bits && index == other.index;}
  bool operator!=(ReverseIterator other) noexcept {return bits != other.bits || index != other.index;}

  ReverseIterator& operator++() noexcept {
    if(isNull()) return self;
    if(index > 0) {--index;}
    else {index = 7; --bits;}
    return self;
  }

  ReverseIterator& operator--() noexcept {
    if(isNull()) return self;
    if(index < 7) {++index;}
    else {index = 0; ++bits;}
    return self;
  }

  ReverseIterator operator++(int) noexcept {
    ReverseIterator tmp(self);
    if(isNull()) return tmp;
    --self;
    return tmp;
  }

  ReverseIterator operator--(int) noexcept {
    ReverseIterator tmp(self);
    if(isNull()) return tmp;
    ++self;
    return tmp;
  }

  inline operator ValueRef() const noexcept {if(isNull()) return ValueRef(); return ValueRef(bits, index);}
  ValueRef operator*() const noexcept {if(isNull()) return ValueRef(); return ValueRef(self);}

};


inline Bits::ValueRef Bits::getNullValue() noexcept {return ValueRef();}
inline Bits::Iterator Bits::getNullIterator() noexcept {return Iterator();}
inline Bits::ReverseIterator Bits::getNullReverseIterator() noexcept {return ReverseIterator();}
inline Bits::ValueRef Bits::operator[](ui8 index) const noexcept { return ValueRef(this, index); }
inline Bits::Iterator Bits::begin() const noexcept {return Iterator(this, 0);}
inline Bits::Iterator Bits::end() const noexcept {return Iterator(this + 1, 0);}
inline Bits::ReverseIterator Bits::rbegin() const noexcept {return ReverseIterator(this, 7);}
inline Bits::ReverseIterator Bits::rend() const noexcept {return ReverseIterator(this - 1, 7);}
inline Bits::Iterator Bits::getItearatorAt(ui8 index) const noexcept {return Iterator(this, index);}
inline Bits::ReverseIterator Bits::getReverseIteratorAt(ui8 index) noexcept {return ReverseIterator(this, index);}




}

namespace binom {

typedef priv::Bits::Iterator BitIterator;
typedef priv::Bits::ReverseIterator BitReverseIterator;
typedef priv::Bits::ValueRef BitValueRef;

}

#endif // BITS_HXX
