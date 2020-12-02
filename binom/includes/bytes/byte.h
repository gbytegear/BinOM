#ifndef BYTE_H
#define BYTE_H

#include "types.h"

class Byte {
public:

  typedef bool bit;
  class BitPtr {
    Byte* byte_ptr;
    ui8 bit_ptr:3;
    BitPtr(Byte* byte_ptr, ui8 bit_ptr) : byte_ptr(byte_ptr), bit_ptr(bit_ptr) {}
    friend class Byte;
  public:
    BitPtr(const BitPtr& other) : byte_ptr(other.byte_ptr), bit_ptr(other.bit_ptr) {}
    BitPtr(const BitPtr&& other) : byte_ptr(other.byte_ptr), bit_ptr(other.bit_ptr) {}
    BitPtr& operator=(BitPtr& other) {byte_ptr = other.byte_ptr; bit_ptr = other.bit_ptr; return *this;}
    bit operator=(bit val) {return byte_ptr->set(bit_ptr, val);}
    bit operator*() {return (*byte_ptr)[bit_ptr];}
  };

private:

  union ByteData{
    struct Bits {
      bit b0:1;
      bit b1:1;
      bit b2:1;
      bit b3:1;
      bit b4:1;
      bit b5:1;
      bit b6:1;
      bit b7:1;
    } bits;
    bool bl;
    i8 ch;
    ui8 uch = 0;
  } byte;

public:
  Byte() = default;
  Byte(const Byte& copied) noexcept {byte.uch = copied.byte.uch;}
  Byte(const Byte&& moved) noexcept {byte.uch = moved.byte.uch;}

  Byte(const i8 signed_val) noexcept {byte.ch = signed_val;}
  Byte(const i16 signed_val) noexcept {byte.ch = signed_val;}
  Byte(const i32 signed_val) noexcept {byte.ch = signed_val;}
  Byte(const i64 signed_val) noexcept {byte.ch = signed_val;}

  Byte(const ui8 unsigned_val) noexcept {byte.uch = unsigned_val;}
  Byte(const ui16 unsigned_val) noexcept {byte.uch = unsigned_val;}
  Byte(const ui32 unsigned_val) noexcept {byte.uch = unsigned_val;}
  Byte(const ui64 unsigned_val) noexcept {byte.uch = unsigned_val;}
  Byte(const bool bl) noexcept {byte.bl = bl;}

  ui8& asUnsigned() const noexcept {return *const_cast<ui8*>(&byte.uch);}
  i8& asSigned() const noexcept {return *const_cast<i8*>(&byte.ch);}
  bool& asBool() const noexcept {return *const_cast<bool*>(&byte.bl);}


  Byte& operator++() noexcept {++byte.uch; return *this;}
  Byte operator++(int) noexcept {Byte tmp(byte.uch++); return tmp;}

  Byte& operator--() noexcept {--byte.uch; return *this;}
  Byte operator--(int) noexcept {Byte tmp(byte.uch--); return tmp;}

  Byte operator~() {return Byte(~byte.uch);}

  Byte& operator=(const Byte& other) noexcept {byte.uch = other.byte.uch; return *this;}
  Byte& operator+=(const Byte& other) noexcept {byte.uch += other.byte.uch; return *this;}
  Byte& operator-=(const Byte& other) noexcept {byte.uch -= other.byte.uch; return *this;}
  Byte& operator*=(const Byte& other) noexcept {byte.uch *= other.byte.uch; return *this;}
  Byte& operator/=(const Byte& other) noexcept {byte.uch /= other.byte.uch; return *this;}
  Byte& operator%=(const Byte& other) noexcept {byte.uch %= other.byte.uch; return *this;}
  Byte& operator&=(const Byte& other) noexcept {byte.uch &= other.byte.uch; return *this;}
  Byte& operator|=(const Byte& other) noexcept {byte.uch |= other.byte.uch; return *this;}
  Byte& operator^=(const Byte& other) noexcept {byte.uch ^= other.byte.uch; return *this;}
  Byte& operator<<=(const Byte& other) noexcept {byte.uch <<= other.byte.uch; return *this;}
  Byte& operator>>=(const Byte& other) noexcept {byte.uch >>= other.byte.uch; return *this;}

  Byte operator+(const Byte& other) const noexcept {return Byte(ui8(byte.uch + other.byte.uch));  }
  Byte operator-(const Byte& other) const noexcept {return Byte(ui8(byte.uch - other.byte.uch));  }
  Byte operator*(const Byte& other) const noexcept {return Byte(ui8(byte.uch * other.byte.uch));  }
  Byte operator/(const Byte& other) const noexcept {return Byte(ui8(byte.uch / other.byte.uch));  }
  Byte operator%(const Byte& other) const noexcept {return Byte(ui8(byte.uch % other.byte.uch));  }
  Byte operator&(const Byte& other) const noexcept {return Byte(ui8(byte.uch & other.byte.uch));  }
  Byte operator|(const Byte& other) const noexcept {return Byte(ui8(byte.uch | other.byte.uch));  }
  Byte operator^(const Byte& other) const noexcept {return Byte(ui8(byte.uch ^ other.byte.uch));  }
  Byte operator<<(const Byte& other) const noexcept {return Byte(ui8(byte.uch << other.byte.uch));  }
  Byte operator>>(const Byte& other) const noexcept {return Byte(ui8(byte.uch >> other.byte.uch));  }

  bit operator[](const ui8 index) const noexcept {
    switch (index) {
      case 0: return byte.bits.b0;
      case 1: return byte.bits.b1;
      case 2: return byte.bits.b2;
      case 3: return byte.bits.b3;
      case 4: return byte.bits.b4;
      case 5: return byte.bits.b5;
      case 6: return byte.bits.b6;
      case 7: return byte.bits.b7;
      default: return 0;
    }
  }

  bit set(const ui8 index, bit val) {
    switch (index) {
      case 0: return byte.bits.b0 = val;
      case 1: return byte.bits.b1 = val;
      case 2: return byte.bits.b2 = val;
      case 3: return byte.bits.b3 = val;
      case 4: return byte.bits.b4 = val;
      case 5: return byte.bits.b5 = val;
      case 6: return byte.bits.b6 = val;
      case 7: return byte.bits.b7 = val;
      default: return 0;
    }
  }

  bool operator==(const Byte& other) {return byte.uch == other.byte.uch;}
  bool operator!=(const Byte& other) {return byte.uch != other.byte.uch;}
  bool operator<(const Byte& other) {return byte.uch < other.byte.uch;}
  bool operator>(const Byte& other) {return byte.uch > other.byte.uch;}
  bool operator<=(const Byte& other) {return byte.uch <= other.byte.uch;}
  bool operator>=(const Byte& other) {return byte.uch >= other.byte.uch;}

};

#ifdef STDCPPOUTPUT

#include <iostream>
#include <iomanip>

std::ostream& operator << (std::ostream& os, const Byte& byte) {
  return os << std::right << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << ui16(byte.asUnsigned());
}


#endif


#endif // BYTE_H
