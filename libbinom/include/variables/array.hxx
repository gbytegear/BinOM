#ifndef ARRAY_HXX
#define ARRAY_HXX

#include "variable.hxx"
#include "../utils/reverse_iterator.hxx"
#include "../binom_impl/ram_storage_implementation/array_impl.hxx"

namespace binom {

class Array : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator BufferArray& () = delete;
  operator List& () = delete;
  operator Map& () = delete;
  operator MultiMap& () = delete;

  Number& toNumber() = delete;
  BitArray& toBitArray() = delete;
  BufferArray& toBufferArray() = delete;
  Array& toArray() = delete;
  List& toList() = delete;
  Map& toMap() = delete;
  MultiMap& toMultiMap() = delete;

  operator const Number& () const = delete;
  operator const BitArray& () const = delete;
  operator const BufferArray& () const = delete;
  operator const List& () const = delete;
  operator const Map& () const = delete;
  operator const MultiMap& () const = delete;

  const Number& toNumber() const = delete;
  const BitArray& toBitArray() const = delete;
  const BufferArray& toBufferArray() const = delete;
  const Array& toArray() const = delete;
  const List& toList() const = delete;
  const Map& toMap() const = delete;
  const MultiMap& toMultiMap() const = delete;

  Variable& operator=(const Variable& other) = delete;
  Variable& operator=(Variable&& other) = delete;

  Variable& changeLink(const Variable& other) = delete;
  Variable& changeLink(Variable&& other) = delete;

  priv::ArrayImplementation*& getData() const noexcept;

  friend class Variable;
  Array(priv::Link&& link);

public:
  typedef Variable*                                           Iterator;
  typedef const Variable*                                     ConstIterator;
  typedef reverse_iterator::ReverseIterator<Variable*>        ReverseIterator;
  typedef const reverse_iterator::ReverseIterator<Variable*>  ConstReverseIterator;

  Array();
  Array(const literals::arr array);
  Array(const Array& other) noexcept;
  Array(const Array&& other) noexcept;

  Array move() noexcept;
  const Array move() const noexcept;

  size_t getElementCount() const noexcept;
  size_t getCapacity() const noexcept;
  size_t getSize() const noexcept;

  Variable pushBack(Variable variable);
  Iterator pushBack(const literals::arr variable_list);

  Variable pushFront(Variable variable);
  Iterator pushFront(const literals::arr variable_list);

  Variable insert(size_t at, Variable variable);
  Iterator insert(size_t at, const literals::arr variable_list);

  void popBack(size_t count = 1);
  void popFront(size_t count = 1);

  void remove(size_t at, size_t count = 1);
  void clear();

  Variable operator[](size_t index) noexcept;
  const Variable operator[](size_t index) const noexcept;

  Array& operator+=(Variable variable);
  Array& operator+=(literals::arr variable_list);
  Array& operator+=(const Array variable_array);

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

  Array& operator=(const Array& other);
  Array& operator=(Array&& other);

  Array& changeLink(const Array& other);
  Array& changeLink(Array&& other);
};

}

#endif // ARRAY_HXX
