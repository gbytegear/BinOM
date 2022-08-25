#include "libbinom/include/binom_impl/ram_storage_implementation/map_impl.hxx"
#include "libbinom/include/variables/named_variable.hxx"
#include "libbinom/include/utils/util_functions.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;

MapImplementation::NamedVariable* MapImplementation::convert(AVLNode* node) {
  return reinterpret_cast<NamedVariable*>(
        reinterpret_cast<byte*>(node) + offsetof(NamedVariable, node)
                                );
}

const MapImplementation::NamedVariable* MapImplementation::convert(const AVLNode* node) {
  return reinterpret_cast<const NamedVariable*>(
        reinterpret_cast<const byte*>(node) + offsetof(NamedVariable, node)
        );
}

MapImplementation::MapImplementation(const literals::map& map) {
  for(auto& element : map) {
    NamedVariable* named_variable = new NamedVariable(std::move(element));
    avl_tree.insert(&named_variable->node);
  }
}

MapImplementation::MapImplementation(const MapImplementation& other) {
  // Yeah, that's damn stupid way
  // TODO: Think of a better way
  for (const AVLNode& node : other.avl_tree) {
    const NamedVariable& other_named_variable = *convert(&node);
    NamedVariable* named_variable = new NamedVariable(other_named_variable.getKey(),
                                                      other_named_variable.getVariable());
    avl_tree.insert(&named_variable->node);
  }
}

MapImplementation::~MapImplementation() { clear(); }

bool MapImplementation::isEmpty() const noexcept {return avl_tree.isEmpty();}

size_t MapImplementation::getSize() const noexcept {return size;}

bool MapImplementation::contains(KeyValue value) const {return avl_tree.get(value);}

err::ProgressReport<MapImplementation::NamedVariable> MapImplementation::insert(KeyValue key, Variable variable) {
  NamedVariable* named_variable = new NamedVariable(std::move(key), variable.move());
  if(!avl_tree.insert(&named_variable->node)) return err::ErrorType::binom_key_unique_error;
  ++size;
  return named_variable;
}

err::Error MapImplementation::remove(KeyValue key) {
  NamedVariable* named_variable = convert(avl_tree.extract(std::move(key)));
  if(!named_variable) return err::ErrorType::binom_out_of_range;
  delete named_variable;
  --size;
  return err::ErrorType::no_error;
}

err::ProgressReport<MapImplementation::NamedVariable> MapImplementation::rename(KeyValue old_key, KeyValue new_key) {
  NamedVariable* named_variable = convert(avl_tree.extract(old_key));
  if(!named_variable) return err::ErrorType::binom_out_of_range;
  named_variable->node.getKey() = std::move(new_key);
  if(!avl_tree.insert(&named_variable->node)) {
    named_variable->node.getKey() = std::move(old_key);
    avl_tree.insert(&named_variable->node);
    return err::ProgressReport(err::ErrorType::binom_key_unique_error, named_variable);
  }
  return named_variable;
}

MapImplementation::NamedVariable& MapImplementation::getOrInsertNamedVariable(KeyValue key) {
  NamedVariable* named_variable = convert(avl_tree.get(key));
  if(!named_variable) return *insert(std::move(key), nullptr);
  return *named_variable;
}

Variable MapImplementation::getVariable(KeyValue key) {
  NamedVariable* named_variable = convert(avl_tree.get(key));
  if(!named_variable) return nullptr;
  return named_variable->getVariable();
}

void MapImplementation::clear() {
  avl_tree.clear([](AVLNode* node) { delete convert(node); });
}

MapImplementation::Iterator::Iterator(AVLTree::Iterator iterator) : iterator(std::move(iterator)) {}

MapImplementation::Iterator::Iterator(const Iterator& iterator) : iterator(std::move(iterator.iterator)) {}

MapImplementation::Iterator::Iterator(Iterator&& iterator) : iterator(std::move(iterator.iterator)) {}

MapImplementation::Iterator& MapImplementation::Iterator::operator=(Iterator& other) noexcept {return *new(this) Iterator(other);}

MapImplementation::Iterator& MapImplementation::Iterator::operator=(Iterator&& other) noexcept {return *new(this) Iterator(std::move(other));}

MapImplementation::Iterator& MapImplementation::Iterator::operator=(AVLNode* node) noexcept {return *new(this) Iterator(node);}

MapImplementation::Iterator& MapImplementation::Iterator::operator=(AVLNode& node) noexcept {return *new(this) Iterator(&node);}

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

MapImplementation::NamedVariable& MapImplementation::Iterator::operator*() {return *convert(&*iterator);}

MapImplementation::NamedVariable* MapImplementation::Iterator::operator->() {return convert(&*iterator);}

const MapImplementation::NamedVariable& MapImplementation::Iterator::operator*() const {return *convert(&*iterator);}

const MapImplementation::NamedVariable* MapImplementation::Iterator::operator->() const {return convert(&*iterator);}




MapImplementation::ReverseIterator::ReverseIterator(AVLTree::ReverseIterator iterator) : iterator(std::move(iterator)) {}

MapImplementation::ReverseIterator::ReverseIterator(const ReverseIterator& iterator) : iterator(std::move(iterator.iterator)) {}

MapImplementation::ReverseIterator::ReverseIterator(ReverseIterator&& iterator) : iterator(std::move(iterator.iterator)) {}

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator=(ReverseIterator& other) noexcept {return *new(this) ReverseIterator(other);}

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator=(ReverseIterator&& other) noexcept {return *new(this) ReverseIterator(std::move(other));}

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator=(AVLNode* node) noexcept {return *new(this) ReverseIterator(node);}

MapImplementation::ReverseIterator& MapImplementation::ReverseIterator::operator=(AVLNode& node) noexcept {return *new(this) ReverseIterator(&node);}

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

MapImplementation::NamedVariable& MapImplementation::ReverseIterator::operator*() {return *convert(&*iterator);}

MapImplementation::NamedVariable* MapImplementation::ReverseIterator::operator->() {return convert(&*iterator);}

const MapImplementation::NamedVariable& MapImplementation::ReverseIterator::operator*() const {return *convert(&*iterator);}

const MapImplementation::NamedVariable* MapImplementation::ReverseIterator::operator->() const {return convert(&*iterator);}
