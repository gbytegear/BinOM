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
  mutable Node* node;

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

  const Iterator& operator++() const;
  const Iterator operator++(int) const;
  const Iterator& operator--() const;
  const Iterator operator--(int) const;

  Variable operator*();
  Variable* operator->();

  const Variable operator*() const;
  const Variable* operator->() const;

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

  Variable& operator=(const Variable& other) = delete;
  Variable& operator=(Variable&& other) = delete;

  Variable& changeLink(const Variable& other) = delete;
  Variable& changeLink(Variable&& other) = delete;

  priv::DoublyLinkedListImplementation*& getData() const noexcept;

  friend class Variable;
  DoublyLinkedList(priv::Link&& link);
public:
  typedef priv::DoublyLinkedListImplementation::Iterator Iterator;
  typedef priv::DoublyLinkedListImplementation::ReverseIterator ReverseIterator;
  typedef const priv::DoublyLinkedListImplementation::Iterator ConstIterator;
  typedef const priv::DoublyLinkedListImplementation::ReverseIterator ConstReverseIterator;

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
};

}

#endif // DOUBLY_LINKED_LIST_HXX
