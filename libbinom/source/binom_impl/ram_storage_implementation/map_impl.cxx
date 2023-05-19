#include "libbinom/include/binom_impl/ram_storage_implementation/map_impl.hxx"
#include "libbinom/include/binom_impl/ram_storage_implementation/table_impl.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;

MapImplementation::MapImplementation(WeakLink owner, const map& map) {
  for(auto& element : map)
    insert(owner, std::move(element.getKey()), element.getValue().move());
}

MapImplementation::MapImplementation(WeakLink owner, const MapImplementation& other) {
  for(auto element : other) insert(owner, element.getKey(), element.getValue());
}

MapImplementation::MapImplementation(WeakLink owner, MapImplementation&& other) {
  for(auto element : other) insert(owner, element.getKey(), element.getValue());
}

MapImplementation::~MapImplementation() {
  if(table_list) { delete table_list; table_list = nullptr; }
}

err::ProgressReport<FieldRef> MapImplementation::insert(WeakLink owner, KeyValue key, Variable variable) {
  auto insert_result = data.emplace(owner, key, variable.move());
  if(insert_result.second) {
    updateKey(const_cast<binom::index::Field&>(*insert_result.first), key);
    return FieldRef(*insert_result.first);
  }
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

void MapImplementation::addTable(TableImplementation& table) {
  if(!table_list) table_list = new std::set<TableImplementation*>();
  table_list->insert(&table);
}

Error binom::priv::MapImplementation::removeTable(TableImplementation& table) {
  if(!table_list) return ErrorType::out_of_range;
  if(table_list->erase(&table) == 0) return ErrorType::out_of_range;
  if(table_list->empty()) { delete table_list; table_list = nullptr; }
  return Error{};
}

bool MapImplementation::updateKey(index::Field& field, const KeyValue& new_key) {
  if(!table_list) return false;
  bool flag = false;
  for(auto table_ptr : *table_list)
    if(auto column = (*table_ptr)[new_key]; column) {
      auto lk = column->getLock(MtxLockType::unique_locked);
      column->add(field);
      flag = true;
    }
  return flag;
}
