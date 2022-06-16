#ifndef DOUBLY_LINKED_LIST_HXX
#define DOUBLY_LINKED_LIST_HXX

#include "variable.hxx"

namespace binom {
namespace priv {

struct DoublyLinkedListImplementation::Node {
  Variable value;
  Node* next = nullptr;
  Node* prev = nullptr;
};

}

class priv::DoublyLinkedListImplementation::Iterator {
  Node* node;

  friend class binom::priv::DoublyLinkedListImplementation;
  friend class binom::DoublyLinkedList;
  Iterator(Node* node);

public:
  Iterator(const Iterator& other);
  Iterator(const Iterator&& other);
  Iterator(const ReverseIterator& other);
  Iterator(const ReverseIterator&& other);

  Iterator& operator++();
  Iterator operator++(int);
  Iterator& operator--();
  Iterator operator--(int);

  Variable operator*();
  Variable* operator->();

  bool operator==(const Iterator& other) const noexcept;
  bool operator==(const Iterator&& other) const noexcept;
  bool operator!=(const Iterator& other) const noexcept;
  bool operator!=(const Iterator&& other) const noexcept;
  bool operator==(const ReverseIterator& other) const noexcept;
  bool operator==(const ReverseIterator&& other) const noexcept;
  bool operator!=(const ReverseIterator& other) const noexcept;
  bool operator!=(const ReverseIterator&& other) const noexcept;
};

class DoublyLinkedList : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator BufferArray& () = delete;
  operator Array& () = delete;
  operator SinglyLinkedList& () = delete;
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
  operator const Map& () const = delete;

  const Number& toNumber() const = delete;
  const BitArray& toBitArray() const = delete;
  const BufferArray& toBufferArray() const = delete;
  const Array& toArray() const = delete;
  const SinglyLinkedList& toSinglyLinkedList() const = delete;
  const DoublyLinkedList& toDoublyLinkedList() const = delete;
  const Map& toMap() const = delete;

  priv::DoublyLinkedListImplementation*& getData() const noexcept;

  friend class Variable;
  DoublyLinkedList(priv::Link&& link);
public:
  typedef priv::DoublyLinkedListImplementation::Iterator Iterator;
  typedef priv::DoublyLinkedListImplementation::ReverseIterator ReverseIterator;

  DoublyLinkedList();
  DoublyLinkedList(const literals::dllist doubly_linked_list);
  DoublyLinkedList(const DoublyLinkedList& other) noexcept;
  DoublyLinkedList(const DoublyLinkedList&& other) noexcept;

  bool isEmpty() const;

  DoublyLinkedList move() noexcept;
  const DoublyLinkedList move() const noexcept;

  Variable pushBack(Variable variable);
  Variable pushFront(Variable variable);

  Variable insert(Iterator it, Variable variable);

  void popBack();
  void popFront();

  void remove(Iterator it);
  void clear();

  Iterator begin() const;
  static Iterator end();

  ReverseIterator rbegin() const;
  static ReverseIterator rend();
};

}

#endif // DOUBLY_LINKED_LIST_HXX
