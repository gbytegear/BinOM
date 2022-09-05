#ifndef MULTI_MAP_HXX
#define MULTI_MAP_HXX

#include "named_variable.hxx"

namespace binom {

class MultiMap : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
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
  operator const BitArray& () const = delete;
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

  Variable& operator=(const Variable& other) = delete;
  Variable& operator=(Variable&& other) = delete;

  Variable& changeLink(const Variable& other) = delete;
  Variable& changeLink(Variable&& other) = delete;

  priv::MultiMapImplementation* getData() const;

  friend class Variable;
  MultiMap(priv::Link&& link);
public:
  using NamedVariable = binom::priv::MultiMapImplementation::NamedVariable;
  typedef priv::MultiMapImplementation::Iterator             Iterator;
  typedef priv::MultiMapImplementation::ReverseIterator      ReverseIterator;
  typedef priv::MultiMapImplementation::ConstIterator        ConstIterator;
  typedef priv::MultiMapImplementation::ConstReverseIterator ConstReverseIterator;

  MultiMap();
  MultiMap(literals::multimap multimap_elements);
  MultiMap(const MultiMap& other);
  MultiMap(MultiMap&& other);

  MultiMap move() noexcept;
  const MultiMap move() const noexcept;

  void clear();
  bool isEmpty() const noexcept;
  size_t getElementCount() const noexcept;
  bool contains(KeyValue key) const noexcept;

  err::ProgressReport<NamedVariable> insert(KeyValue key, Variable variable, NewNodePosition position = NewNodePosition::back);
  Error remove(Iterator it);
  Error remove(ReverseIterator it);
  Error removeAll(KeyValue key);
  err::ProgressReport<NamedVariable> rename(Iterator it, KeyValue new_key);

  Iterator find(KeyValue key);
  ReverseIterator rfind(KeyValue key);
  Iterator findLast(KeyValue key);
  ReverseIterator rfindLast(KeyValue key);
  ConstIterator find(KeyValue key) const;
  ConstReverseIterator rfind(KeyValue key) const;
  ConstIterator findLast(KeyValue key) const;
  ConstReverseIterator rfindLast(KeyValue key) const;

  Iterator operator[](KeyValue key) noexcept;
  ConstIterator operator[](KeyValue key) const noexcept;

  Iterator begin();
  Iterator end();

  ReverseIterator rbegin();
  ReverseIterator rend();

  ConstIterator begin() const noexcept;
  ConstIterator end() const noexcept;

  ConstReverseIterator rbegin() const noexcept;
  ConstReverseIterator rend() const noexcept;

  ConstIterator cbegin() const noexcept;
  ConstIterator cend() const noexcept;

  ConstReverseIterator crbegin() const noexcept;
  ConstReverseIterator crend() const noexcept;

  MultiMap& operator=(const MultiMap& other);
  MultiMap& operator=(MultiMap&& other);

  MultiMap& changeLink(const MultiMap& other);
  MultiMap& changeLink(MultiMap&& other);
};

}

#endif // MULTI_MAP_HXX
