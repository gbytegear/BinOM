#include "libbinom/include/binom_impl/ram_storage_implementation/map_impl.hxx"
#include "libbinom/include/variables/named_variable.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;

MapImplementation::MapImplementation(const literals::map& map) { for(auto& element : map) insert(std::move(element.getKey()), element.getVariable().move()); }

MapImplementation::MapImplementation(const MapImplementation& other) { for(auto& element : other.storage) insert(element.first, element.second); }

MapImplementation::~MapImplementation() { clear(); }

bool MapImplementation::isEmpty() const noexcept {return !storage.size();}

size_t MapImplementation::getSize() const noexcept {return storage.size();}

bool MapImplementation::contains(KeyValue value) const noexcept {return storage.contains(value);}

err::ProgressReport<MapImplementation::NamedVariable> MapImplementation::insert(KeyValue key, Variable variable) {
  auto insert_result = storage.emplace(std::move(key), variable.move());
  if(insert_result.second) return MapImplementation::NamedVariable(*insert_result.first);
  return err::ErrorType::binom_key_unique_error;
}

binom::err::Error MapImplementation::remove(KeyValue key) {
  if(storage.erase(std::move(key))) return ErrorType::no_error;
  else return ErrorType::binom_out_of_range;
}

err::ProgressReport<MapImplementation::NamedVariable> MapImplementation::rename(KeyValue old_key, KeyValue new_key) {
  auto element = storage.extract(old_key);
  if(element.empty()) return ErrorType::binom_out_of_range;
  element.key() = new_key;
  auto insert_result = storage.insert(std::move(element));
  if(insert_result.inserted) return NamedVariable(*insert_result.position);
  insert_result.node.key() = old_key;
  insert_result = storage.insert(std::move(insert_result.node));
  if(insert_result.inserted) return {ErrorType::binom_key_unique_error, NamedVariable(*insert_result.position)};
  return ErrorType::binom_key_unique_error;
}

MapImplementation::NamedVariable MapImplementation::getOrInsertNamedVariable(KeyValue key) {
  auto it = storage.find(key);
  if(it == storage.cend())
    return *storage.emplace(std::move(key), nullptr).first;
  return *it;
}

binom::Variable MapImplementation::getVariable(KeyValue key) const {
  auto it = storage.find(key);
  if(it == storage.cend())
    return nullptr;
  return it->second.move();
}

MapImplementation::Iterator MapImplementation::find(KeyValue key) {return storage.find(std::move(key));}

MapImplementation::ReverseIterator MapImplementation::rfind(KeyValue key) {return std::reverse_iterator(storage.find(std::move(key)));}

MapImplementation::ConstIterator MapImplementation::find(KeyValue key) const {return storage.find(std::move(key));}

MapImplementation::ConstReverseIterator MapImplementation::rfind(KeyValue key) const {return std::reverse_iterator(storage.find(std::move(key)));}

void MapImplementation::clear() {storage.clear();}

MapImplementation::Iterator MapImplementation::begin() noexcept {return storage.begin();}

MapImplementation::Iterator MapImplementation::end() noexcept {return storage.end();}

MapImplementation::ReverseIterator MapImplementation::rbegin() noexcept {return storage.rbegin();}

MapImplementation::ReverseIterator MapImplementation::rend() noexcept {return storage.rend();}

MapImplementation::ConstIterator MapImplementation::begin() const noexcept {return cbegin();}

MapImplementation::ConstIterator MapImplementation::end() const noexcept {return cend();}

MapImplementation::ConstReverseIterator MapImplementation::rbegin() const noexcept {return crbegin();}

MapImplementation::ConstReverseIterator MapImplementation::rend() const noexcept {return crend();}

MapImplementation::ConstIterator MapImplementation::cbegin() const noexcept {return storage.cbegin();}

MapImplementation::ConstIterator MapImplementation::cend() const noexcept {return storage.cend();}

MapImplementation::ConstReverseIterator MapImplementation::crbegin() const noexcept {return storage.crbegin();}

MapImplementation::ConstReverseIterator MapImplementation::crend() const noexcept {return storage.crend();}




MapImplementation::Iterator::Iterator(VariableMap::iterator map_it) : VariableMap::iterator(map_it) {}

MapImplementation::Iterator::Iterator(const Iterator& other) : VariableMap::iterator(dynamic_cast<const VariableMap::iterator&>(other)) {}

MapImplementation::Iterator::Iterator(Iterator&& other) : VariableMap::iterator(dynamic_cast<VariableMap::iterator&&>(other)) {}

MapImplementation::NamedVariable MapImplementation::Iterator::operator*() {return NamedVariable(*dynamic_cast<VariableMap::iterator&>(self));}

pseudo_ptr::PseudoPointer<MapImplementation::NamedVariable> MapImplementation::Iterator::operator->() {return *self;}

const MapImplementation::NamedVariable MapImplementation::Iterator::operator*() const {return *dynamic_cast<const VariableMap::iterator&>(self);}

pseudo_ptr::PseudoPointer<const MapImplementation::NamedVariable> MapImplementation::Iterator::operator->() const {return *self;}




MapImplementation::ReverseIterator::ReverseIterator(VariableMap::reverse_iterator map_rit) : VariableMap::reverse_iterator(map_rit) {}

MapImplementation::ReverseIterator::ReverseIterator(const ReverseIterator& other) : VariableMap::reverse_iterator(dynamic_cast<const VariableMap::reverse_iterator&>(other)) {}

MapImplementation::ReverseIterator::ReverseIterator(ReverseIterator&& other) : VariableMap::reverse_iterator(dynamic_cast<VariableMap::reverse_iterator&&>(other)) {}

MapImplementation::NamedVariable MapImplementation::ReverseIterator::operator*() {return *dynamic_cast<VariableMap::reverse_iterator&>(self);}

pseudo_ptr::PseudoPointer<MapImplementation::NamedVariable> MapImplementation::ReverseIterator::operator->() {return *self;}

const MapImplementation::NamedVariable MapImplementation::ReverseIterator::operator*() const {return *dynamic_cast<const VariableMap::reverse_iterator&>(self);}

pseudo_ptr::PseudoPointer<const MapImplementation::NamedVariable> MapImplementation::ReverseIterator::operator->() const {return *self;}




MapImplementation::ConstIterator::ConstIterator(VariableMap::const_iterator map_it) : VariableMap::const_iterator(map_it) {}

MapImplementation::ConstIterator::ConstIterator(const ConstIterator& other) : VariableMap::const_iterator(dynamic_cast<const VariableMap::const_iterator&>(other)) {}

MapImplementation::ConstIterator::ConstIterator(ConstIterator&& other) : VariableMap::const_iterator(dynamic_cast<VariableMap::const_iterator&&>(other)) {}

const MapImplementation::NamedVariable MapImplementation::ConstIterator::operator*() const {return *dynamic_cast<const VariableMap::const_iterator&>(self);}

pseudo_ptr::PseudoPointer<const MapImplementation::NamedVariable> MapImplementation::ConstIterator::operator->() const {return *self;}




MapImplementation::ConstReverseIterator::ConstReverseIterator(VariableMap::const_reverse_iterator map_rit) : VariableMap::const_reverse_iterator(map_rit) {}

MapImplementation::ConstReverseIterator::ConstReverseIterator(const ConstReverseIterator& other) : VariableMap::const_reverse_iterator(dynamic_cast<const VariableMap::const_reverse_iterator&>(other)) {}

MapImplementation::ConstReverseIterator::ConstReverseIterator(ConstReverseIterator&& other) : VariableMap::const_reverse_iterator(dynamic_cast<VariableMap::const_reverse_iterator&&>(other)) {}

const MapImplementation::NamedVariable MapImplementation::ConstReverseIterator::operator*() const {return *dynamic_cast<const VariableMap::const_reverse_iterator&>(self);}

pseudo_ptr::PseudoPointer<const MapImplementation::NamedVariable> MapImplementation::ConstReverseIterator::operator->() const {return *self;}