#include "libbinom/include/binom_impl/ram_storage_implementation/multi_map_impl.hxx"
#include "libbinom/include/variables/named_variable.hxx"
#include "libbinom/include/utils/util_functions.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;

MultiMapImplementation::MultiMapImplementation(WeakLink owner, const multimap& map) {
  for(auto& element : map)
    insert(owner, std::move(element.getKeyRef()), element.getVariableRef().move());
}

MultiMapImplementation::MultiMapImplementation(WeakLink owner, const MultiMapImplementation& other) {
  for(auto element : other)
    insert(owner, element.getKey().move(), element.getValue());
}

MultiMapImplementation::~MultiMapImplementation() {
  if(table_list) delete table_list;
}

FieldRef MultiMapImplementation::insert(WeakLink owner, KeyValue key, Variable variable, NewNodePosition position) {
  auto insert_result = data.emplace(owner, std::move(key), variable.move());
  if(table_list) {
    // TODO: Add to index in table if has index with equal field key
  }
  return FieldRef(*insert_result);
}

void MultiMapImplementation::remove(Iterator it) {data.erase(it);}

void MultiMapImplementation::remove(ReverseIterator it) {data.erase(ContainerType::reverse_iterator(it).base());}

void MultiMapImplementation::removeAll(KeyValue key) {
//  data.erase(std::move(key)); // C++23 waiting...
  auto [first, last] = data.equal_range(std::move(key));
  data.erase(first, last);
}

err::ProgressReport<FieldRef> MultiMapImplementation::rename(Iterator it, KeyValue new_key) {
  if(auto element = data.extract(it); !element.empty()) {
    element.value().setKey(new_key);
    return FieldRef(*data.insert(std::move(element)));
  } else return ErrorType::binom_out_of_range;
}

std::pair<MultiMapImplementation::Iterator, MultiMapImplementation::Iterator> MultiMapImplementation::getRange(KeyValue key) {
  auto [first, second] = data.equal_range(std::move(key));
  return {first, second};
}

std::pair<MultiMapImplementation::ReverseIterator, MultiMapImplementation::ReverseIterator> MultiMapImplementation::getReverseRange(KeyValue key) {
  auto [first, second] = data.equal_range(std::move(key));
  return {std::multiset<index::Field, index::MapComparator>::const_reverse_iterator(--first), std::multiset<index::Field, index::MapComparator>::const_reverse_iterator(--second)};
}

MultiMapImplementation::Iterator MultiMapImplementation::find(KeyValue key) {return data.find(std::move(key));}

MultiMapImplementation::ReverseIterator MultiMapImplementation::rfind(KeyValue key) {return std::multiset<index::Field, index::MapComparator>::reverse_iterator(data.find(std::move(key)));}

MultiMapImplementation::Iterator MultiMapImplementation::findLast(KeyValue key) {return --data.upper_bound(std::move(key));}

MultiMapImplementation::ReverseIterator MultiMapImplementation::rfindLast(KeyValue key) {return std::multiset<index::Field, index::MapComparator>::reverse_iterator(--data.upper_bound(std::move(key)));}


std::pair<MultiMapImplementation::ConstIterator, MultiMapImplementation::ConstIterator> MultiMapImplementation::getRange(KeyValue key) const {
  auto [first, second] = data.equal_range(std::move(key));
  return {first, second};
}

std::pair<MultiMapImplementation::ConstReverseIterator, MultiMapImplementation::ConstReverseIterator> MultiMapImplementation::getReverseRange(KeyValue key) const {
  auto [first, second] = data.equal_range(std::move(key));
  return {std::multiset<index::Field, index::MapComparator>::const_reverse_iterator(--first), std::multiset<index::Field, index::MapComparator>::const_reverse_iterator(--second)};
}


MultiMapImplementation::ConstIterator MultiMapImplementation::find(KeyValue key) const  {return data.find(std::move(key));}

MultiMapImplementation::ConstReverseIterator MultiMapImplementation::rfind(KeyValue key) const {return std::multiset<index::Field, index::MapComparator>::const_reverse_iterator(data.find(std::move(key)));}

MultiMapImplementation::ConstIterator MultiMapImplementation::findLast(KeyValue key) const {return --data.upper_bound(std::move(key));}

MultiMapImplementation::ConstReverseIterator MultiMapImplementation::rfindLast(KeyValue key) const {return std::multiset<index::Field, index::MapComparator>::const_reverse_iterator(--data.upper_bound(std::move(key)));}

void MultiMapImplementation::clear() {data.clear();}

MultiMapImplementation::Iterator MultiMapImplementation::begin() noexcept {return data.begin();}

MultiMapImplementation::Iterator MultiMapImplementation::end() noexcept {return data.end();}

MultiMapImplementation::ReverseIterator MultiMapImplementation::rbegin() noexcept {return data.rbegin();}

MultiMapImplementation::ReverseIterator MultiMapImplementation::rend() noexcept {return data.rend();}

MultiMapImplementation::ConstIterator MultiMapImplementation::begin() const noexcept {return cbegin();}

MultiMapImplementation::ConstIterator MultiMapImplementation::end() const noexcept {return cend();}

MultiMapImplementation::ConstReverseIterator MultiMapImplementation::rbegin() const noexcept {return crbegin();}

MultiMapImplementation::ConstReverseIterator MultiMapImplementation::rend() const noexcept {return crend();}

MultiMapImplementation::ConstIterator MultiMapImplementation::cbegin() const noexcept {return data.cbegin();}

MultiMapImplementation::ConstIterator MultiMapImplementation::cend() const noexcept {return data.cend();}

MultiMapImplementation::ConstReverseIterator MultiMapImplementation::crbegin() const noexcept {return data.crbegin();}

MultiMapImplementation::ConstReverseIterator MultiMapImplementation::crend() const noexcept {return data.crend();}