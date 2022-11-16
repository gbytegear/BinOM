#include "libbinom/include/variables/map.hxx"

using namespace binom;
using namespace binom::priv;

MapImplementation* Map::getData() { return resource_link->data.map_implementation; }

const MapImplementation* Map::getData() const { return resource_link->data.map_implementation; }

Map::Map(priv::Link&& link) : Variable(std::move(link)) {}

Map::Map() : Variable(literals::map{}) {}

Map::Map(literals::map element_list)
  : Variable(std::move(element_list)) {}

Map::Map(const Map& other)
  : Variable(other) {}

Map::Map(Map&& other)
  : Variable(std::move(other)) {}

Map Map::move() noexcept {return Link(resource_link);}
const Map Map::move() const noexcept {return Link(resource_link);}

bool Map::isEmpty() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->isEmpty();
}

size_t Map::getElementCount() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->getSize();
}

bool Map::contains(KeyValue value) const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return false;
  return getData()->contains(std::move(value));
}

void Map::clear() {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;
  return getData()->clear();
}

err::ProgressReport<FieldRef> Map::insert(KeyValue key, Variable variable) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return err::ErrorType::binom_resource_not_available;
  return getData()->insert(resource_link, std::move(key), variable.move());
}

Error Map::remove(KeyValue key) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return err::ErrorType::binom_resource_not_available;
  return getData()->remove(std::move(key));
}

err::ProgressReport<FieldRef> Map::rename(KeyValue old_key, KeyValue new_key) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return err::ErrorType::binom_resource_not_available;
  return getData()->rename(std::move(old_key), std::move(new_key));
}

Variable Map::getVariable(KeyValue key) {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return nullptr;
  return getData()->getVariable(std::move(key));
}

FieldRef binom::Map::getField(KeyValue key) {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return nullptr;
  return getData()->getField(std::move(key));
}

FieldRef Map::operator[](KeyValue key) {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return nullptr;
  return getData()->getOrInsertFieldRef(resource_link, std::move(key));
}

Map::Iterator Map::find(KeyValue key) {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator::nullit();
  return getData()->find(std::move(key));
}

Map::ReverseIterator Map::rfind(KeyValue key) {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ReverseIterator::nullit();
  return getData()->rfind(std::move(key));
}

Map::ConstIterator Map::find(KeyValue key) const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstIterator::nullit();
  return getData()->find(std::move(key));
}

Map::ConstReverseIterator Map::rfind(KeyValue key) const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstReverseIterator::nullit();
  return getData()->rfind(std::move(key));
}

const Variable Map::getVariable(KeyValue key) const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return nullptr;
  return getData()->getVariable(std::move(key));
}

const FieldRef Map::getField(KeyValue key) const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return nullptr;
  return getData()->getField(std::move(key));
}

const FieldRef Map::operator[](KeyValue key) const {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return nullptr;
  return getData()->getField(std::move(key));
}

Map::Iterator Map::begin() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator::nullit();
  return getData()->begin();
}

Map::Iterator Map::end() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return Iterator::nullit();
  return getData()->end();
}

Map::ReverseIterator Map::rbegin() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ReverseIterator::nullit();
  return getData()->rbegin();
}

Map::ReverseIterator Map::rend() {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ReverseIterator::nullit();
  return getData()->rend();
}

Map::ConstIterator Map::begin() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstIterator::nullit();
  return getData()->cbegin();
}

Map::ConstIterator Map::end() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstIterator::nullit();
  return getData()->cend();
}

Map::ConstReverseIterator Map::rbegin() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstReverseIterator::nullit();
  return getData()->crbegin();
}

Map::ConstReverseIterator Map::rend() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return ConstReverseIterator::nullit();
  return getData()->crend();
}


Map& Map::operator=(const Map& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

Map& Map::operator=(Map&& other) {
  if(this == &other) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

Map& Map::changeLink(const Map& other) {
  if(this == &other) return self;
  this->~Map();
  return *new(this) Map(other);
}

Map& Map::changeLink(Map&& other) {
  if(this == &other) return self;
  this->~Map();
  return *new(this) Map(std::move(other));
}
