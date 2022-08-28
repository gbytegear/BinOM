#ifndef DOUBLY_LINKED_LIST_IMPL_HXX
#define DOUBLY_LINKED_LIST_IMPL_HXX

#include "../../variables/variable.hxx"

namespace binom::priv {

class DoublyLinkedListImplementation {
  struct Node;
  size_t size = 0;
  Node* first = nullptr;
  Node* last = nullptr;


public:
  class Iterator;
  typedef reverse_iterator::ReverseIterator<Iterator> ReverseIterator;
  DoublyLinkedListImplementation(const literals::dllist& value_list);
  DoublyLinkedListImplementation(const DoublyLinkedListImplementation& other);
  ~DoublyLinkedListImplementation();

  bool isEmpty();
  size_t getElementCount() const noexcept;
  bool isOwnIterator(Iterator it) const;

  void clear();

  Variable pushBack(Variable var);
  Iterator pushBack(const literals::dllist& value_list);

  Variable pushFront(Variable var);
  Iterator pushFront(const literals::dllist& value_list);

  Iterator insert(Iterator it, Variable var);

  void popBack();
  void popFront();
  void remove(Iterator it);

  Iterator begin() const;
  Iterator end() const;

  ReverseIterator rbegin() const;
  ReverseIterator rend() const;
};

}

#endif // DOUBLY_LINKED_LIST_IMPL_HXX
