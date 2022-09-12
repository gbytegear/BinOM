#include "libbinom/include/binom_impl/ram_storage_implementation/map_impl.hxx"
#include "libbinom/include/variables/named_variable.hxx"
#include "libbinom/include/utils/util_functions.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;

MapImplementation::MapImplementation(const literals::map& map) {
  for(auto& element : map)
    avl_tree.insert(std::move(element.getKey()), std::move(element.getVariable()));
}

MapImplementation::MapImplementation(const MapImplementation& other) {
  // Yeah, that's damn stupid way
  // TODO: Think of a better way
  for (const auto& node : other.avl_tree) avl_tree.insert(node.getKey(), node.getValue());
}

MapImplementation::~MapImplementation() { clear(); }

bool MapImplementation::isEmpty() const noexcept {return avl_tree.isEmpty();}

size_t MapImplementation::getSize() const noexcept {return size;}

bool MapImplementation::contains(KeyValue value) const {return avl_tree.get(value);}

err::ProgressReport<MapImplementation::NamedVariable> MapImplementation::insert(KeyValue key, Variable variable) {
  if(auto avl_node = avl_tree.insert(std::move(key), variable.move()); avl_node) {
    ++size;
    return NamedVariable(avl_node);
  } else return err::ErrorType::binom_key_unique_error;
}

err::Error MapImplementation::remove(KeyValue key) {
  if(auto avl_node = avl_tree.extract(std::move(key)); avl_node) {
    delete avl_node;
    --size;
    return err::ErrorType::no_error;
  } else return err::ErrorType::binom_out_of_range;
}

err::ProgressReport<MapImplementation::NamedVariable> MapImplementation::rename(KeyValue old_key, KeyValue new_key) {
  if(auto avl_node = avl_tree.extract(std::move(old_key)); avl_node) {
    if(avl_tree.insert(*avl_node)) return NamedVariable(avl_node);
    return err::ProgressReport(err::ErrorType::binom_key_unique_error, NamedVariable(avl_node));
  } else return err::ErrorType::binom_out_of_range;
}

MapImplementation::NamedVariable MapImplementation::getOrInsertNamedVariable(KeyValue key) {
  auto node = avl_tree.get(key);
  if(!node) return avl_tree.insert(key, nullptr);
  NamedVariable a = *node;
  return *node;
}

Variable MapImplementation::getVariable(KeyValue key) {
  auto node = avl_tree.get(std::move(key));
  if(!node) return nullptr;
  return node->getValue().move();
}

void MapImplementation::clear() { avl_tree.clear(); }

MapImplementation::Iterator MapImplementation::find(KeyValue key) { return Iterator(avl_tree.get(std::move(key))); }

MapImplementation::ReverseIterator MapImplementation::rfind(KeyValue key) { return ReverseIterator(avl_tree.get(std::move(key))); }

MapImplementation::ConstIterator MapImplementation::find(KeyValue key) const { return ConstIterator(avl_tree.get(std::move(key))); }

MapImplementation::ConstReverseIterator MapImplementation::rfind(KeyValue key) const { return ConstReverseIterator(avl_tree.get(std::move(key))); }

MapImplementation::Iterator::Iterator(VariableAVLTree::Iterator iterator) : iterator(std::move(iterator)) {}

MapImplementation::Iterator::Iterator(const Iterator& iterator) : iterator(std::move(iterator.iterator)) {}

MapImplementation::Iterator::Iterator(Iterator&& iterator) : iterator(std::move(iterator.iterator)) {}

MapImplementation::Iterator& MapImplementation::Iterator::operator=(Iterator& other) noexcept {return *new(this) Iterator(other);}

MapImplementation::Iterator& MapImplementation::Iterator::operator=(Iterator&& other) noexcept {return *new(this) Iterator(std::move(other));}

MapImplementation::Iterator& MapImplementation::Iterator::operator++() { ++iterator; return self; }

MapImplementation::Iterator& MapImplementation::Iterator::operator--() { --iterator; return self; }

MapImplementation::Iterator MapImplementation::Iterator::operator++(int) {Iterator tmp(self); ++self; return tmp;}

MapImplementation::Iterator MapImplementation::Iterator::operator--(int) {Iterator tmp(self); --self; return tmp;}

const MapImplementation::Iterator& MapImplementation::Iterator::operator++() const { ++iterator; return self; }

const MapImplementation::Iterator& MapImplementation::Iterator::operator--() const { --iterator; return self; }

const MapImplementation::Iterator MapImplementation::Iterator::operator++(int) const {Iterator tmp(self); ++self; return tmp;}

const MapImplementation::Iterator MapImplementation::Iterator::operator--(int) const {Iterator tmp(self); --self; return tmp;}

bool MapImplementation::Iterator::operator==(const Iterator other) const noexcept {return iterator == other.iterator;}

bool MapImplementation::Iterator::operator!=(const Iterator other) const noexcept {return iterator != other.iterator;}

MapImplementation::NamedVariable MapImplementation::Iterator::operator*() {return *iterator;}

pseudo_ptr::PseudoPointer<MapImplementation::NamedVariable> MapImplementation::Iterator::operator->() {return {NamedVariable(*iterator)};}

const MapImplementation::NamedVariable MapImplementation::Iterator::operator*() const {return *iterator;}

const pseudo_ptr::PseudoPointer<MapImplementation::NamedVariable> MapImplementation::Iterator::operator->() const {return {NamedVariable(*iterator)};}




MapImplementation::ReverseIterator::ReverseIterator(VariableAVLTree::ReverseIterator iterator) : iterator(std::move(iterator)) {}

MapImplementation::ReverseIterator::ReverseIterator(const ReverseIterator& iterator) : iterator(std::move(iterator.iterator)) {}

MapImplementation::ReverseIterator::ReverseIterator(ReverseIterator&& iterator) : iterator(std::move(iterator.iterator)) {}

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator=(ReverseIterator& other) noexcept {return *new(this) ReverseIterator(other);}

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator=(ReverseIterator&& other) noexcept {return *new(this) ReverseIterator(std::move(other));}

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator++() { ++iterator; return self; }

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator--() { --iterator; return self; }

MapImplementation::ReverseIterator MapImplementation::ReverseIterator::operator++(int) {ReverseIterator tmp(self); ++self; return tmp;}

MapImplementation::ReverseIterator MapImplementation::ReverseIterator::operator--(int) {ReverseIterator tmp(self); --self; return tmp;}

const MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator++() const { ++iterator; return self; }

const MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator--() const { --iterator; return self; }

const MapImplementation::ReverseIterator MapImplementation::ReverseIterator::operator++(int) const {ReverseIterator tmp(self); ++self; return tmp;}

const MapImplementation::ReverseIterator MapImplementation::ReverseIterator::operator--(int) const {ReverseIterator tmp(self); --self; return tmp;}

bool MapImplementation::ReverseIterator::operator==(const ReverseIterator other) const noexcept {return iterator == other.iterator;}

bool MapImplementation::ReverseIterator::operator!=(const ReverseIterator other) const noexcept {return iterator != other.iterator;}

MapImplementation::NamedVariable MapImplementation::ReverseIterator::operator*() {return *iterator;}

pseudo_ptr::PseudoPointer<MapImplementation::NamedVariable> MapImplementation::ReverseIterator::operator->() {return {NamedVariable(*iterator)};}

const MapImplementation::NamedVariable MapImplementation::ReverseIterator::operator*() const {return *iterator;}

const pseudo_ptr::PseudoPointer<MapImplementation::NamedVariable> MapImplementation::ReverseIterator::operator->() const {return {NamedVariable(*iterator)};}
