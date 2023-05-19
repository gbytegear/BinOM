#ifndef BITS_HXX
#define BITS_HXX

#include "types.hxx"
#include "../utils/pseudo_pointer.hxx"

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
  ValueRef(const Bits* bits, ui8 index);
  ValueRef() = default;
public:
  ValueRef(const ValueRef& other);
  ValueRef(ValueRef&& other);

  bool isNull() const noexcept;

  ValueRef& operator=(ValueRef value) noexcept;

  operator bool() const;

  ValueRef& operator=(bool value) noexcept;
};


class Bits::Iterator {
  friend struct Bits;
  mutable Bits* bits = nullptr;
  mutable ui8 index = 0;
  Iterator() = default;
  Iterator(const Bits* bits, ui8 index);
public:
  Iterator(const Iterator& other);
  Iterator(Iterator&& other);

  bool isNull() const noexcept;
  operator bool() const noexcept;

  Iterator& operator=(Iterator value) noexcept;
  bool operator==(Iterator other) noexcept;
  bool operator!=(Iterator other) noexcept;

  Iterator& operator++() noexcept;
  Iterator& operator--() noexcept;

  const Iterator& operator++() const noexcept;
  const Iterator& operator--() const noexcept;

  const Iterator operator++(int) const noexcept;
  const Iterator operator--(int) const noexcept;

  Iterator operator++(int) noexcept;
  Iterator operator--(int) noexcept;

  Iterator& operator+=(std::size_t shift) noexcept;
  Iterator& operator-=(std::size_t shift) noexcept;

  Iterator operator+(std::size_t shift) const noexcept;
  Iterator operator-(std::size_t shift) const noexcept;

  operator ValueRef() noexcept;
  operator const ValueRef() const noexcept;
  ValueRef operator*() noexcept;
  const ValueRef operator*() const noexcept;
  pseudo_ptr::PseudoPointer<ValueRef> operator->() noexcept;
  pseudo_ptr::PseudoPointer<const ValueRef> operator->() const noexcept;
};


class Bits::ReverseIterator {
  friend struct Bits;
  mutable Bits* bits = nullptr;
  mutable ui8 index = 0;
  ReverseIterator() = default;
  ReverseIterator(const Bits* bits, ui8 index);
public:
  ReverseIterator(const ReverseIterator& other);
  ReverseIterator(ReverseIterator&& other);

  bool isNull() const noexcept;

  ReverseIterator& operator=(ReverseIterator value) noexcept;
  bool operator==(ReverseIterator other) const noexcept;
  bool operator!=(ReverseIterator other) const noexcept;

  ReverseIterator& operator++() noexcept;
  ReverseIterator& operator--() noexcept;

  const ReverseIterator& operator++() const noexcept;
  const ReverseIterator& operator--() const noexcept;

  ReverseIterator operator++(int) noexcept;
  ReverseIterator operator--(int) noexcept;

  const ReverseIterator operator++(int) const noexcept;
  const ReverseIterator operator--(int) const noexcept;

  operator ValueRef() noexcept;
  ValueRef operator*() noexcept;
  operator const ValueRef() const noexcept;
  const ValueRef operator*() const noexcept;

};





}

namespace binom {

typedef priv::Bits::Iterator BitIterator;
typedef priv::Bits::ReverseIterator BitReverseIterator;
typedef priv::Bits::ValueRef BitValueRef;

typedef const priv::Bits::Iterator BitConstIterator;
typedef const priv::Bits::ReverseIterator BitConstReverseIterator;
typedef const priv::Bits::ValueRef BitConstValueRef;

}

#endif // BITS_HXX
