#include "libbinom/include/binom_impl/ram_storage_implementation/multi_map_impl.hxx"
#include "libbinom/include/variables/named_variable.hxx"
#include "libbinom/include/utils/util_functions.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;



MultiMapImplementation::MultiMapNode* MultiMapImplementation::convert(MultiAVLTree::AVLNode* node) {
  return reinterpret_cast<MultiMapNode*>(reinterpret_cast<byte*>(node) - offsetof(MultiMapNode, node));
}

const MultiMapImplementation::MultiMapNode* MultiMapImplementation::convert(const MultiAVLTree::AVLNode* node) {
  return reinterpret_cast<const MultiMapNode*>(reinterpret_cast<const byte*>(node) - offsetof(MultiMapNode, node));
}

MultiMapImplementation::MultiMapImplementation(const literals::multimap& map) {

}

MultiMapImplementation::MultiMapImplementation(const MultiMapImplementation& other) {

}

MultiMapImplementation::~MultiMapImplementation() {

}

MultiMapImplementation::NamedVariable MultiMapImplementation::insert(KeyValue key, Variable variable, NewNodePosition position) {
  MultiMapNode* mm_node = new MultiMapNode{MultiAVLTree::AVLNode(), variable.move()};
  auto node_pair = multi_avl_tree.insert(std::move(key), &mm_node->node, position);
  ++size;
  return NamedVariable(node_pair.key_node->getKey(), convert(node_pair.node));
}

void MultiMapImplementation::remove(Iterator it) {
  delete convert(multi_avl_tree.extract(it.iterator));
}

void MultiMapImplementation::remove(ReverseIterator it) {
  delete convert(multi_avl_tree.extract(it.iterator));
}

err::ProgressReport<MultiMapImplementation::NamedVariable> MultiMapImplementation::rename(Iterator it, KeyValue new_key) {

}








MultiMapImplementation::Iterator::Iterator(MultiAVLTree::Iterator iterator) : iterator(std::move(iterator)) {}

MultiMapImplementation::Iterator::Iterator(const Iterator& iterator) : iterator(std::move(iterator.iterator)) {}

MultiMapImplementation::Iterator::Iterator(Iterator&& iterator) : iterator(std::move(iterator.iterator)) {}

MultiMapImplementation::Iterator& MultiMapImplementation::Iterator::operator=(Iterator& other) noexcept {return *new(this) Iterator(other);}

MultiMapImplementation::Iterator& MultiMapImplementation::Iterator::operator=(Iterator&& other) noexcept {return *new(this) Iterator(std::move(other));}

MultiMapImplementation::Iterator& MultiMapImplementation::Iterator::operator++() { ++iterator; return self; }

MultiMapImplementation::Iterator& MultiMapImplementation::Iterator::operator--() { --iterator; return self; }

MultiMapImplementation::Iterator MultiMapImplementation::Iterator::operator++(int) {Iterator tmp(self); ++self; return tmp;}

MultiMapImplementation::Iterator MultiMapImplementation::Iterator::operator--(int) {Iterator tmp(self); --self; return tmp;}

const MultiMapImplementation::Iterator& MultiMapImplementation::Iterator::operator++() const { ++iterator; return self; }

const MultiMapImplementation::Iterator& MultiMapImplementation::Iterator::operator--() const { --iterator; return self; }

const MultiMapImplementation::Iterator MultiMapImplementation::Iterator::operator++(int) const {Iterator tmp(self); ++self; return tmp;}

const MultiMapImplementation::Iterator MultiMapImplementation::Iterator::operator--(int) const {Iterator tmp(self); --self; return tmp;}

bool MultiMapImplementation::Iterator::operator==(const Iterator other) const noexcept {return iterator == other.iterator;}

bool MultiMapImplementation::Iterator::operator!=(const Iterator other) const noexcept {return iterator != other.iterator;}

MultiMapImplementation::NamedVariable MultiMapImplementation::Iterator::operator*() {return NamedVariable(iterator.getKey(), convert(&*iterator)->variable.move());}

MultiMapImplementation::PseudoPointer<MultiMapImplementation::NamedVariable> MultiMapImplementation::Iterator::operator ->() {return {NamedVariable(iterator.getKey(), convert(&*iterator)->variable.move())};}

const MultiMapImplementation::NamedVariable MultiMapImplementation::Iterator::operator*() const {return NamedVariable(iterator.getKey(), convert(&*iterator)->variable.move());}

const MultiMapImplementation::PseudoPointer<MultiMapImplementation::NamedVariable> MultiMapImplementation::Iterator::operator->() const {return {NamedVariable(iterator.getKey(), convert(&*iterator)->variable.move())};}




MultiMapImplementation::ReverseIterator::ReverseIterator(MultiAVLTree::ReverseIterator iterator) : iterator(std::move(iterator)) {}

MultiMapImplementation::ReverseIterator::ReverseIterator(const ReverseIterator& iterator) : iterator(std::move(iterator.iterator)) {}

MultiMapImplementation::ReverseIterator::ReverseIterator(ReverseIterator&& iterator) : iterator(std::move(iterator.iterator)) {}

MultiMapImplementation::ReverseIterator& MultiMapImplementation::ReverseIterator::operator=(ReverseIterator& other) noexcept {return *new(this) ReverseIterator(other);}

MultiMapImplementation::ReverseIterator& MultiMapImplementation::ReverseIterator::operator=(ReverseIterator&& other) noexcept {return *new(this) ReverseIterator(std::move(other));}

MultiMapImplementation::ReverseIterator& MultiMapImplementation::ReverseIterator::operator++() { ++iterator; return self; }

MultiMapImplementation::ReverseIterator& MultiMapImplementation::ReverseIterator::operator--() { --iterator; return self; }

MultiMapImplementation::ReverseIterator MultiMapImplementation::ReverseIterator::operator++(int) {ReverseIterator tmp(self); ++self; return tmp;}

MultiMapImplementation::ReverseIterator MultiMapImplementation::ReverseIterator::operator--(int) {ReverseIterator tmp(self); --self; return tmp;}

const MultiMapImplementation::ReverseIterator& MultiMapImplementation::ReverseIterator::operator++() const { ++iterator; return self; }

const MultiMapImplementation::ReverseIterator& MultiMapImplementation::ReverseIterator::operator--() const { --iterator; return self; }

const MultiMapImplementation::ReverseIterator MultiMapImplementation::ReverseIterator::operator++(int) const {ReverseIterator tmp(self); ++self; return tmp;}

const MultiMapImplementation::ReverseIterator MultiMapImplementation::ReverseIterator::operator--(int) const {ReverseIterator tmp(self); --self; return tmp;}

bool MultiMapImplementation::ReverseIterator::operator==(const ReverseIterator other) const noexcept {return iterator == other.iterator;}

bool MultiMapImplementation::ReverseIterator::operator!=(const ReverseIterator other) const noexcept {return iterator != other.iterator;}

MultiMapImplementation::NamedVariable MultiMapImplementation::ReverseIterator::operator*() {return NamedVariable(iterator.getKey(), convert(&*iterator)->variable.move());}

MultiMapImplementation::PseudoPointer<MultiMapImplementation::NamedVariable> MultiMapImplementation::ReverseIterator::operator->() {return {NamedVariable(iterator.getKey(), convert(&*iterator)->variable.move())};}

const MultiMapImplementation::NamedVariable MultiMapImplementation::ReverseIterator::operator*() const {return NamedVariable(iterator.getKey(), convert(&*iterator)->variable.move());}

const MultiMapImplementation::PseudoPointer<MultiMapImplementation::NamedVariable> MultiMapImplementation::ReverseIterator::operator->() const {return {NamedVariable(iterator.getKey(), convert(&*iterator)->variable.move())};}

