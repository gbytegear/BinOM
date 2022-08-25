#ifndef SINGLY_LINKED_LIST_IMPL_HXX
#define SINGLY_LINKED_LIST_IMPL_HXX

#include "../../variables/variable.hxx"

namespace binom::priv {

class SinglyLinkedListImplementation {
  struct Node;
  size_t size = 0;
  Node* first = nullptr;
  Node* last = nullptr;
public:
  class Iterator;
  SinglyLinkedListImplementation(const literals::sllist& value_list);
  SinglyLinkedListImplementation(const SinglyLinkedListImplementation& other);
  ~SinglyLinkedListImplementation();

  bool isEmpty();
  bool isOwnIterator(Iterator it) const;

  void clear();

  Variable pushBack(Variable var);
  Iterator pushBack(const literals::sllist& value_list);

  Variable pushFront(Variable var);
  Iterator pushFront(const literals::sllist& value_list);

  Iterator insert(Iterator it, Variable var);
  void remove(Iterator it);

  Iterator begin() const;
  Iterator end() const;
};

}

#endif // SINGLY_LINKED_LIST_IMPL_HXX
