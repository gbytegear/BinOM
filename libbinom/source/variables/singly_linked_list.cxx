#include "libbinom/include/variables/singly_linked_list.hxx"

using namespace binom;
using namespace binom::priv;

inline SinglyLinkedListHeader*& SinglyLinkedList::getData() const noexcept {return resource_link->data.single_linked_list_header;}

SinglyLinkedList::SinglyLinkedList(priv::Link&& link) : Variable(std::move(link)) {}

SinglyLinkedList::SinglyLinkedList() : Variable(literals::sllist{}) {}
SinglyLinkedList::SinglyLinkedList(const literals::sllist singly_linked_list) : Variable(singly_linked_list) {}
SinglyLinkedList::SinglyLinkedList(const SinglyLinkedList& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
SinglyLinkedList::SinglyLinkedList(const SinglyLinkedList&& other) noexcept : Variable(dynamic_cast<const Variable&&>(other)) {}

SinglyLinkedList SinglyLinkedList::getReference() noexcept {return Link(resource_link);}
const SinglyLinkedList SinglyLinkedList::getReference() const noexcept {return Link(resource_link);}

bool SinglyLinkedList::isEmpty() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->isEmpty();
  else return true;
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

SinglyLinkedList::Iterator SinglyLinkedList::begin() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->begin();
  else return Iterator(nullptr, nullptr);
}

SinglyLinkedList::Iterator SinglyLinkedList::end() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->end();
  else return Iterator(nullptr, nullptr);
}

SinglyLinkedListHeader::Iterator::Iterator(Node* node, Node* prev) : prev(prev), node(node) {}
SinglyLinkedListHeader::Iterator::Iterator(const Iterator& other) : prev(other.prev), node(other.node) {}
SinglyLinkedListHeader::Iterator::Iterator(const Iterator&& other) : prev(other.prev), node(other.node) {}

binom::priv::SinglyLinkedListHeader::Iterator& SinglyLinkedListHeader::Iterator::operator++() {if(node) { prev = node; node = node->next;} return self;}
binom::priv::SinglyLinkedListHeader::Iterator SinglyLinkedListHeader::Iterator::operator++(int) {Iterator tmp(self); ++self; return tmp;}

Variable SinglyLinkedListHeader::Iterator::operator*() {if(node) return node->value.getReference(); else return nullptr;}
Variable* SinglyLinkedListHeader::Iterator::operator->() {if(node) return &node->value; else return nullptr;}

bool SinglyLinkedListHeader::Iterator::operator==(const Iterator& other) const noexcept {return node == other.node;}
bool SinglyLinkedListHeader::Iterator::operator==(const Iterator&& other) const noexcept {return node == other.node;}
bool SinglyLinkedListHeader::Iterator::operator!=(const Iterator& other) const noexcept {return node != other.node;}
bool SinglyLinkedListHeader::Iterator::operator!=(const Iterator&& other) const noexcept {return node != other.node;}
