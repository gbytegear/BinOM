#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include "variable.hxx"

namespace binom {

struct priv::SinglyLinkedListImplementation::Node {
  Variable value;
  Node* next = nullptr;
};

class priv::SinglyLinkedListImplementation::Iterator {
  mutable Node* prev = nullptr; //!< Required for insert
  mutable Node* node;

  friend class binom::priv::SinglyLinkedListImplementation;
  friend class binom::SinglyLinkedList;
  Iterator(Node* node, Node* prev = nullptr);

public:
  Iterator(const Iterator& other);
  Iterator(const Iterator&& other);

  Iterator& operator++();
  Iterator operator++(int);

  const Iterator& operator++() const;
  const Iterator operator++(int) const;

  Variable operator*();
  Variable* operator->();

  const Variable operator*() const;
  const Variable* operator->() const;

  bool operator==(const Iterator& other) const noexcept;
  bool operator==(const Iterator&& other) const noexcept;
  bool operator!=(const Iterator& other) const noexcept;
  bool operator!=(const Iterator&& other) const noexcept;
};

class SinglyLinkedList : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator BufferArray& () = delete;
  operator Array& () = delete;
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
  operator const DoublyLinkedList& () const = delete;
  operator const Map& () const = delete;

  const Number& toNumber() const = delete;
  const BitArray& toBitArray() const = delete;
  const BufferArray& toBufferArray() const = delete;
  const Array& toArray() const = delete;
  const SinglyLinkedList& toSinglyLinkedList() const = delete;
  const DoublyLinkedList& toDoublyLinkedList() const = delete;
  const Map& toMap() const = delete;

  priv::SinglyLinkedListImplementation*& getData() const noexcept;

  friend class Variable;
  SinglyLinkedList(priv::Link&& link);
public:
  typedef priv::SinglyLinkedListImplementation::Iterator Iterator;
  typedef const priv::SinglyLinkedListImplementation::Iterator ConstIterator;

  SinglyLinkedList();
  SinglyLinkedList(const literals::sllist singly_linked_list);
  SinglyLinkedList(const SinglyLinkedList& other) noexcept;
  SinglyLinkedList(const SinglyLinkedList&& other) noexcept;

  SinglyLinkedList move() noexcept;
  const SinglyLinkedList move() const noexcept;

  bool isEmpty() const;

  Variable pushBack(Variable var);
  Iterator pushBack(const literals::sllist value_list);

  Variable pushFront(Variable var);
  Iterator pushFront(const literals::sllist& value_list);

  Iterator insert(Iterator it, Variable var);

  void remove(Iterator it);
  void clear();

  Iterator begin();
  Iterator end();

  inline ConstIterator begin() const noexcept {return cbegin();}
  inline ConstIterator end() const noexcept {return cend();}

  ConstIterator cbegin() const;
  ConstIterator cend() const;

  SinglyLinkedList& operator=(const SinglyLinkedList& other);
  SinglyLinkedList& operator=(SinglyLinkedList&& other);

  SinglyLinkedList& changeLink(const SinglyLinkedList& other);
  SinglyLinkedList& changeLink(SinglyLinkedList&& other);
};

}

#endif // SINGLY_LINKED_LIST_H
