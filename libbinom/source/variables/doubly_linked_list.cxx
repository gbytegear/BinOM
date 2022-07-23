#include "libbinom/include/variables/doubly_linked_list.hxx"

using namespace binom;
using namespace binom::priv;

DoublyLinkedListImplementation::Iterator::Iterator(Node* node) : node(node) {}
DoublyLinkedListImplementation::Iterator::Iterator(const Iterator& other) : node(other.node) {}
DoublyLinkedListImplementation::Iterator::Iterator(const Iterator&& other) : node(other.node) {}
DoublyLinkedListImplementation::Iterator::Iterator(const ReverseIterator& other) : node(other.getIterator().node) {}
DoublyLinkedListImplementation::Iterator::Iterator(const ReverseIterator&& other) : node(other.getIterator().node) {}

binom::priv::DoublyLinkedListImplementation::Iterator& DoublyLinkedListImplementation::Iterator::operator++() {if(node) node = node->next; return self;}
binom::priv::DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::Iterator::operator++(int) {
  auto tmp = self;
  if(node) node = node->next;
  return tmp;
}

binom::priv::DoublyLinkedListImplementation::Iterator& DoublyLinkedListImplementation::Iterator::operator--() {if(node) node = node->prev; return self;}
binom::priv::DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::Iterator::operator--(int) {
  auto tmp = self;
  if(node) node = node->prev;
  return tmp;
}

const binom::priv::DoublyLinkedListImplementation::Iterator& DoublyLinkedListImplementation::Iterator::operator++() const {if(node) node = node->next; return self;}
const binom::priv::DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::Iterator::operator++(int) const {
  auto tmp = self;
  if(node) node = node->next;
  return tmp;
}

const binom::priv::DoublyLinkedListImplementation::Iterator& DoublyLinkedListImplementation::Iterator::operator--() const {if(node) node = node->prev; return self;}
const binom::priv::DoublyLinkedListImplementation::Iterator DoublyLinkedListImplementation::Iterator::operator--(int) const {
  auto tmp = self;
  if(node) node = node->prev;
  return tmp;
}

Variable DoublyLinkedListImplementation::Iterator::operator*() {return node->value.move();}
Variable* DoublyLinkedListImplementation::Iterator::operator->() {return &node->value;}

const Variable DoublyLinkedListImplementation::Iterator::operator*() const {return node->value.move();}
const Variable* DoublyLinkedListImplementation::Iterator::operator->() const {return &node->value;}

bool DoublyLinkedListImplementation::Iterator::operator==(const Iterator& other) const noexcept {return node == other.node;}
bool DoublyLinkedListImplementation::Iterator::operator==(const Iterator&& other) const noexcept {return node == other.node;}
bool DoublyLinkedListImplementation::Iterator::operator!=(const Iterator& other) const noexcept {return node != other.node;}
bool DoublyLinkedListImplementation::Iterator::operator!=(const Iterator&& other) const noexcept {return node != other.node;}
bool DoublyLinkedListImplementation::Iterator::operator==(const ReverseIterator& other) const noexcept {return node == other.getIterator().node;}
bool DoublyLinkedListImplementation::Iterator::operator==(const ReverseIterator&& other) const noexcept {return node == other.getIterator().node;}
bool DoublyLinkedListImplementation::Iterator::operator!=(const ReverseIterator& other) const noexcept {return node != other.getIterator().node;}
bool DoublyLinkedListImplementation::Iterator::operator!=(const ReverseIterator&& other) const noexcept {return node != other.getIterator().node;}

DoublyLinkedListImplementation*& DoublyLinkedList::getData() const noexcept {return resource_link->data.doubly_linked_list_implementation;}

DoublyLinkedList::DoublyLinkedList(priv::Link&& link) : Variable(std::move(link)) {}

DoublyLinkedList::DoublyLinkedList() : Variable(literals::dllist{}) {}
DoublyLinkedList::DoublyLinkedList(const literals::dllist doubly_linked_list) : Variable(doubly_linked_list) {}
DoublyLinkedList::DoublyLinkedList(const DoublyLinkedList& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
DoublyLinkedList::DoublyLinkedList(const DoublyLinkedList&& other) noexcept : Variable(dynamic_cast<const Variable&&>(other)) {}

bool DoublyLinkedList::isEmpty() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->isEmpty();
  else return true;
}

DoublyLinkedList DoublyLinkedList::move() noexcept {return Link(resource_link);}
const DoublyLinkedList DoublyLinkedList::move() const noexcept {return Link(resource_link);}

Variable DoublyLinkedList::pushBack(Variable variable) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->pushBack(std::move(variable));
  else return nullptr;
}

Variable DoublyLinkedList::pushFront(Variable variable) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->pushFront(std::move(variable));
  else return nullptr;
}

Variable DoublyLinkedList::insert(Iterator it, Variable variable) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return *getData()->insert(it, std::move(variable));
  else return nullptr;
}

void DoublyLinkedList::popBack() {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->popBack();
  else return;
}

void DoublyLinkedList::popFront() {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->popFront();
  else return;
}

void DoublyLinkedList::remove(Iterator it) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->remove(it);
  else return;
}

void DoublyLinkedList::clear() {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->clear();
  else return;
}

DoublyLinkedList::Iterator DoublyLinkedList::begin() {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->begin();
  else return Iterator(nullptr);
}

DoublyLinkedList::Iterator DoublyLinkedList::end() {return Iterator(nullptr);}

DoublyLinkedList::ReverseIterator DoublyLinkedList::rbegin() {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->rbegin();
  else return ReverseIterator(nullptr);
}

DoublyLinkedList::ReverseIterator DoublyLinkedList::rend() {return ReverseIterator(nullptr);}

DoublyLinkedList::ConstIterator DoublyLinkedList::cbegin() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->begin();
  else return Iterator(nullptr);
}

DoublyLinkedList::ConstIterator DoublyLinkedList::cend() const {return Iterator(nullptr);}

DoublyLinkedList::ConstReverseIterator DoublyLinkedList::crbegin() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->rbegin();
  else return ReverseIterator(nullptr);
}

DoublyLinkedList::ConstReverseIterator DoublyLinkedList::crend() const {return ReverseIterator(nullptr);}
