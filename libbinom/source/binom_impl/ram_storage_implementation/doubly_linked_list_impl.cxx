#include "libbinom/include/binom_impl/ram_storage_implementation/doubly_linked_list_impl.hxx"
#include "libbinom/include/utils/util_functions.hxx"
#include "libbinom/include/variables/doubly_linked_list.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;

DoublyLinkedListImplementation::DoublyLinkedListImplementation(const literals::dllist& value_list) {pushBack(value_list);}
DoublyLinkedListImplementation::DoublyLinkedListImplementation(const DoublyLinkedListImplementation& other) {for(const auto& value : other) pushBack(value);}

DoublyLinkedListImplementation::~DoublyLinkedListImplementation() {clear();}

bool DoublyLinkedListImplementation::isEmpty() {return !first;}

bool DoublyLinkedListImplementation::isOwnIterator(Iterator it) const {
  /* WARNING:
   * O(1) without this chek, but can have troubles;
   * In current implementation way - checking complicaticity is O(n), but much secure;
   * Second way - add pointer at list in every list nodes for this checking, but that way have O(n) memory overhead;
   * TODO: Third way(best choice) - add pointer at list in every iterators for this checking,
   * but that way have O(1) memory overhead.
  */
  if(it.node == first || it.node == last) return true;
  for (++it;it.node;++it) if(it.node == last) return true;
  return false;
}

void DoublyLinkedListImplementation::clear() {
  auto it = begin(), _end = end();
  while(it != _end) {
    Node* node = it.node;
    ++it;
    delete node;
  }
  first = last = nullptr;
}

Variable DoublyLinkedListImplementation::pushBack(Variable var) {
  if(!last) {
    first = last = new Node{std::move(var), nullptr, nullptr};
  } else {
    last = last->next = new Node{std::move(var), nullptr, last};
  }
  return last->value.move();
}

DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::pushBack(const literals::dllist& value_list) {
  Iterator result(nullptr);

  auto it = value_list.begin();

  if(!last) {
    result.node = first = last = new Node{std::move(*it), nullptr, last};
  } else {
    result.node = last = last->next = new Node{std::move(*it), nullptr, last};
  }

  for(auto end = value_list.end(); it != end; ++it)
      last = last->next = new Node{std::move(*it), nullptr, last};

  return result;
}

Variable DoublyLinkedListImplementation::pushFront(Variable var) {
  if(!first) {
    first = last = new Node{std::move(var), nullptr, nullptr};
  } else {
    first = first->prev = new Node{std::move(var), first, nullptr};
  }
  return first->value.move();
}

DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::pushFront(const literals::dllist& value_list) {
  Node* last_first = first;
  Node** ptr_it = &first; // Pointer at first or Node::next
  Node* prev_node = nullptr;

  for(const auto& value : value_list) {
    prev_node = (*ptr_it) = new Node{std::move(value), nullptr, prev_node};
    ptr_it = &(*ptr_it)->next;
  }

  (*ptr_it)->next = last_first;
  last_first->prev = prev_node;

  return first;
}

DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::insert(Iterator it, Variable var) {
  if(!it.node) return it;
  elif(it.node == nullptr) {
    it.node = last = last->next = new Node{std::move(var), nullptr, last};
  } elif(it.node == first) {
    it.node = first = first->prev = new Node{std::move(var), first, nullptr};
  } else {
    it.node = it.node->prev->next = it.node->prev = new Node{std::move(var), it.node, it.node->prev};
  }
  return it;
}

void DoublyLinkedListImplementation::popBack() {
  if(!last) return;
  Node* prev_last = last;
  last = last->prev;
  if(first == prev_last) first = nullptr;
  delete prev_last;
}

void DoublyLinkedListImplementation::popFront() {
  if(!first) return;
  Node* prev_first = first;
  first = first->next;
  if(last == prev_first) last = nullptr;
  delete prev_first;
}

void DoublyLinkedListImplementation::remove(Iterator it) {
  if(it.node == first) return popFront();
  elif(it.node == last) return popBack();

  if(!isOwnIterator(it)) return;

  if(it.node->prev) it.node->prev->next = it.node->next;
  if(it.node->next) it.node->next->prev = it.node->prev;
  delete it.node;
}

DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::begin() const {return Iterator(first);}
DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::end() const {return Iterator(nullptr);}

DoublyLinkedListImplementation::ReverseIterator DoublyLinkedListImplementation::rbegin() const {return ReverseIterator(last);}
DoublyLinkedListImplementation::ReverseIterator DoublyLinkedListImplementation::rend() const {return ReverseIterator(nullptr);}
