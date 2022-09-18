#include "libbinom/include/variables/multi_map.hxx"

using namespace binom;
using namespace binom::priv;

MultiMapImplementation* MultiMap::getData() { return resource_link->data.multi_map_implementation; }

const MultiMapImplementation* MultiMap::getData() const { return resource_link->data.multi_map_implementation; }

MultiMap::MultiMap(priv::Link&& link) : Variable(std::move(link)) {}

MultiMap::MultiMap() : Variable(literals::multimap{}) {}

MultiMap::MultiMap(literals::multimap element_list)
  : Variable(std::move(element_list)) {}

MultiMap::MultiMap(const MultiMap& other)
  : Variable(other) {}

MultiMap::MultiMap(MultiMap&& other)
  : Variable(std::move(other)) {}

MultiMap MultiMap::move() noexcept {return Link(resource_link);}
const MultiMap MultiMap::move() const noexcept {return Link(resource_link);}

bool MultiMap::isEmpty() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->isEmpty();
}

size_t MultiMap::getElementCount() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->getSize();
}

bool MultiMap::contains(KeyValue value) const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return false;
  return getData()->contains(std::move(value));
}

void MultiMap::clear() {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;
  return getData()->clear();
}

err::ProgressReport<MultiMap::NamedVariable> MultiMap::insert(KeyValue key, Variable variable, NewNodePosition position) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return err::ErrorType::binom_resource_not_available;
  return getData()->insert(std::move(key), variable.move(), position);
}

Error MultiMap::remove(Iterator it) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return err::ErrorType::binom_resource_not_available;
  getData()->remove(std::move(it));
  return err::ErrorType::no_error;
}

Error MultiMap::remove(ReverseIterator it) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return err::ErrorType::binom_resource_not_available;
  getData()->remove(std::move(it));
  return err::ErrorType::no_error;
}

Error MultiMap::removeAll(KeyValue key) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return err::ErrorType::binom_resource_not_available;
  getData()->removeAll(std::move(key));
  return err::ErrorType::no_error;
}

err::ProgressReport<MultiMap::NamedVariable> MultiMap::rename(Iterator it, KeyValue new_key) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return err::ErrorType::binom_resource_not_available;
  return getData()->rename(std::move(it), std::move(new_key));
}

std::pair<MultiMap::Iterator, MultiMap::Iterator> MultiMap::getRange(KeyValue key) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return {Iterator::nullit(), Iterator::nullit()};
  return getData()->getRange(std::move(key));
}

std::pair<MultiMap::ReverseIterator, MultiMap::ReverseIterator> MultiMap::getReverseRange(KeyValue key) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return {ReverseIterator::nullit(), ReverseIterator::nullit()};
  return getData()->getReverseRange(std::move(key));
}

std::pair<MultiMap::ConstIterator, MultiMap::ConstIterator> MultiMap::getRange(KeyValue key) const {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return {ConstIterator::nullit(), ConstIterator::nullit()};
  return getData()->getRange(std::move(key));
}

std::pair<MultiMap::ConstReverseIterator, MultiMap::ConstReverseIterator> MultiMap::getReverseRange(KeyValue key) const {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return {ConstReverseIterator::nullit(), ConstReverseIterator::nullit()};
  return getData()->getReverseRange(std::move(key));
}

MultiMap::Iterator MultiMap::find(KeyValue key) {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator::nullit();
  return getData()->find(std::move(key));
}

MultiMap::ReverseIterator MultiMap::rfind(KeyValue key) {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ReverseIterator::nullit();
  return getData()->rfind(std::move(key));
}

MultiMap::Iterator MultiMap::findLast(KeyValue key) {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator::nullit();
  return getData()->findLast(std::move(key));
}

MultiMap::ReverseIterator MultiMap::rfindLast(KeyValue key) {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ReverseIterator::nullit();
  return getData()->rfindLast(std::move(key));
}

MultiMap::ConstIterator MultiMap::find(KeyValue key) const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstIterator::nullit();
  return getData()->find(std::move(key));
}

MultiMap::ConstReverseIterator MultiMap::rfind(KeyValue key) const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstReverseIterator::nullit();
  return getData()->rfind(std::move(key));
}

MultiMap::ConstIterator MultiMap::findLast(KeyValue key) const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstIterator::nullit();
  return getData()->findLast(std::move(key));
}

MultiMap::ConstReverseIterator MultiMap::rfindLast(KeyValue key) const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstReverseIterator::nullit();
  return getData()->rfindLast(std::move(key));
}

MultiMap::Iterator MultiMap::operator[](KeyValue key) noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator::nullit();
  return getData()->find(std::move(key));
}

MultiMap::ConstIterator MultiMap::operator[](KeyValue key) const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstIterator::nullit();
  return getData()->find(std::move(key));
}

MultiMap::Iterator MultiMap::begin() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator::nullit();
  return getData()->begin();
}

MultiMap::Iterator MultiMap::end() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator::nullit();
  return getData()->end();
}

MultiMap::ReverseIterator MultiMap::rbegin() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ReverseIterator::nullit();
  return getData()->rbegin();
}

MultiMap::ReverseIterator MultiMap::rend() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ReverseIterator::nullit();
  return getData()->rend();
}

MultiMap::ConstIterator MultiMap::begin() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstIterator::nullit();
  return getData()->cbegin();
}

MultiMap::ConstIterator MultiMap::end() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstIterator::nullit();
  return getData()->cend();
}

MultiMap::ConstIterator MultiMap::cbegin() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstIterator::nullit();
  return getData()->cbegin();
}

MultiMap::ConstIterator MultiMap::cend() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstIterator::nullit();
  return getData()->cend();
}

MultiMap::ConstReverseIterator MultiMap::rbegin() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstReverseIterator::nullit();
  return getData()->crbegin();
}

MultiMap::ConstReverseIterator MultiMap::rend() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstReverseIterator::nullit();
  return getData()->crend();
}

MultiMap::ConstReverseIterator MultiMap::crbegin() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstReverseIterator::nullit();
  return getData()->crbegin();
}

MultiMap::ConstReverseIterator MultiMap::crend() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstReverseIterator::nullit();
  return getData()->crend();
}

MultiMap& MultiMap::operator=(const MultiMap& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

MultiMap& MultiMap::operator=(MultiMap&& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

MultiMap& MultiMap::changeLink(const MultiMap& other) {
  if(this == &other) return self;
  this->~MultiMap();
  return *new(this) MultiMap(other);
}

MultiMap& MultiMap::changeLink(MultiMap&& other) {
  if(this == &other) return self;
  this->~MultiMap();
  return *new(this) MultiMap(std::move(other));
}