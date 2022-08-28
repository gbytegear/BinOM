#include "libbinom/include/binom_impl/ram_storage_implementation/singly_linked_list_impl.hxx"
#include "libbinom/include/utils/util_functions.hxx"
#include "libbinom/include/variables/singly_linked_list.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;

SinglyLinkedListImplementation::SinglyLinkedListImplementation(const sllist& value_list) {pushBack(value_list);}

SinglyLinkedListImplementation::SinglyLinkedListImplementation(const SinglyLinkedListImplementation& other) {for(const auto& value : other) pushBack(value);}

SinglyLinkedListImplementation::~SinglyLinkedListImplementation() {clear();}

bool SinglyLinkedListImplementation::isEmpty() {return !first;}

size_t SinglyLinkedListImplementation::getElementCount() const noexcept {return size;}

bool SinglyLinkedListImplementation::isOwnIterator(Iterator it) const {
  if(it.node == first || it.node == last ||
     it.prev == first || it.prev == last) return true;
  for (++it;it.node;++it) if(it.node == last) return true;
  return false;
}

void SinglyLinkedListImplementation::clear() {
  auto it = begin(), _end = end();
  while(it != _end) {
    Node* node = it.node;
    ++it;
    delete node;
  }
  first = last = nullptr;
}

Variable SinglyLinkedListImplementation::pushBack(Variable var) {
  if(!last) {
    first = last = new Node{std::move(var), nullptr};
  } else {
    last = last->next = new Node{std::move(var), nullptr};
  }
  return last->value.move();
}

SinglyLinkedListImplementation::Iterator SinglyLinkedListImplementation::pushBack(const literals::sllist& value_list) {
  Iterator result(nullptr, nullptr);

  auto it = value_list.begin();

  if(!last) {
    result.node = first = last = new Node{std::move(*it), nullptr};
  } else {
    result.prev = last;
    result.node = last = last->next = new Node{std::move(*it), nullptr};
  }

  for(auto end = value_list.end(); it != end; ++it)
      last = last->next = new Node{std::move(*it), nullptr};

  return result;
}

Variable SinglyLinkedListImplementation::pushFront(Variable var) {
  if(!first) {
    first = last = new Node{std::move(var), nullptr};
  } else {
    first = new Node{std::move(var), first};
  }
  return first->value.move();
}

SinglyLinkedListImplementation::Iterator SinglyLinkedListImplementation::pushFront(const literals::sllist& value_list) {
  Node* last_first = first;
  Node** ptr_it = &first; // Pointer at first or Node::next

  for(const auto& value : value_list) {
    (*ptr_it) = new Node{std::move(value), nullptr};
    ptr_it = &(*ptr_it)->next;
  }

  (*ptr_it)->next = last_first;

  return first;
}

SinglyLinkedListImplementation::Iterator SinglyLinkedListImplementation::insert(Iterator it, Variable var) {
  if(!it.node && !it.prev) return it;
  elif(!it.node && it.prev == last) {
    last = it.prev = it.prev->next = new Node{std::move(var), it.node};
  } elif(it.prev) {
    it.prev->next = new Node{std::move(var), it.node};
    it.node = it.prev->next;
  } elif (first == it.node) {
    first = new Node{std::move(var), it.node};
    it.node = first;
  }
  return it;
}

void SinglyLinkedListImplementation::remove(Iterator it) {
  if(!it.node) return;
  Node* removable_node = it.node;
  delete removable_node;
}

SinglyLinkedListImplementation::Iterator SinglyLinkedListImplementation::begin() const {return Iterator(first);}
SinglyLinkedListImplementation::Iterator SinglyLinkedListImplementation::end() const {return Iterator(nullptr, last);}
