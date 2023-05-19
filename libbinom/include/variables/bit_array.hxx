#ifndef BIT_ARRAY_HXX
#define BIT_ARRAY_HXX

#include "variable.hxx"
#include "../binom_impl/ram_storage_implementation/bit_array_impl.hxx"

namespace binom {

class BitArray : public Variable {
  operator Number& () = delete;
  operator BufferArray& () = delete;
  operator Array& () = delete;
  operator List& () = delete;
  operator Map& () = delete;
  operator MultiMap& () = delete;
  operator Table& () = delete;

  Number& toNumber() = delete;
  BitArray& toBitArray() = delete;
  BufferArray& toBufferArray() = delete;
  Array& toArray() = delete;
  List& toList() = delete;
  Map& toMap() = delete;
  MultiMap& toMultiMap() = delete;
  Table& toTable() = delete;

  operator const Number& () const = delete;
  operator const BufferArray& () const = delete;
  operator const Array& () const = delete;
  operator const List& () const = delete;
  operator const Map& () const = delete;
  operator const MultiMap& () const = delete;
  operator const Table& () const = delete;

  const Number& toNumber() const = delete;
  const BitArray& toBitArray() const = delete;
  const BufferArray& toBufferArray() const = delete;
  const Array& toArray() const = delete;
  const List& toList() const = delete;
  const Map& toMap() const = delete;
  const MultiMap& toMultiMap() const = delete;
  const Table& toTable() const = delete;

  Variable& operator=(const Variable& other) = delete;
  Variable& operator=(Variable&& other) = delete;

  Variable& changeLink(const Variable& other) = delete;
  Variable& changeLink(Variable&& other) = delete;

  priv::BitArrayImplementation*& getData() const noexcept;
  const void* getDataPointer() const noexcept;

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

  BitArray move() noexcept;
  const BitArray move() const noexcept;

  size_t getElementCount() const noexcept;
  size_t getSize() const noexcept;
  size_t getCapacity() const noexcept;

  ValueRef operator[] (size_t index);
  const ValueRef operator[] (size_t index) const;

  ValueRef operator+=(bool value);
  Iterator operator+=(const literals::bitarr value_list);
  Iterator operator+=(const BitArray other);

  ValueRef pushBack(bool value);
  Iterator pushBack(const literals::bitarr value_list);

  ValueRef pushFront(bool value);
  Iterator pushFront(const literals::bitarr value_list);

  ValueRef insert(size_t at, bool value);
  Iterator insert(size_t at, const literals::bitarr value_list);

  void popBack(size_t size = 1);
  void popFront(size_t size = 1);
  void remove(size_t at, size_t size = 1);
  void clear();

  Iterator begin();
  Iterator end();

  ReverseIterator rbegin();
  ReverseIterator rend();

  inline ConstIterator begin() const noexcept {return cbegin();}
  inline ConstIterator end() const noexcept {return cend();}

  inline ConstReverseIterator rbegin() const noexcept {return crbegin();}
  inline ConstReverseIterator rend() const noexcept {return crend();}

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
