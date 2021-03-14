#include "binom/includes/structure/path.h"

using namespace binom;

PathNode::PathNode(ui64 index)
  : _type(PathNodeType::index),
    value(index)
{}

PathNode::PathNode(BufferArray name)
  : _type(PathNodeType::name),
    value(std::move(name))
{}

PathNode::PathNode(const char* name)
  : _type(PathNodeType::name),
    value(BufferArray(name))
{}

PathNode::PathNode(int index)
  : _type(PathNodeType::index),
    value(ui64(index))
{}

PathNode::PathNode(const PathNode& other)
  : _type(other._type),
    value(_type, other.value) {
  iterator it = begin();
  if(other.next)
    for(const_iterator path_node_it = ++other.cbegin();
        path_node_it != other.cend();
        (++path_node_it, ++it)) {
      it->next = new PathNode(*path_node_it);
    }
}

PathNode::PathNode(PathNode&& other)
  : next(other.next),
    _type(other._type),
    value(_type, std::move(other.value)) {other.next = nullptr;}

PathNode::PathNode(std::initializer_list<PathNode> path)
  : PathNode(*path.begin()) {
  iterator it = begin();
  for(const PathNode& path_node : path) {
    if(&path_node == path.begin()) continue;
    it->next = new PathNode(std::move(path_node));
    ++it;
  }
}

PathNode::~PathNode() {
  if(_type == PathNodeType::name &&
     !value.name.asVar().isNull())
    value.name.~BufferArray();
  if(next)
    delete next;
}

PathNodeType PathNode::type() const {return _type;}

ui8 PathNode::index() const {return value.index;}

BufferArray PathNode::name() const {return value.name;}

PathNode::iterator PathNode::begin() {return this;}

PathNode::iterator PathNode::end() {return nullptr;}

PathNode::const_iterator PathNode::cbegin() const {return this;}

PathNode::const_iterator PathNode::cend() const {return nullptr;}

PathNode::PathNodeValue::~PathNodeValue() {}

PathNode::PathNodeValue::PathNodeValue(ui8 index)
  : index(index) {}

PathNode::PathNodeValue::PathNodeValue(BufferArray name)
  : name(std::move(name)) {}

PathNode::PathNodeValue::PathNodeValue(PathNodeType type,
                                       const PathNode::PathNodeValue& value) {
  switch (type) {
    case PathNodeType::index: index = value.index; return;
    case PathNodeType::name: new(&name) BufferArray(value.name); return;
  }
}

PathNode::PathNodeValue::PathNodeValue(PathNodeType type,
                                       PathNode::PathNodeValue&& value) {
  switch (type) {
    case PathNodeType::index: index = value.index; return;
    case PathNodeType::name: new(&name) BufferArray(std::move(value.name)); return;
  }
}

PathNode::iterator::iterator(PathNode* current)
  : current(current) {}

PathNode::iterator::iterator(PathNode::iterator& other)
  : current(other.current) {}

PathNode::iterator::iterator(PathNode::iterator&& other)
  : current(other.current) {}

PathNode& PathNode::iterator::operator*() {return *current;}

PathNode* PathNode::iterator::operator->() {return current;}

PathNode::iterator& PathNode::iterator::operator++() {current = current->next; return *this;}

PathNode::iterator PathNode::iterator::operator++(int) {iterator last(current); current = current->next;return last;}

bool PathNode::iterator::operator==(PathNode::iterator other) {return current == other.current;}

bool PathNode::iterator::operator!=(PathNode::iterator other) {return current != other.current;}

PathNode::const_iterator::const_iterator(const PathNode* current)
  : current(current) {}

PathNode::const_iterator::const_iterator(PathNode::const_iterator& other)
  : current(other.current) {}

PathNode::const_iterator::const_iterator(PathNode::const_iterator&& other)
  : current(other.current) {}

const PathNode& PathNode::const_iterator::operator*() {return *current;}

const PathNode* PathNode::const_iterator::operator->() {return current;}

PathNode::const_iterator& PathNode::const_iterator::operator++() {current = current->next; return *this;}

PathNode::const_iterator PathNode::const_iterator::operator++(int) {const_iterator last(current); current = current->next;return last;}

bool PathNode::const_iterator::operator==(PathNode::const_iterator other) {return current == other.current;}

bool PathNode::const_iterator::operator!=(PathNode::const_iterator other) {return current != other.current;}
