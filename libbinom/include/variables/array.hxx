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

  Number& toNumber() const = delete;
  BitArray& toBitArray() const = delete;
  BufferArray& toBufferArray() const = delete;
  Array& toArray() const = delete;
  SinglyLinkedList& toSinglyLinkedList() const = delete;
  DoublyLinkedList& toDoublyLinkedList() const = delete;
  Map& toMap() const = delete;

  priv::ArrayHeader*& getData() const noexcept;

  friend class Variable;
  Array(priv::Link&& link);
public:
  typedef Variable* Iterator;
  typedef std::reverse_iterator<Variable*> ReverseIterator;

  Array();
  Array(const literals::arr array);
  Array(const Array& other) noexcept;
  Array(const Array&& other) noexcept;

  Array getReference() noexcept;
  size_t getElementCount() const noexcept;
  size_t getCapacity() const noexcept;
  size_t getSize() const noexcept;

  Variable pushBack(Variable variable);
  Iterator pushBack(literals::arr variable_list);

  Variable pushFront(Variable variable);
  Iterator pushFront(literals::arr variable_list);

  Variable insert(size_t at, Variable variable);
  Iterator insert(size_t at, literals::arr variable_list);

  void popBack(size_t count = 1);
  void popFront(size_t count = 1);

  void remove(size_t at, size_t count = 1);
  void clear();

  Variable operator[](size_t index) noexcept;

  Variable operator+=(Variable variable);
  Variable operator+=(literals::arr variable_list);

  Iterator begin() const;
  Iterator end() const;

  ReverseIterator rbegin() const;
  ReverseIterator rend() const;


};

}

#endif // ARRAY_HXX
