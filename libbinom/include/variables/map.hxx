#ifndef MAP_HXX
#define MAP_HXX

#include "variable.hxx"

namespace binom {

class Map : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator BufferArray& () = delete;
  operator Array& () = delete;
  operator SinglyLinkedList& () = delete;
  operator DoublyLinkedList& () = delete;

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
  operator const Array& () const = delete;
  operator const SinglyLinkedList& () const = delete;
  operator const DoublyLinkedList& () const = delete;

  Variable& operator=(const Variable& other) = delete;
  Variable& operator=(Variable&& other) = delete;

  Variable& changeLink(const Variable& other) = delete;
  Variable& changeLink(Variable&& other) = delete;

  priv::MapImplementation* getData() const;

  friend class Variable;
  Map(priv::Link&& link);

public:
  typedef priv::MapImplementation::Iterator             Iterator;
  typedef priv::MapImplementation::ReverseIterator      ReverseIterator;
  typedef priv::MapImplementation::ConstIterator        ConstIterator;
  typedef priv::MapImplementation::ConstReverseIterator ConstReverseIterator;

  Map();
  Map(literals::map element_list);
  Map(const Map& other);
  Map(Map&& other);

  Map move() noexcept;
  const Map move() const noexcept;

  bool isEmpty() const noexcept;
  size_t getElementCount() const noexcept;
  bool contains(KeyValue value) const noexcept;

  void clear();
  err::ProgressReport<NamedVariable> insert(KeyValue key, Variable variable);
  err::Error remove(KeyValue key);
  err::ProgressReport<NamedVariable> rename(KeyValue old_key, KeyValue new_key);

  Variable getVariable(KeyValue key);
  Variable operator[] (KeyValue key);
  const Variable getVariable(KeyValue key) const;
  const Variable operator[] (KeyValue key) const;

  Iterator begin();
  Iterator end();

  ReverseIterator rbegin();
  ReverseIterator rend();

  ConstIterator begin() const noexcept;
  ConstIterator end() const noexcept;

  ConstReverseIterator rbegin() const noexcept;
  ConstReverseIterator rend() const noexcept;

  ConstIterator cbegin() const;
  ConstIterator cend() const;

  ConstReverseIterator crbegin() const;
  ConstReverseIterator crend() const;

  Map& operator=(const Map& other);
  Map& operator=(Map&& other);

  Map& changeLink(const Map& other);
  Map& changeLink(Map&& other);

};

}

#endif // MAP_HXX
