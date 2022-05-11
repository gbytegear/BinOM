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

  class Value {
    friend struct Bits;
    Bits* bits = nullptr;
    ui8 index = 0;
    Value(const Bits* bits, ui8 index) : bits(const_cast<Bits*>(bits)), index(index) {}
    Value() = default;
  public:
    Value(const Value& other) : bits(other.bits), index(other.index) {}
    Value(Value&& other) : bits(other.bits), index(other.index) {}

    bool isNull() const noexcept {return !bits;}

    inline Value& operator=(Value value) noexcept {if(!bits) return self; return self = bool(value);}

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

    Value& operator=(bool value) noexcept {
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

  static Value getNullValue() noexcept {return Value();}

  class Iterator {
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

    inline operator Value() const noexcept {if(isNull()) return Value(); return Value(bits, index);}
    Value operator*() const noexcept {if(isNull()) return Value(); return Value(self);}

  };

  static Iterator getNullIterator() noexcept {return Iterator();}

  typedef std::reverse_iterator<Iterator> ReverseIterator;

  Value operator[](ui8 index) const noexcept { return Value(this, index); }

  Iterator begin() const noexcept {return Iterator(this, 0);}
  Iterator end() const noexcept {return Iterator(this + 1, 0);}
  Iterator getItearatorAt(ui8 index) const noexcept {return Iterator(this, index);}
};

typedef Bits::Iterator BitIterator;
typedef Bits::ReverseIterator ReverseBitIterator;
typedef Bits::Value BitValue;

}

#endif // BITS_HXX
