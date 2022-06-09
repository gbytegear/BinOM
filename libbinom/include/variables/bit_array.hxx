#ifndef BIT_ARRAY_HXX
#define BIT_ARRAY_HXX

#include "variable.hxx"

namespace binom {

class BitArray : public Variable {
  operator Number& () = delete;
  operator BufferArray& () = delete;
  operator Array& () = delete;
  operator SinglyLinkedList& () = delete;
  operator DoublyLinkedList& () = delete;
  operator Map& () = delete;

  Number& toNumber() = delete;
  BitArray& toBitArray() = delete;
  BufferArray& toBufferArray() = delete;
  Array& toArray() = delete;
  SinglyLinkedList& toSinglyLinkedList() = delete;
  DoublyLinkedList& toDoublyLinkedList() = delete;
  Map& toMap() = delete;

  operator const Number& () const = delete;
  operator const BufferArray& () const = delete;
  operator const Array& () const = delete;
  operator const SinglyLinkedList& () const = delete;
  operator const DoublyLinkedList& () const = delete;
  operator const Map& () const = delete;

  const Number& toNumber() const = delete;
  const BitArray& toBitArray() const = delete;
  const BufferArray& toBufferArray() const = delete;
  const Array& toArray() const = delete;
  const SinglyLinkedList& toSinglyLinkedList() const = delete;
  const DoublyLinkedList& toDoublyLinkedList() const = delete;
  const Map& toMap() const = delete;


  priv::BitArrayHeader*& getData() const noexcept;

  BitArray(priv::Link&& link);

  friend class binom::Variable;
  friend class binom::KeyValue;
public:
  typedef BitIterator Iterator;
  typedef BitValueRef ValueRef;
  typedef BitReverseIterator ReverseIterator;
  typedef BitConstIterator ConstIterator;
  typedef BitConstValueRef ConstValueRef;
  typedef BitConstReverseIterator ConstReverseIterator;

  BitArray();
  BitArray(const literals::bitarr bit_array);
  BitArray(const BitArray& other) noexcept;
  BitArray(const BitArray&& other) noexcept;

  BitArray getReference() noexcept;
  const BitArray getReference() const noexcept;

  size_t getElementCount() const noexcept;
  size_t getSize() const noexcept;
  size_t getCapacity() const noexcept;

  ValueRef operator[] (size_t index) const;

  ValueRef operator+=(bool value);
  Iterator operator+=(const literals::bitarr value_list);
  Iterator operator+=(const BitArray other);

  ValueRef pushBack(bool value);
  Iterator pushBack(const literals::bitarr value_list);

  ValueRef pushFront(bool value);
  Iterator pushFront(const literals::bitarr value_list);

  ValueRef insert(size_t at, bool value);
  Iterator insert(size_t at, const literals::bitarr value_list);

  void popBack(size_t size);
  void popFront(size_t size);
  void remove(size_t at, size_t size);

  Iterator begin() const;
  Iterator end() const;

  ReverseIterator rbegin() const;
  ReverseIterator rend() const;

  ConstIterator cbegin() const;
  ConstIterator cend() const;

  ConstReverseIterator crbegin() const;
  ConstReverseIterator crend() const;

  BitArray& operator=(const BitArray& other);
  BitArray& operator=(BitArray&& other);

  BitArray& changeLink(const BitArray& other);
  BitArray& changeLink(BitArray&& other);
};

}

#endif // BIT_ARRAY_HXX
