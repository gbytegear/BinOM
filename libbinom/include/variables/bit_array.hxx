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

  priv::BitArrayHeader*& getData() const noexcept;

  friend class Variable;
  BitArray(priv::Link&& link);

public:
  typedef BitIterator Iterator;
  typedef BitValueRef ValueRef;
  typedef BitReverseIterator ReverseIterator;

  BitArray();
  BitArray(const literals::bitarr bit_array);
  BitArray(const BitArray& other) noexcept;
  BitArray(const BitArray&& other) noexcept;

  BitArray getReference() noexcept;
  size_t getElementCount() const noexcept;
  size_t getSize() const noexcept;
  size_t getCapacity() const noexcept;

  ValueRef operator[] (size_t index) const;

  ValueRef operator+=(bool value);
  Iterator operator+=(const literals::bitarr value_list);
  Iterator operator+=(BitArray&& other);

  ValueRef pushBack(bool value);
  Iterator pushBack(const literals::bitarr value_list);
  Iterator pushBack(BitArray&& other);

  ValueRef pushFront(bool value);
  Iterator pushFront(const literals::bitarr value_list);
  Iterator pushFront(BitArray&& other);

  ValueRef insert(size_t at, bool value);
  Iterator insert(size_t at, const literals::bitarr value_list);
  Iterator insert(size_t at, BitArray&& other);

  void popBack(size_t size);
  void popFront(size_t size);
  void remove(size_t at, size_t size);



  Iterator begin() const;
  Iterator end() const;

  ReverseIterator rbegin() const;
  ReverseIterator rend() const;

  BitArray& operator=(const BitArray& other);
  BitArray& operator=(BitArray&& other);

  BitArray& changeLink(const BitArray& other);
  BitArray& changeLink(BitArray&& other);
};

}

#endif // BIT_ARRAY_HXX
