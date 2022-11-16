#include "libbinom/include/binom_impl/ram_storage_implementation/map_impl.hxx"
#include "libbinom/include/variables/named_variable.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;









//MapImplementation::MapImplementation(const literals::map& map) { for(auto& element : map) insert(std::move(element.getKey()), element.getVariable().move()); }

//MapImplementation::MapImplementation(const MapImplementation& other) : storage(other.storage) {}

//MapImplementation::MapImplementation(MapImplementation&& other) : storage(std::move(other.storage)) {}

//MapImplementation::~MapImplementation() { clear(); }

//bool MapImplementation::isEmpty() const noexcept {return !storage.size();}

//size_t MapImplementation::getSize() const noexcept {return storage.size();}

//bool MapImplementation::contains(KeyValue value) const noexcept {return storage.contains(value);}

//err::ProgressReport<FieldRef> MapImplementation::insert(KeyValue key, Variable variable) {
//  auto insert_result = storage.emplace(std::move(key), variable.move());
//  if(insert_result.second) return FieldRef(*insert_result.first);
//  return err::ErrorType::binom_key_unique_error;
//}

//binom::err::Error MapImplementation::remove(KeyValue key) {
//  if(storage.erase(std::move(key))) return ErrorType::no_error;
//  else return ErrorType::binom_out_of_range;
//}

//err::ProgressReport<FieldRef> MapImplementation::rename(KeyValue old_key, KeyValue new_key) {
//  auto element = storage.extract(old_key);
//  if(element.empty()) return ErrorType::binom_out_of_range;
//  element.key() = new_key;
//  auto insert_result = storage.insert(std::move(element));
//  if(insert_result.inserted) return FieldRef(*insert_result.position);
//  insert_result.node.key() = old_key;
//  insert_result = storage.insert(std::move(insert_result.node));
//  if(insert_result.inserted) return {ErrorType::binom_key_unique_error, FieldRef(*insert_result.position)};
//  return ErrorType::binom_key_unique_error;
//}

//FieldRef MapImplementation::getOrInsertFieldRef(KeyValue key) {
//  auto it = storage.find(key);
//  if(it == storage.cend())
//    return *storage.emplace(std::move(key), nullptr).first;
//  return *it;
//}

//binom::Variable MapImplementation::getVariable(KeyValue key) const {
//  auto it = storage.find(key);
//  if(it == storage.cend())
//    return nullptr;
//  return it->second.move();
//}

//MapImplementation::Iterator MapImplementation::find(KeyValue key) {return storage.find(std::move(key));}

//MapImplementation::ReverseIterator MapImplementation::rfind(KeyValue key) {return std::reverse_iterator(storage.find(std::move(key)));}

//MapImplementation::ConstIterator MapImplementation::find(KeyValue key) const {return storage.find(std::move(key));}

//MapImplementation::ConstReverseIterator MapImplementation::rfind(KeyValue key) const {return std::reverse_iterator(storage.find(std::move(key)));}

//void MapImplementation::clear() {storage.clear();}

//MapImplementation::Iterator MapImplementation::begin() noexcept {return storage.begin();}

//MapImplementation::Iterator MapImplementation::end() noexcept {return storage.end();}

//MapImplementation::ReverseIterator MapImplementation::rbegin() noexcept {return storage.rbegin();}

//MapImplementation::ReverseIterator MapImplementation::rend() noexcept {return storage.rend();}

//MapImplementation::ConstIterator MapImplementation::begin() const noexcept {return cbegin();}

//MapImplementation::ConstIterator MapImplementation::end() const noexcept {return cend();}

//MapImplementation::ConstReverseIterator MapImplementation::rbegin() const noexcept {return crbegin();}

//MapImplementation::ConstReverseIterator MapImplementation::rend() const noexcept {return crend();}

//MapImplementation::ConstIterator MapImplementation::cbegin() const noexcept {return storage.cbegin();}

//MapImplementation::ConstIterator MapImplementation::cend() const noexcept {return storage.cend();}

//MapImplementation::ConstReverseIterator MapImplementation::crbegin() const noexcept {return storage.crbegin();}

//MapImplementation::ConstReverseIterator MapImplementation::crend() const noexcept {return storage.crend();}




//MapImplementation::Iterator::Iterator(VariableMap::iterator map_it) : VariableMap::iterator(map_it) {}

//MapImplementation::Iterator::Iterator(const Iterator& other) : VariableMap::iterator(dynamic_cast<const VariableMap::iterator&>(other)) {}

//MapImplementation::Iterator::Iterator(Iterator&& other) : VariableMap::iterator(dynamic_cast<VariableMap::iterator&&>(other)) {}

//FieldRef MapImplementation::Iterator::operator*() {return FieldRef(*dynamic_cast<VariableMap::iterator&>(self));}

//pseudo_ptr::PseudoPointer<FieldRef> MapImplementation::Iterator::operator->() {return *self;}

//const FieldRef MapImplementation::Iterator::operator*() const {return *dynamic_cast<const VariableMap::iterator&>(self);}

//pseudo_ptr::PseudoPointer<const FieldRef> MapImplementation::Iterator::operator->() const {return *self;}




//MapImplementation::ReverseIterator::ReverseIterator(VariableMap::reverse_iterator map_rit) : VariableMap::reverse_iterator(map_rit) {}

//MapImplementation::ReverseIterator::ReverseIterator(const ReverseIterator& other) : VariableMap::reverse_iterator(dynamic_cast<const VariableMap::reverse_iterator&>(other)) {}

//MapImplementation::ReverseIterator::ReverseIterator(ReverseIterator&& other) : VariableMap::reverse_iterator(dynamic_cast<VariableMap::reverse_iterator&&>(other)) {}

//FieldRef MapImplementation::ReverseIterator::operator*() {return *dynamic_cast<VariableMap::reverse_iterator&>(self);}

//pseudo_ptr::PseudoPointer<FieldRef> MapImplementation::ReverseIterator::operator->() {return *self;}

//const FieldRef MapImplementation::ReverseIterator::operator*() const {return *dynamic_cast<const VariableMap::reverse_iterator&>(self);}

//pseudo_ptr::PseudoPointer<const FieldRef> MapImplementation::ReverseIterator::operator->() const {return *self;}




//MapImplementation::ConstIterator::ConstIterator(VariableMap::const_iterator map_it) : VariableMap::const_iterator(map_it) {}

//MapImplementation::ConstIterator::ConstIterator(const ConstIterator& other) : VariableMap::const_iterator(dynamic_cast<const VariableMap::const_iterator&>(other)) {}

//MapImplementation::ConstIterator::ConstIterator(ConstIterator&& other) : VariableMap::const_iterator(dynamic_cast<VariableMap::const_iterator&&>(other)) {}

//const FieldRef MapImplementation::ConstIterator::operator*() const {return *dynamic_cast<const VariableMap::const_iterator&>(self);}

//pseudo_ptr::PseudoPointer<const FieldRef> MapImplementation::ConstIterator::operator->() const {return *self;}




//MapImplementation::ConstReverseIterator::ConstReverseIterator(VariableMap::const_reverse_iterator map_rit) : VariableMap::const_reverse_iterator(map_rit) {}

//MapImplementation::ConstReverseIterator::ConstReverseIterator(const ConstReverseIterator& other) : VariableMap::const_reverse_iterator(dynamic_cast<const VariableMap::const_reverse_iterator&>(other)) {}

//MapImplementation::ConstReverseIterator::ConstReverseIterator(ConstReverseIterator&& other) : VariableMap::const_reverse_iterator(dynamic_cast<VariableMap::const_reverse_iterator&&>(other)) {}

//const FieldRef MapImplementation::ConstReverseIterator::operator*() const {return *dynamic_cast<const VariableMap::const_reverse_iterator&>(self);}

//pseudo_ptr::PseudoPointer<const FieldRef> MapImplementation::ConstReverseIterator::operator->() const {return *self;}

MapImplementation::MapImplementation(WeakLink owner, const map& map) {for(auto& element : map) insert(owner, std::move(element.getKey()), element.getValue().move());}

err::ProgressReport<FieldRef> MapImplementation::insert(WeakLink owner, KeyValue key, Variable variable) {
  auto insert_result = data.emplace(owner, std::move(key), variable.move());
  if(insert_result.second) return FieldRef(*insert_result.first);
  return err::ErrorType::binom_key_unique_error;
}

Error MapImplementation::remove(KeyValue key) {
  if(auto it = data.find(std::move(key)); it != data.cend()) {
    data.erase(it);
    return ErrorType::no_error;
  } else return ErrorType::binom_out_of_range;
}

err::ProgressReport<FieldRef> MapImplementation::rename(KeyValue old_key, KeyValue new_key) {
  auto element = data.extract(data.find(old_key));
  if(element.empty()) return ErrorType::binom_out_of_range;
  element.value().setKey(new_key);
  auto insert_result = data.insert(std::move(element));
  if(insert_result.inserted) return FieldRef(*insert_result.position);
  insert_result.node.value().setKey(std::move(old_key));
  insert_result = data.insert(std::move(insert_result.node));
  if(insert_result.inserted) return {ErrorType::binom_key_unique_error, FieldRef(*insert_result.position)};
  return ErrorType::binom_key_unique_error;
}

FieldRef MapImplementation::getOrInsertFieldRef(WeakLink owner, KeyValue key) {
  auto it = data.find(key);
  if(it == data.cend()) return *data.emplace(std::move(owner), std::move(key), nullptr).first;
  return *it;
}

Variable MapImplementation::getVariable(KeyValue key) const {
  auto it = data.find(key);
  if(it == data.cend()) return nullptr;
  return it->getValue().move();
}

FieldRef MapImplementation::getField(KeyValue key) const {
  auto it = data.find(key);
  if(it == data.cend()) return nullptr;
  return *it;
}

MapImplementation::Iterator MapImplementation::find(KeyValue key) { return data.find(std::move(key)); }

MapImplementation::ReverseIterator MapImplementation::rfind(KeyValue key) {return std::reverse_iterator(data.find(std::move(key)));}

MapImplementation::ConstIterator MapImplementation::find(KeyValue key) const {return data.find(std::move(key));}

MapImplementation::ConstReverseIterator MapImplementation::rfind(KeyValue key) const {return std::reverse_iterator(data.find(std::move(key)));}

void MapImplementation::clear() {data.clear();}

MapImplementation::Iterator MapImplementation::begin() noexcept {return data.begin();}

MapImplementation::Iterator MapImplementation::end() noexcept {return data.end();}

MapImplementation::ReverseIterator MapImplementation::rbegin() noexcept {return data.rbegin();}

MapImplementation::ReverseIterator MapImplementation::rend() noexcept {return data.rend();}

MapImplementation::ConstIterator MapImplementation::begin() const noexcept {return cbegin();}

MapImplementation::ConstIterator MapImplementation::end() const noexcept {return cend();}

MapImplementation::ConstReverseIterator MapImplementation::rbegin() const noexcept {return crbegin();}

MapImplementation::ConstReverseIterator MapImplementation::rend() const noexcept {return crend();}

MapImplementation::ConstIterator MapImplementation::cbegin() const noexcept {return data.cbegin();}

MapImplementation::ConstIterator MapImplementation::cend() const noexcept {return data.cend();}

MapImplementation::ConstReverseIterator MapImplementation::crbegin() const noexcept {return data.crbegin();}

MapImplementation::ConstReverseIterator MapImplementation::crend() const noexcept {return data.crend();}




MapImplementation::Iterator::Iterator(ContainerType::iterator map_it) : ContainerType::iterator(map_it) {}

MapImplementation::Iterator::Iterator(const Iterator& other) : ContainerType::iterator(dynamic_cast<const ContainerType::iterator&>(other)) {}

MapImplementation::Iterator::Iterator(Iterator&& other) : ContainerType::iterator(dynamic_cast<ContainerType::iterator&&>(other)) {}

FieldRef MapImplementation::Iterator::operator*() {return FieldRef(*dynamic_cast<ContainerType::iterator&>(self));}

pseudo_ptr::PseudoPointer<FieldRef> MapImplementation::Iterator::operator->() {return *self;}

const FieldRef MapImplementation::Iterator::operator*() const {return *dynamic_cast<const ContainerType::iterator&>(self);}

pseudo_ptr::PseudoPointer<const FieldRef> MapImplementation::Iterator::operator->() const {return *self;}






MapImplementation::ReverseIterator::ReverseIterator(ContainerType::reverse_iterator map_rit) : ContainerType::reverse_iterator(map_rit) {}

MapImplementation::ReverseIterator::ReverseIterator(const ReverseIterator& other) : ContainerType::reverse_iterator(dynamic_cast<const ContainerType::reverse_iterator&>(other)) {}

MapImplementation::ReverseIterator::ReverseIterator(ReverseIterator&& other) : ContainerType::reverse_iterator(dynamic_cast<ContainerType::reverse_iterator&&>(other)) {}

FieldRef MapImplementation::ReverseIterator::operator*() {return *dynamic_cast<ContainerType::reverse_iterator&>(self);}

pseudo_ptr::PseudoPointer<FieldRef> MapImplementation::ReverseIterator::operator->() {return *self;}

const FieldRef MapImplementation::ReverseIterator::operator*() const {return *dynamic_cast<const ContainerType::reverse_iterator&>(self);}

pseudo_ptr::PseudoPointer<const FieldRef> MapImplementation::ReverseIterator::operator->() const {return *self;}




MapImplementation::ConstIterator::ConstIterator(ContainerType::const_iterator map_it) : ContainerType::const_iterator(map_it) {}

MapImplementation::ConstIterator::ConstIterator(const ConstIterator& other) : ContainerType::const_iterator(dynamic_cast<const ContainerType::const_iterator&>(other)) {}

MapImplementation::ConstIterator::ConstIterator(ConstIterator&& other) : ContainerType::const_iterator(dynamic_cast<ContainerType::const_iterator&&>(other)) {}

const FieldRef MapImplementation::ConstIterator::operator*() const {return *dynamic_cast<const ContainerType::const_iterator&>(self);}

pseudo_ptr::PseudoPointer<const FieldRef> MapImplementation::ConstIterator::operator->() const {return *self;}




MapImplementation::ConstReverseIterator::ConstReverseIterator(ContainerType::const_reverse_iterator map_rit) : ContainerType::const_reverse_iterator(map_rit) {}

MapImplementation::ConstReverseIterator::ConstReverseIterator(const ConstReverseIterator& other) : ContainerType::const_reverse_iterator(dynamic_cast<const ContainerType::const_reverse_iterator&>(other)) {}

MapImplementation::ConstReverseIterator::ConstReverseIterator(ConstReverseIterator&& other) : ContainerType::const_reverse_iterator(dynamic_cast<ContainerType::const_reverse_iterator&&>(other)) {}

const FieldRef MapImplementation::ConstReverseIterator::operator*() const {return *dynamic_cast<const ContainerType::const_reverse_iterator&>(self);}

pseudo_ptr::PseudoPointer<const FieldRef> MapImplementation::ConstReverseIterator::operator->() const {return *self;}
