#ifndef DOUBLY_LINKED_LIST_HXX
#define DOUBLY_LINKED_LIST_HXX

#include "variable.hxx"

namespace binom {
namespace priv {

struct DoublyLinkedListHeader::Node {
  Variable value;
  Node* next = nullptr;
  Node* prev = nullptr;
};

}

class priv::DoublyLinkedListHeader::Iterator {
  Node* node;

  friend class binom::priv::DoublyLinkedListHeader;
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

class priv::DoublyLinkedListHeader::ReverseIterator {
  Node* node;

  friend class binom::priv::DoublyLinkedListHeader;
  friend class binom::DoublyLinkedList;
  ReverseIterator(Node* node);

public:
  ReverseIterator(const ReverseIterator& other);
  ReverseIterator(const ReverseIterator&& other);
  ReverseIterator(const Iterator& other);
  ReverseIterator(const Iterator&& other);

  ReverseIterator& operator++();
  ReverseIterator operator++(int);
  ReverseIterator& operator--();
  ReverseIterator operator--(int);

  Variable operator*();
  Variable* operator->();

  bool operator==(const ReverseIterator& other) const noexcept;
  bool operator==(const ReverseIterator&& other) const noexcept;
  bool operator!=(const ReverseIterator& other) const noexcept;
  bool operator!=(const ReverseIterator&& other) const noexcept;
  bool operator==(const Iterator& other) const noexcept;
  bool operator==(const Iterator&& other) const noexcept;
  bool operator!=(const Iterator& other) const noexcept;
  bool operator!=(const Iterator&& other) const noexcept;
};

class DoublyLinkedList : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator BufferArray& () = delete;
  operator Array& () = delete;
  operator SinglyLinkedList& () = delete;
  operator Map& () = delete;

  Number& toNumber() const = delete;
  BitArray& toBitArray() const = delete;
  BufferArray& toBufferArray() const = delete;
  Array& toArray() const = delete;
  SinglyLinkedList& toSinglyLinkedList() const = delete;
  DoublyLinkedList& toDoublyLinkedList() const = delete;
  Map& toMap() const = delete;

  priv::DoublyLinkedListHeader*& getData() const noexcept;

  friend class Variable;
  DoublyLinkedList(priv::Link&& link);
public:
  typedef priv::DoublyLinkedListHeader::Iterator Iterator;
  typedef priv::DoublyLinkedListHeader::ReverseIterator ReverseIterator;

  DoublyLinkedList();
  DoublyLinkedList(const literals::dllist doubly_linked_list);
  DoublyLinkedList(const DoublyLinkedList& other) noexcept;
  DoublyLinkedList(const DoublyLinkedList&& other) noexcept;

  DoublyLinkedList getReference() noexcept;

  Variable pushBack(Variable variable);
  Variable pushFront(Variable variable);

  Variable insert(Iterator it, Variable variable);

  void popBack();
  void popFront();

  void remove(Iterator it);
//  void clear();

  Iterator begin() const;
  static Iterator end();

  ReverseIterator rbegin() const;
  static ReverseIterator rend();
};

}

#endif // DOUBLY_LINKED_LIST_HXX
