#include "libbinom/include/variables/singly_linked_list.hxx"

using namespace binom;
using namespace binom::priv;

inline SinglyLinkedListImplementation*& SinglyLinkedList::getData() const noexcept {return resource_link->data.single_linked_list_implementation;}

SinglyLinkedList::SinglyLinkedList(priv::Link&& link) : Variable(std::move(link)) {}

SinglyLinkedList::SinglyLinkedList() : Variable(literals::sllist{}) {}
SinglyLinkedList::SinglyLinkedList(const literals::sllist singly_linked_list) : Variable(singly_linked_list) {}
SinglyLinkedList::SinglyLinkedList(const SinglyLinkedList& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
SinglyLinkedList::SinglyLinkedList(const SinglyLinkedList&& other) noexcept : Variable(dynamic_cast<const Variable&&>(other)) {}

SinglyLinkedList SinglyLinkedList::move() noexcept {return Link(resource_link);}
const SinglyLinkedList SinglyLinkedList::move() const noexcept {return Link(resource_link);}

bool SinglyLinkedList::isEmpty() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->isEmpty();
  else return true;
}

size_t SinglyLinkedList::getElementCount() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->getElementCount();
  else return 0;
}

Variable SinglyLinkedList::pushBack(Variable var) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->pushBack(std::move(var));
  else return nullptr;
}

SinglyLinkedList::Iterator SinglyLinkedList::pushBack(const literals::sllist value_list) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->pushBack(value_list);
  else return Iterator(nullptr, nullptr);
}

Variable SinglyLinkedList::pushFront(Variable var) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->pushFront(std::move(var));
  else return nullptr;
}

SinglyLinkedList::Iterator SinglyLinkedList::pushFront(const literals::sllist& value_list) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->pushFront(value_list);
  else return Iterator(nullptr, nullptr);
}

SinglyLinkedList::Iterator SinglyLinkedList::insert(Iterator it, Variable var) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->insert(it, std::move(var));
  else return Iterator(nullptr, nullptr);
}

void SinglyLinkedList::remove(Iterator it) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->remove(it);
  else return;
}

void SinglyLinkedList::clear() {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->clear();
  else return;
}

SinglyLinkedList::Iterator SinglyLinkedList::begin() {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->begin();
  else return Iterator(nullptr, nullptr);
}

SinglyLinkedList::Iterator SinglyLinkedList::end() {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->end();
  else return Iterator(nullptr, nullptr);
}

SinglyLinkedList::ConstIterator SinglyLinkedList::cbegin() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->begin();
  else return Iterator(nullptr, nullptr);
}

SinglyLinkedList::ConstIterator SinglyLinkedList::cend() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->end();
  else return Iterator(nullptr, nullptr);
}

SinglyLinkedList& SinglyLinkedList::operator+=(Variable var) {pushBack(var.move()); return self;}

SinglyLinkedList& SinglyLinkedList::operator+=(const SinglyLinkedList& sl_list) {
  for(const Variable& var : sl_list) pushBack(var);
  return self;
}

SinglyLinkedListImplementation::Iterator::Iterator(Node* node, Node* prev) : prev(prev), node(node) {}
SinglyLinkedListImplementation::Iterator::Iterator(const Iterator& other) : prev(other.prev), node(other.node) {}
SinglyLinkedListImplementation::Iterator::Iterator(const Iterator&& other) : prev(other.prev), node(other.node) {}

binom::priv::SinglyLinkedListImplementation::Iterator& SinglyLinkedListImplementation::Iterator::operator++() {if(node) { prev = node; node = node->next;} return self;}
binom::priv::SinglyLinkedListImplementation::Iterator SinglyLinkedListImplementation::Iterator::operator++(int) {Iterator tmp(self); ++self; return tmp;}

const binom::priv::SinglyLinkedListImplementation::Iterator& SinglyLinkedListImplementation::Iterator::operator++() const {if(node) { prev = node; node = node->next;} return self;}
const binom::priv::SinglyLinkedListImplementation::Iterator SinglyLinkedListImplementation::Iterator::operator++(int) const {Iterator tmp(self); ++self; return tmp;}

Variable SinglyLinkedListImplementation::Iterator::operator*() {if(node) return node->value.move(); else return nullptr;}
Variable* SinglyLinkedListImplementation::Iterator::operator->() {if(node) return &node->value; else return nullptr;}

const Variable SinglyLinkedListImplementation::Iterator::operator*() const {if(node) return node->value.move(); else return nullptr;}
const Variable* SinglyLinkedListImplementation::Iterator::operator->() const {if(node) return &node->value; else return nullptr;}

bool SinglyLinkedListImplementation::Iterator::operator==(const Iterator& other) const noexcept {return node == other.node;}
bool SinglyLinkedListImplementation::Iterator::operator==(const Iterator&& other) const noexcept {return node == other.node;}
bool SinglyLinkedListImplementation::Iterator::operator!=(const Iterator& other) const noexcept {return node != other.node;}
bool SinglyLinkedListImplementation::Iterator::operator!=(const Iterator&& other) const noexcept {return node != other.node;}

SinglyLinkedList& SinglyLinkedList::operator=(const SinglyLinkedList& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

SinglyLinkedList& SinglyLinkedList::operator=(SinglyLinkedList&& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

SinglyLinkedList& SinglyLinkedList::changeLink(const SinglyLinkedList& other) {
  if(this == &other) return self;
  this->~SinglyLinkedList();
  return *new(this) SinglyLinkedList(other);
}

SinglyLinkedList& SinglyLinkedList::changeLink(SinglyLinkedList&& other) {
  if(this == &other) return self;
  this->~SinglyLinkedList();
  return *new(this) SinglyLinkedList(std::move(other));
}
