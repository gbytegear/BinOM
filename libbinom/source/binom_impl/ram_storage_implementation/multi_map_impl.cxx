#include "libbinom/include/binom_impl/ram_storage_implementation/multi_map_impl.hxx"
#include "libbinom/include/variables/named_variable.hxx"
#include "libbinom/include/utils/util_functions.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;

MultiMapImplementation::Iterator::Iterator(VariableMultiMap::iterator map_it) : VariableMultiMap::iterator(map_it) {}

MultiMapImplementation::Iterator::Iterator(const Iterator& other) : VariableMultiMap::iterator(other) {}

MultiMapImplementation::Iterator::Iterator(Iterator&& other) : VariableMultiMap::iterator(std::move(other)) {}

MultiMapImplementation::NamedVariable MultiMapImplementation::Iterator::operator*() {return NamedVariable(*dynamic_cast<VariableMultiMap::iterator&>(self));}

pseudo_ptr::PseudoPointer<MultiMapImplementation::NamedVariable> MultiMapImplementation::Iterator::operator->() {return *self;}

const MultiMapImplementation::NamedVariable MultiMapImplementation::Iterator::operator*() const {return NamedVariable(*dynamic_cast<const VariableMultiMap::iterator&>(self));}

pseudo_ptr::PseudoPointer<const MultiMapImplementation::NamedVariable> MultiMapImplementation::Iterator::operator->() const {return *self;}

MultiMapImplementation::Iterator MultiMapImplementation::Iterator::nullit() noexcept {return  VariableMultiMap::iterator(nullptr);}



MultiMapImplementation::ReverseIterator::ReverseIterator(VariableMultiMap::reverse_iterator map_it) : VariableMultiMap::reverse_iterator(map_it) {}

MultiMapImplementation::ReverseIterator::ReverseIterator(const ReverseIterator& other) : VariableMultiMap::reverse_iterator(other) {}

MultiMapImplementation::ReverseIterator::ReverseIterator(ReverseIterator&& other) : VariableMultiMap::reverse_iterator(std::move(other)) {}

MultiMapImplementation::NamedVariable MultiMapImplementation::ReverseIterator::operator*() {return NamedVariable(*dynamic_cast<VariableMultiMap::reverse_iterator&>(self));}

pseudo_ptr::PseudoPointer<MultiMapImplementation::NamedVariable> MultiMapImplementation::ReverseIterator::operator->() {return *self;}

const MultiMapImplementation::NamedVariable MultiMapImplementation::ReverseIterator::operator*() const {return NamedVariable(*dynamic_cast<const VariableMultiMap::reverse_iterator&>(self));}

pseudo_ptr::PseudoPointer<const MultiMapImplementation::NamedVariable> MultiMapImplementation::ReverseIterator::operator->() const {return *self;}

MultiMapImplementation::ReverseIterator MultiMapImplementation::ReverseIterator::nullit() noexcept {return  VariableMultiMap::reverse_iterator(Iterator::nullit());}



MultiMapImplementation::ConstIterator::ConstIterator(VariableMultiMap::const_iterator map_it) : VariableMultiMap::const_iterator(map_it) {}

MultiMapImplementation::ConstIterator::ConstIterator(const ConstIterator& other) : VariableMultiMap::const_iterator(other) {}

MultiMapImplementation::ConstIterator::ConstIterator(ConstIterator&& other) : VariableMultiMap::const_iterator(std::move(other)) {}

const MultiMapImplementation::NamedVariable MultiMapImplementation::ConstIterator::operator*() const {return NamedVariable(*dynamic_cast<const VariableMultiMap::const_iterator&>(self));}

pseudo_ptr::PseudoPointer<const MultiMapImplementation::NamedVariable> MultiMapImplementation::ConstIterator::operator->() const {return *self;}

MultiMapImplementation::ConstIterator MultiMapImplementation::ConstIterator::nullit() noexcept {return  VariableMultiMap::const_iterator(nullptr);}



MultiMapImplementation::ConstReverseIterator::ConstReverseIterator(VariableMultiMap::const_reverse_iterator map_it) : VariableMultiMap::const_reverse_iterator(map_it) {}

MultiMapImplementation::ConstReverseIterator::ConstReverseIterator(const ConstReverseIterator& other) : VariableMultiMap::const_reverse_iterator(other) {}

MultiMapImplementation::ConstReverseIterator::ConstReverseIterator(ConstReverseIterator&& other) : VariableMultiMap::const_reverse_iterator(std::move(other)) {}

const MultiMapImplementation::NamedVariable MultiMapImplementation::ConstReverseIterator::operator*() const {return NamedVariable(*dynamic_cast<const VariableMultiMap::const_reverse_iterator&>(self));}

pseudo_ptr::PseudoPointer<const MultiMapImplementation::NamedVariable> MultiMapImplementation::ConstReverseIterator::operator->() const {return *self;}

MultiMapImplementation::ConstReverseIterator MultiMapImplementation::ConstReverseIterator::nullit() noexcept {return  VariableMultiMap::const_reverse_iterator(ConstIterator::nullit());}



MultiMapImplementation::MultiMapImplementation(const literals::multimap& map, NewNodePosition pos) {
  for(auto& element : map) insert(std::move(element.getKeyRef()), element.getVariableRef().move(), pos);
}

MultiMapImplementation::MultiMapImplementation(const MultiMapImplementation& other) : storage(other.storage) {}

MultiMapImplementation::MultiMapImplementation(MultiMapImplementation&& other) : storage(std::move(other.storage)) {}

bool MultiMapImplementation::isEmpty() const noexcept {return storage.empty();}

size_t MultiMapImplementation::getSize() const noexcept {return storage.size();}

bool MultiMapImplementation::contains(KeyValue key) const {return storage.contains(std::move(key));}

MultiMapImplementation::NamedVariable MultiMapImplementation::insert(KeyValue key, Variable variable, NewNodePosition position) {
  switch (position) {
  case binom::priv::MultiAVLTree::NewNodePosition::front: return *storage.emplace_hint(storage.lower_bound(key), key, variable);
  default:
  case binom::priv::MultiAVLTree::NewNodePosition::back: return *storage.emplace(key, variable);
  }
}

void MultiMapImplementation::remove(Iterator it) {storage.erase(it);}

void MultiMapImplementation::remove(ReverseIterator it) {storage.erase(VariableMultiMap::reverse_iterator(it).base());}

size_t MultiMapImplementation::removeAll(KeyValue key) {return storage.erase(std::move(key));}

err::ProgressReport<MultiMapImplementation::NamedVariable> MultiMapImplementation::rename(Iterator it, KeyValue new_key) {

  if(auto element = storage.extract(it); !element.empty()) {
    element.key() = new_key;
    return NamedVariable(*storage.insert(std::move(element)));
  } else return ErrorType::binom_out_of_range;

}

std::pair<MultiMapImplementation::Iterator, MultiMapImplementation::Iterator> MultiMapImplementation::getRange(KeyValue key) {
  auto [first, second] = storage.equal_range(std::move(key));
  return {first, second};
}

std::pair<MultiMapImplementation::ReverseIterator, MultiMapImplementation::ReverseIterator> MultiMapImplementation::getReverseRange(KeyValue key) {
  auto [first, second] = storage.equal_range(std::move(key));
  return {VariableMultiMap::reverse_iterator(--first), VariableMultiMap::reverse_iterator(--second)};
}

MultiMapImplementation::Iterator MultiMapImplementation::find(KeyValue key) {return storage.find(std::move(key));}

MultiMapImplementation::ReverseIterator MultiMapImplementation::rfind(KeyValue key) {return VariableMultiMap::reverse_iterator(storage.find(std::move(key)));}

MultiMapImplementation::Iterator MultiMapImplementation::findLast(KeyValue key) {return --storage.upper_bound(std::move(key));}

MultiMapImplementation::ReverseIterator MultiMapImplementation::rfindLast(KeyValue key) {return VariableMultiMap::reverse_iterator(--storage.upper_bound(std::move(key)));}

std::pair<MultiMapImplementation::ConstIterator, MultiMapImplementation::ConstIterator> MultiMapImplementation::getRange(KeyValue key) const {
  auto [first, second] = storage.equal_range(std::move(key));
  return {first, second};
}

std::pair<MultiMapImplementation::ConstReverseIterator, MultiMapImplementation::ConstReverseIterator> MultiMapImplementation::getReverseRange(KeyValue key) const {
  auto [first, second] = storage.equal_range(std::move(key));
  return {VariableMultiMap::const_reverse_iterator(--first), VariableMultiMap::const_reverse_iterator(--second)};
}

MultiMapImplementation::ConstIterator MultiMapImplementation::find(KeyValue key) const  {return storage.find(std::move(key));}

MultiMapImplementation::ConstReverseIterator MultiMapImplementation::rfind(KeyValue key) const {return VariableMultiMap::const_reverse_iterator(storage.find(std::move(key)));}

MultiMapImplementation::ConstIterator MultiMapImplementation::findLast(KeyValue key) const {return --storage.upper_bound(std::move(key));}

MultiMapImplementation::ConstReverseIterator MultiMapImplementation::rfindLast(KeyValue key) const {return VariableMultiMap::const_reverse_iterator(--storage.upper_bound(std::move(key)));}

void MultiMapImplementation::clear() {storage.clear();}

MultiMapImplementation::Iterator MultiMapImplementation::begin() noexcept {return storage.begin();}

MultiMapImplementation::Iterator MultiMapImplementation::end() noexcept {return storage.end();}

MultiMapImplementation::ReverseIterator MultiMapImplementation::rbegin() noexcept {return storage.rbegin();}

MultiMapImplementation::ReverseIterator MultiMapImplementation::rend() noexcept {return storage.rend();}

MultiMapImplementation::ConstIterator MultiMapImplementation::begin() const noexcept {return cbegin();}

MultiMapImplementation::ConstIterator MultiMapImplementation::end() const noexcept {return cend();}

MultiMapImplementation::ConstReverseIterator MultiMapImplementation::rbegin() const noexcept {return crbegin();}

MultiMapImplementation::ConstReverseIterator MultiMapImplementation::rend() const noexcept {return crend();}

MultiMapImplementation::ConstIterator MultiMapImplementation::cbegin() const noexcept {return storage.cbegin();}

MultiMapImplementation::ConstIterator MultiMapImplementation::cend() const noexcept {return storage.cend();}

MultiMapImplementation::ConstReverseIterator MultiMapImplementation::crbegin() const noexcept {return storage.crbegin();}

MultiMapImplementation::ConstReverseIterator MultiMapImplementation::crend() const noexcept {return storage.crend();}
