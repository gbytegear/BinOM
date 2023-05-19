#include "libbinom/include/variables/list.hxx"

using namespace binom;
using namespace binom::priv;

ListImplementation*& List::getData() const noexcept {return resource_link->data.list_implementation;}

List::List(priv::Link&& link) : Variable(std::move(link)) {}

List::List() : Variable(literals::list{}) {}
List::List(const literals::list doubly_linked_list) : Variable(doubly_linked_list) {}
List::List(const List& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
List::List(const List&& other) noexcept : Variable(dynamic_cast<const Variable&&>(other)) {}

bool List::isEmpty() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->isEmpty();
  else return true;
}

size_t List::getElementCount() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->getElementCount();
  else return 0;
}

List List::move() noexcept {return Link(resource_link);}
const List List::move() const noexcept {return Link(resource_link);}

Variable List::pushBack(Variable variable) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->pushBack(std::move(variable));
  else return nullptr;
}

Variable List::pushFront(Variable variable) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->pushFront(std::move(variable));
  else return nullptr;
}

Variable List::insert(Iterator it, Variable variable) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return *getData()->insert(it, std::move(variable));
  else return nullptr;
}

void List::popBack() {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->popBack();
  else return;
}

void List::popFront() {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->popFront();
  else return;
}

void List::remove(Iterator it) {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->remove(it);
  else return;
}

void List::clear() {
  if(auto lk = getLock(MtxLockType::unique_locked); lk)
    return getData()->clear();
  else return;
}

List::Iterator List::begin() {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->begin();
  else return Iterator(nullptr);
}

List::Iterator List::end() {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->end();
  else return Iterator(nullptr);
}

List::ConstIterator List::begin() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->begin();
  else return Iterator(nullptr);
}

List::ConstIterator List::end() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->end();
  else return Iterator(nullptr);
}

List::ReverseIterator List::rbegin() {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->rbegin();
  else return ReverseIterator(Iterator(nullptr));
}

List::ReverseIterator List::rend() {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->rend();
  else return ReverseIterator(Iterator(nullptr));
}

List::ConstReverseIterator List::rbegin() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->rbegin();
  else return ConstReverseIterator(Iterator(nullptr));
}

List::ConstReverseIterator List::rend() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->rend();
  else return ConstReverseIterator(Iterator(nullptr));
}

List::ConstIterator List::cbegin() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->begin();
  else return Iterator(nullptr);
}

List::ConstIterator List::cend() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->cend();
  else return Iterator(nullptr);
}

List::ConstReverseIterator List::crbegin() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->crbegin();
  else return ConstReverseIterator(Iterator(nullptr));
}

List::ConstReverseIterator List::crend() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->crend();
  else return ConstReverseIterator(Iterator(nullptr));
}

List& List::operator=(const List& other) {
  if(resource_link == other.resource_link) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

List& List::operator=(List&& other) {
  if(resource_link == other.resource_link) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

List& List::changeLink(const List& other) {
  if(this == &other) return self;
  this->~List();
  return *new(this) List(other);
}

List& List::changeLink(List&& other) {
  if(this == &other) return self;
  this->~List();
  return *new(this) List(std::move(other));
}

