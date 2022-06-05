#ifndef ARRAY_HXX
#define ARRAY_HXX

#include "variable.hxx"

namespace binom {

class Array : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator BufferArray& () = delete;
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
  operator const BitArray& () const = delete;
  operator const BufferArray& () const = delete;
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

  priv::ArrayHeader*& getData() const noexcept;

  friend class Variable;
  Array(priv::Link&& link);

public:
  typedef Variable* Iterator;
  typedef reverse_iterator::ReverseIterator<Variable*> ReverseIterator;

  Array();
  Array(const literals::arr array);
  Array(const Array& other) noexcept;
  Array(const Array&& other) noexcept;

  Array getReference() noexcept;
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

  Iterator begin() const;
  Iterator end() const;

  ReverseIterator rbegin() const;
  ReverseIterator rend() const;
};

}

#endif // ARRAY_HXX
