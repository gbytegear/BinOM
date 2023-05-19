#include "libbinom/include/binom_impl/multi_avl_tree.hxx"

using namespace binom;
using namespace binom::priv;

// ======================================================== MultiAVLTree::AVLKeyNode

void MultiAVLTree::AVLKeyNode::swapPosition(AVLKeyNode& other, MultiAVLTree& avl_tree) {
  auto this_position = getPosition();
  auto other_position = other.getPosition();

  if(parent != &other && other.parent != this) {
    std::swap(parent, other.parent);
    std::swap(left, other.left);
    std::swap(right, other.right);
    std::swap(depth, other.depth);

  } elif (parent == &other) {
    parent = other.parent;
    other.parent = this;
    if(other.left == this) {
      other.left = left;
      left = &other;
      std::swap(right, other.right);
    } else {
      other.right = right;
      right = &other;
      std::swap(left, other.left);
    }
    std::swap(depth, other.depth);

  } else {
    other.parent = parent;
    parent = &other;
    if(left == &other) {
      left = other.left;
      other.left = this;
      std::swap(right, other.right);
    } else {
      right = other.right;
      other.right = this;
      std::swap(left, other.left);
    }
  }

  if(hasLeft()) left->parent = this;
  if(hasRight()) right->parent = this;
  if(other.hasLeft()) other.left->parent = &other;
  if(other.hasRight()) other.right->parent = &other;

  switch (other_position) {
  case NodePosition::left: parent->left = this; break;
  case NodePosition::right: parent->right = this; break;
  case NodePosition::root: avl_tree.root = this; break;
  }

  switch (this_position) {
  case NodePosition::left: other.parent->left = &other; break;
  case NodePosition::right: other.parent->right = &other; break;
  case NodePosition::root: avl_tree.root = &other; break;
  }
}

void MultiAVLTree::AVLKeyNode::unpin(MultiAVLTree& avl_tree) {
  if(isLeft()) parent->left = nullptr;
  elif(isRight()) parent->right = nullptr;
  else avl_tree.root = nullptr;
  parent = nullptr;
}

MultiAVLTree::AVLKeyNode::AVLKeyNode(KeyValue key, AVLKeyNode* parent)
  : parent(parent), key(std::move(key)) {}

MultiAVLTree::AVLKeyNode::AVLKeyNode(AVLKeyNode& other)
  : depth(other.depth),
    left(other.left),
    right(other.right),
    parent(other.parent),
    key(other.key) {}

MultiAVLTree::AVLKeyNode::AVLKeyNode(AVLKeyNode&& other)
  : depth(other.depth),
    left(other.left),
    right(other.right),
    parent(other.parent),
    key(std::move(other.key)) {}

MultiAVLTree::AVLKeyNode& MultiAVLTree::AVLKeyNode::operator=(AVLKeyNode other) { this->~AVLKeyNode(); return *new(this) AVLKeyNode(std::move(other)); }

void MultiAVLTree::AVLKeyNode::pushBack(AVLNode* node) {
  node->key_node = this;
  if(!first) {
    first = last = node;
    node->next = node->prev = nullptr;
    ++size;
    return;
  }
  node->prev = last;
  last = last->next = node;
  ++size;
}

void MultiAVLTree::AVLKeyNode::pushFront(AVLNode* node) {
  node->key_node = this;
  if(!first) {
    first = last = node;
    node->next = node->prev = nullptr;
    ++size;
    return;
  }
  node->next = first;
  first = first->prev = node;
  ++size;
}

MultiAVLTree::AVLNode& MultiAVLTree::AVLKeyNode::extract(Iterator it) {
  AVLNode& node = *it;
  if(node.next) {node.next->prev = node.prev;}
  else if(&node == last) {
    last = node.prev;
  }
  if(node.prev) {node.prev->next = node.next;}
  else if(&node == first) {
    first = node.next;
  }
  node.key_node = nullptr;
  node.next = nullptr;
  node.prev = nullptr;
  --size;
  return node;
}

size_t MultiAVLTree::AVLKeyNode::getElementCount() const noexcept {return size;}

bool MultiAVLTree::AVLKeyNode::isEmpty() const noexcept {return !size;}

bool MultiAVLTree::AVLKeyNode::isRoot() const noexcept {return !parent;}

bool MultiAVLTree::AVLKeyNode::isLeft() const noexcept {return isRoot() ? false : parent->left == this;}

bool MultiAVLTree::AVLKeyNode::isRight() const noexcept {return isRoot() ? false : parent->right == this;}

bool MultiAVLTree::AVLKeyNode::hasLeft() const noexcept {return left;}

bool MultiAVLTree::AVLKeyNode::hasRight() const noexcept {return right;}

bool MultiAVLTree::AVLKeyNode::hasChild() const noexcept {return left || right;}

KeyValue& MultiAVLTree::AVLKeyNode::getKey() {return key;}

const KeyValue& MultiAVLTree::AVLKeyNode::getKey() const {return key;}

MultiAVLTree::AVLKeyNode::NodePosition MultiAVLTree::AVLKeyNode::getPosition() const {
  if(isLeft()) return NodePosition::left;
  elif(isRight()) return NodePosition::right;
  else return NodePosition::root;
}

MultiAVLTree::AVLKeyNode::Iterator MultiAVLTree::AVLKeyNode::begin() noexcept {return first;}

MultiAVLTree::AVLKeyNode::Iterator MultiAVLTree::AVLKeyNode::end() noexcept {return nullptr;}

MultiAVLTree::AVLKeyNode::ConstIterator MultiAVLTree::AVLKeyNode::begin() const noexcept {return first;}

MultiAVLTree::AVLKeyNode::ConstIterator MultiAVLTree::AVLKeyNode::end() const noexcept {return nullptr;}

MultiAVLTree::AVLKeyNode::ConstIterator MultiAVLTree::AVLKeyNode::cbegin() const noexcept {return first;}

MultiAVLTree::AVLKeyNode::ConstIterator MultiAVLTree::AVLKeyNode::cend() const noexcept {return nullptr;}

MultiAVLTree::AVLKeyNode::ReverseIterator MultiAVLTree::AVLKeyNode::rbegin() noexcept {return last;}

MultiAVLTree::AVLKeyNode::ReverseIterator MultiAVLTree::AVLKeyNode::rend() noexcept {return nullptr;}

MultiAVLTree::AVLKeyNode::ConstReverseIterator MultiAVLTree::AVLKeyNode::rbegin() const noexcept {return last;}

MultiAVLTree::AVLKeyNode::ConstReverseIterator MultiAVLTree::AVLKeyNode::rend() const noexcept {return nullptr;}

MultiAVLTree::AVLKeyNode::ConstReverseIterator MultiAVLTree::AVLKeyNode::crbegin() const noexcept {return last;}

MultiAVLTree::AVLKeyNode::ConstReverseIterator MultiAVLTree::AVLKeyNode::crend() const noexcept {return nullptr;}



// ======================================================== MultiAVLTree::AVLKeyNode::Iterator

MultiAVLTree::AVLKeyNode::Iterator::Iterator(AVLNode* node) noexcept : node(node) {}

MultiAVLTree::AVLKeyNode::Iterator::Iterator(const Iterator& other) noexcept : node(other.node) {}

MultiAVLTree::AVLKeyNode::Iterator::Iterator(Iterator&& other) noexcept : node(other.node) {}

MultiAVLTree::AVLKeyNode::Iterator& MultiAVLTree::AVLKeyNode::Iterator::operator=(Iterator other) {node = other.node; return self;}

MultiAVLTree::AVLKeyNode::Iterator& MultiAVLTree::AVLKeyNode::Iterator::operator++() {if(node) node = node->next; return self;}

MultiAVLTree::AVLKeyNode::Iterator MultiAVLTree::AVLKeyNode::Iterator::operator++(int) {Iterator tmp(self); ++self; return tmp;}

MultiAVLTree::AVLKeyNode::Iterator& MultiAVLTree::AVLKeyNode::Iterator::operator--() {if(node) node = node->prev; return self;}

MultiAVLTree::AVLKeyNode::Iterator MultiAVLTree::AVLKeyNode::Iterator::operator--(int) {Iterator tmp(self); --self; return tmp;}

const MultiAVLTree::AVLKeyNode::Iterator& MultiAVLTree::AVLKeyNode::Iterator::operator++() const {if(node) node = node->next; return self;}

const MultiAVLTree::AVLKeyNode::Iterator MultiAVLTree::AVLKeyNode::Iterator::operator++(int) const {Iterator tmp(self); ++self; return tmp;}

const MultiAVLTree::AVLKeyNode::Iterator& MultiAVLTree::AVLKeyNode::Iterator::operator--() const {if(node) node = node->prev; return self;}

const MultiAVLTree::AVLKeyNode::Iterator MultiAVLTree::AVLKeyNode::Iterator::operator--(int) const {Iterator tmp(self); --self; return tmp;}

bool MultiAVLTree::AVLKeyNode::Iterator::operator==(Iterator other) const noexcept {return node == other.node;}

bool MultiAVLTree::AVLKeyNode::Iterator::operator!=(Iterator other) const noexcept {return node != other.node;}

MultiAVLTree::AVLNode& MultiAVLTree::AVLKeyNode::Iterator::operator*() {return *node;}

MultiAVLTree::AVLNode* MultiAVLTree::AVLKeyNode::Iterator::operator->() noexcept {return node;}

const MultiAVLTree::AVLNode& MultiAVLTree::AVLKeyNode::Iterator::operator*() const {return *node;}

const MultiAVLTree::AVLNode* MultiAVLTree::AVLKeyNode::Iterator::operator->() const noexcept {return node;}

MultiAVLTree::AVLKeyNode::Iterator::operator ReverseIterator() noexcept {return node;}

MultiAVLTree::AVLKeyNode::Iterator::operator const ReverseIterator() const noexcept {return node;}


// ======================================================== MultiAVLTree::AVLKeyNode::ReverseIterator

MultiAVLTree::AVLKeyNode::ReverseIterator::ReverseIterator(AVLNode* node) noexcept : node(node) {}

MultiAVLTree::AVLKeyNode::ReverseIterator::ReverseIterator(const ReverseIterator& other) noexcept : node(other.node) {}

MultiAVLTree::AVLKeyNode::ReverseIterator::ReverseIterator(ReverseIterator&& other) noexcept : node(other.node) {}

MultiAVLTree::AVLKeyNode::ReverseIterator& MultiAVLTree::AVLKeyNode::ReverseIterator::operator=(ReverseIterator other) {node = other.node; return self;}

MultiAVLTree::AVLKeyNode::ReverseIterator& MultiAVLTree::AVLKeyNode::ReverseIterator::operator++() {if(node) node = node->prev; return self;}

MultiAVLTree::AVLKeyNode::ReverseIterator MultiAVLTree::AVLKeyNode::ReverseIterator::operator++(int) {ReverseIterator tmp(self); ++self; return tmp;}

MultiAVLTree::AVLKeyNode::ReverseIterator& MultiAVLTree::AVLKeyNode::ReverseIterator::operator--() {if(node) node = node->next; return self;}

MultiAVLTree::AVLKeyNode::ReverseIterator MultiAVLTree::AVLKeyNode::ReverseIterator::operator--(int) {ReverseIterator tmp(self); --self; return tmp;}

const MultiAVLTree::AVLKeyNode::ReverseIterator& MultiAVLTree::AVLKeyNode::ReverseIterator::operator++() const {if(node) node = node->prev; return self;}

const MultiAVLTree::AVLKeyNode::ReverseIterator MultiAVLTree::AVLKeyNode::ReverseIterator::operator++(int) const {ReverseIterator tmp(self); ++self; return tmp;}

const MultiAVLTree::AVLKeyNode::ReverseIterator& MultiAVLTree::AVLKeyNode::ReverseIterator::operator--() const {if(node) node = node->next; return self;}

const MultiAVLTree::AVLKeyNode::ReverseIterator MultiAVLTree::AVLKeyNode::ReverseIterator::operator--(int) const {ReverseIterator tmp(self); --self; return tmp;}

bool MultiAVLTree::AVLKeyNode::ReverseIterator::operator==(ReverseIterator other) const noexcept {return node == other.node;}

bool MultiAVLTree::AVLKeyNode::ReverseIterator::operator!=(ReverseIterator other) const noexcept {return node != other.node;}

MultiAVLTree::AVLNode& MultiAVLTree::AVLKeyNode::ReverseIterator::operator*() {return *node;}

MultiAVLTree::AVLNode* MultiAVLTree::AVLKeyNode::ReverseIterator::operator->() noexcept {return node;}

const MultiAVLTree::AVLNode& MultiAVLTree::AVLKeyNode::ReverseIterator::operator*() const {return *node;}

const MultiAVLTree::AVLNode* MultiAVLTree::AVLKeyNode::ReverseIterator::operator->() const noexcept {return node;}

MultiAVLTree::AVLKeyNode::ReverseIterator::operator Iterator() noexcept {return node;}

MultiAVLTree::AVLKeyNode::ReverseIterator::operator const Iterator() const noexcept {return node;}


// ======================================================== MultiAVLTree

i64 MultiAVLTree::max(i64 a, i64 b) noexcept {return (a > b) ? a : b;}

i64 MultiAVLTree::depth(AVLKeyNode* node) noexcept {return node ? node->depth : 0;}

MultiAVLTree::AVLKeyNode* MultiAVLTree::rotateRight(AVLKeyNode* y) {
  auto y_position = y->getPosition();
  AVLKeyNode* x = y->left;
  AVLKeyNode* T2 = x->right;

  x->right = y;
  y->left = T2;

  y->depth = max(depth(y->left), depth(y->right)) + 1;
  x->depth = max(depth(x->left), depth(x->right)) + 1;

  // Update parent poionters
  x->parent = y->parent;
  y->parent = x;
  if(T2) T2->parent = y;

  // Update parent's child poionter
  if(y_position == AVLKeyNode::NodePosition::left) x->parent->left = x;
  elif(y_position == AVLKeyNode::NodePosition::right) x->parent->right = x;
  else root = x;

  return x;
}

MultiAVLTree::AVLKeyNode* MultiAVLTree::rotateLeft(AVLKeyNode* x) {
  auto x_position = x->getPosition();
  AVLKeyNode* y = x->right;
  AVLKeyNode* T2 = y->left;

  y->left = x;
  x->right = T2;

  y->depth = max(depth(y->left), depth(y->right)) + 1;
  x->depth = max(depth(x->left), depth(x->right)) + 1;

  // Update parent poionters
  y->parent = x->parent;
  x->parent = y;
  if(T2) T2->parent = x;

  // Update parent's child poionter
  if(x_position == AVLKeyNode::NodePosition::left) y->parent->left = y;
  elif(x_position == AVLKeyNode::NodePosition::right) y->parent->right = y;
  else root = y;

  return y;
}

i64 MultiAVLTree::getBalance(AVLKeyNode* node) { return node ? depth(node->left) - depth(node->right) : 0; }

MultiAVLTree::AVLKeyNode* MultiAVLTree::minKeyNode(AVLKeyNode* node) noexcept {
  if(!node) return nullptr;
  while(node->left) node = node->left;
  return node;
}

MultiAVLTree::AVLKeyNode* MultiAVLTree::minKeyNode() const noexcept {return minKeyNode(root);}

MultiAVLTree::AVLKeyNode* MultiAVLTree::maxKeyNode(AVLKeyNode* node) noexcept {
  if(!node) return nullptr;
  while (node->right) node = node->right;
  return node;
}

MultiAVLTree::AVLKeyNode* MultiAVLTree::maxKeyNode() const noexcept {return maxKeyNode(root);}

bool MultiAVLTree::isEmpty() const noexcept {return !root;}

MultiAVLTree::NodePair MultiAVLTree::insert(KeyValue key, AVLNode* new_node, NewNodePosition position) {
  AVLKeyNode* node = root;
  NodePair result{.node = new_node};
  if(!node) {
    root = new AVLKeyNode(std::move(key));
    root->pushBack(new_node);
    return {root, new_node};
  }

  forever {
    auto cmp = key.getCompare(node->key);
    if(cmp == KeyValue::lower) {
      if(node->left) { node = node->left; continue; }
      else {
        node->left = new AVLKeyNode(std::move(key), node);
        node->left->pushBack(new_node);
        result.key_node = node->left;
        break;
      }
    } elif(cmp == KeyValue::highter) {
      if(node->right) { node = node->right; continue; }
      else {
        node->right = new AVLKeyNode(std::move(key), node);
        node->right->pushBack(new_node);
        result.key_node = node->right;
        break;
      }
    } else { // KeyValue::equal
      switch (position) {
      case binom::priv::MultiAVLTree::NewNodePosition::front:
        node->pushFront(new_node);
      break;
      case binom::priv::MultiAVLTree::NewNodePosition::back:
        node->pushBack(new_node);
      break;
      }
      return {node, new_node};
    }
  }

  while(node) { // Balancing
    node->depth = 1 + max(depth(node->left), depth(node->right));

    i64 balance = getBalance(node);

    if (balance > 1 && getBalance(root->left) >= 0) {
      rotateRight(node);
    } elif (balance > 1  && getBalance(root->left) < 0) {
      rotateLeft(node->left);
      rotateRight(node);
    } elif (balance < -1 && getBalance(node->right) <= 0) {
      rotateLeft(node);
    } elif (balance < -1 && getBalance(node->right) > 0) {
      rotateRight(node->right);
      rotateLeft(node);
    }

    node = node->parent;
  }

  return result;
}

MultiAVLTree::AVLNode* MultiAVLTree::extract(Iterator it) {
  auto& key_node = it.getKeyNode();
  auto iterator = it.getKeyNodeIterator();
  auto& result = key_node.extract(iterator);
  if(key_node.getElementCount()) return &result;

  AVLKeyNode* node = &key_node;
  forever if(!node->left || !node->right) {
    AVLKeyNode* tmp = node->left ? node->left : node->right;
    if(!tmp) { // If node hasn't child
      tmp = node;
      node = nullptr;
      tmp->unpin(self);
    } else { // If node has 1 child
      node->swapPosition(*tmp, self);
      node->unpin(self);
      node = tmp; // For balancing
    }
    break;
  } else { // If node has 2 childs (Test it!!!)

    // Change the position of the node
    // to be deleted whith the position
    // of the leftmost node in th right branch

    AVLKeyNode* tmp = minKeyNode(node->right);
    node->swapPosition(*tmp, self);
    continue;
  }

  while(node) { // Balancing

    node->depth = 1 + max(depth(node->left), depth(node->right));

    i64 balance = getBalance(node);

    if (balance > 1 && getBalance(root->left) >= 0) {
      rotateRight(node);
    } elif (balance > 1  && getBalance(root->left) < 0) {
      rotateLeft(node->left);
      rotateRight(node);
    } elif (balance < -1 && getBalance(node->right) <= 0) {
      rotateLeft(node);
    } elif (balance < -1 && getBalance(node->right) > 0) {
      rotateRight(node->right);
      rotateLeft(node);
    }

    node = node->parent;
  }

  delete &key_node;
  return &result;
}

MultiAVLTree::AVLNode* MultiAVLTree::extract(ReverseIterator r_it) {return extract(Iterator(r_it));}

bool MultiAVLTree::removeKey(KeyValue key, std::function<void (AVLNode*)> destructor) {
  AVLKeyNode* node = root;

  // Find key node by key
  forever {
    if(!node) return false;

    auto cmp = key.getCompare(node->key);
    if(cmp == KeyValue::lower) {node = node->left; continue;}
    elif(cmp == KeyValue::highter) {node = node->left; continue;}
    else break;
  }

  AVLKeyNode* deletable_node = node;

  // Delete value-nodes
  for(auto it = node->begin(), end = node->end(); it != end; destructor(&*it++));

  // Extract key-node
  forever if(!node->left || !node->right) {
    AVLKeyNode* tmp = node->left ? node->left : node->right;
    if(!tmp) { // If node hasn't child
      tmp = node;
      node = nullptr;
      tmp->unpin(self);
    } else { // If node has 1 child
      node->swapPosition(*tmp, self);
      node->unpin(self);
      node = tmp; // For balancing
    }
    break;
  } else { // If node has 2 childs (Test it!!!)

    // Change the position of the node
    // to be deleted whith the position
    // of the leftmost node in th right branch

    AVLKeyNode* tmp = minKeyNode(node->right);
    node->swapPosition(*tmp, self);
    continue;
  }

  while(node) { // Balancing

    node->depth = 1 + max(depth(node->left), depth(node->right));

    i64 balance = getBalance(node);

    if (balance > 1 && getBalance(root->left) >= 0) {
      rotateRight(node);
    } elif (balance > 1  && getBalance(root->left) < 0) {
      rotateLeft(node->left);
      rotateRight(node);
    } elif (balance < -1 && getBalance(node->right) <= 0) {
      rotateLeft(node);
    } elif (balance < -1 && getBalance(node->right) > 0) {
      rotateRight(node->right);
      rotateLeft(node);
    }

    node = node->parent;
  }

  // Delete key-node
  delete deletable_node;
  return true;
}

MultiAVLTree::Iterator MultiAVLTree::find(KeyValue key) const {
  AVLKeyNode* key_node = root;
  forever {
    if(!key_node) return nullptr;

    auto cmp = key.getCompare(key_node->key);
    if(cmp == KeyValue::lower) {key_node = key_node->left; continue;}
    elif(cmp == KeyValue::highter) {key_node = key_node->left; continue;}
    else return Iterator(key_node); // Node is finded
  }
}

MultiAVLTree::Iterator MultiAVLTree::findLast(KeyValue key) const {
  AVLKeyNode* key_node = root;
  forever {
    if(!key_node) return nullptr;

    auto cmp = key.getCompare(key_node->key);
    if(cmp == KeyValue::lower) {key_node = key_node->left; continue;}
    elif(cmp == KeyValue::highter) {key_node = key_node->left; continue;}
    else return Iterator(key_node->rbegin()); // Node is finded
  }
}

MultiAVLTree::ReverseIterator MultiAVLTree::rfind(KeyValue key) const {
  AVLKeyNode* key_node = root;
  forever {
    if(!key_node) return nullptr;

    auto cmp = key.getCompare(key_node->key);
    if(cmp == KeyValue::lower) {key_node = key_node->left; continue;}
    elif(cmp == KeyValue::highter) {key_node = key_node->left; continue;}
    else return Iterator(key_node); // Node is finded
  }
}

MultiAVLTree::ReverseIterator MultiAVLTree::rfindLast(KeyValue key) const {
  AVLKeyNode* key_node = root;
  forever {
    if(!key_node) return nullptr;

    auto cmp = key.getCompare(key_node->key);
    if(cmp == KeyValue::lower) {key_node = key_node->left; continue;}
    elif(cmp == KeyValue::highter) {key_node = key_node->left; continue;}
    else return Iterator(key_node->rbegin()); // Node is finded
  }
}

MultiAVLTree::Iterator MultiAVLTree::begin() noexcept {return minKeyNode();}

MultiAVLTree::Iterator MultiAVLTree::end() noexcept {return nullptr;}

MultiAVLTree::ReverseIterator MultiAVLTree::rbegin() noexcept {return maxKeyNode();}

MultiAVLTree::ReverseIterator MultiAVLTree::rend() noexcept {return nullptr;}

MultiAVLTree::ConstIterator MultiAVLTree::begin() const noexcept {return cbegin();}

MultiAVLTree::ConstIterator MultiAVLTree::end() const noexcept {return cend();}

MultiAVLTree::ConstReverseIterator MultiAVLTree::rbegin() const noexcept {return crbegin();}

MultiAVLTree::ConstReverseIterator MultiAVLTree::rend() const noexcept {return crend();}

MultiAVLTree::ConstIterator MultiAVLTree::cbegin() const noexcept {return minKeyNode();}

MultiAVLTree::ConstIterator MultiAVLTree::cend() const noexcept {return nullptr;}

MultiAVLTree::ConstReverseIterator MultiAVLTree::crbegin() const noexcept {return maxKeyNode();}

MultiAVLTree::ConstReverseIterator MultiAVLTree::crend() const noexcept {return nullptr;}

void MultiAVLTree::clear(std::function<void (AVLNode*)> destructor) {
  if(!root) return;
  AVLKeyNode* key_node = minKeyNode();
  if(key_node->hasRight()) key_node = key_node->right;
  while(key_node) {
    AVLKeyNode* last = key_node;
    if(!!key_node->parent) {
      if(key_node->isLeft() && key_node->parent->hasRight()) {
        key_node = minKeyNode(key_node->parent->right);
        if(key_node->hasRight()) key_node = key_node->right;
      } elif(key_node->isLeft() || key_node->isRight())
          key_node = key_node->parent;
    } else key_node = nullptr;

    for(auto it = last->begin(), end = last->end();
        it != end;) {
      AVLNode* node = &*it;
      ++it;
      destructor(node);
    }

    delete last;
  }
  root = nullptr;
}


// ======================================================== MultiAVLTree::Iterator

MultiAVLTree::Iterator::Iterator(AVLKeyNode* key_node)
  : iterator(key_node ? key_node->begin() : nullptr) {}

MultiAVLTree::Iterator::Iterator(AVLKeyNode::Iterator iterator)
  : iterator(iterator) {}

MultiAVLTree::Iterator::Iterator(const Iterator& other)
  : iterator(other.iterator) {}

MultiAVLTree::Iterator::Iterator(Iterator&& other)
  : iterator(other.iterator) {}

MultiAVLTree::Iterator& MultiAVLTree::Iterator::goToNextKey() {
  if(!iterator) return self;
  AVLKeyNode* key_node = iterator->key_node;
  if(!key_node) {
    iterator = nullptr;
    return self;
  }
  elif(key_node->hasRight()) key_node = MultiAVLTree::minKeyNode(key_node->right);
  elif(key_node->isLeft()) key_node = key_node->parent;
  elif(key_node->isRight()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isRight() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    iterator = nullptr;
    return self;
  }
  if(key_node) iterator = key_node->begin();
  else iterator = nullptr;
  return self;
}

MultiAVLTree::Iterator& MultiAVLTree::Iterator::goToPrevKey() {
  if(!iterator) return self;
  AVLKeyNode* key_node = iterator->key_node;
  if(!key_node) {
    iterator = nullptr;
    return self;
  }
  elif(key_node->hasLeft()) key_node = MultiAVLTree::maxKeyNode(key_node->left);
  elif(key_node->isRight()) key_node = key_node->parent;
  elif(key_node->isLeft()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isLeft() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    iterator = nullptr;
    return self;
  }
  if(key_node) iterator = key_node->rbegin();
  else iterator = nullptr;
  return *this;
}

const MultiAVLTree::Iterator& MultiAVLTree::Iterator::goToNextKey() const {
  if(!iterator) return self;
  AVLKeyNode* key_node = iterator->key_node;
  if(!key_node) {
    iterator = nullptr;
    return self;
  }
  elif(key_node->hasRight()) key_node = MultiAVLTree::minKeyNode(key_node->right);
  elif(key_node->isLeft()) key_node = key_node->parent;
  elif(key_node->isRight()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isRight() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    iterator = nullptr;
    return self;
  }
  if(key_node) iterator = key_node->begin();
  else iterator = nullptr;
  return self;
}

const MultiAVLTree::Iterator& MultiAVLTree::Iterator::goToPrevKey() const {
  if(!iterator) return self;
  AVLKeyNode* key_node = iterator->key_node;
  if(!key_node) {
    iterator = nullptr;
    return self;
  }
  elif(key_node->hasLeft()) key_node = MultiAVLTree::maxKeyNode(key_node->left);
  elif(key_node->isRight()) key_node = key_node->parent;
  elif(key_node->isLeft()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isLeft() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    iterator = nullptr;
    return self;
  }
  if(key_node) iterator = key_node->rbegin();
  else iterator = nullptr;
  return *this;
}

MultiAVLTree::Iterator& MultiAVLTree::Iterator::operator=(Iterator other) {iterator = other.iterator; return self;}

MultiAVLTree::Iterator& MultiAVLTree::Iterator::operator++() {
  if(!iterator) return self;
  AVLKeyNode* key_node = iterator->key_node;
  ++iterator;
  if(iterator != nullptr) return self;
  elif(!key_node) {
    iterator = nullptr;
    return self;
  }
  elif(key_node->hasRight()) key_node = MultiAVLTree::minKeyNode(key_node->right);
  elif(key_node->isLeft()) key_node = key_node->parent;
  elif(key_node->isRight()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isRight() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    iterator = nullptr;
    return self;
  }
  if(key_node) iterator = key_node->begin();
  else iterator = nullptr;
  return self;
}

MultiAVLTree::Iterator MultiAVLTree::Iterator::operator++(int) {Iterator tmp(self); ++self; return tmp;}

MultiAVLTree::Iterator& MultiAVLTree::Iterator::operator--() {
  if(!iterator) return self;
  AVLKeyNode* key_node = iterator->key_node;
  --iterator;
  if(iterator != nullptr) return self;
  elif(!key_node) {
    iterator = nullptr;
    return self;
  }
  elif(key_node->hasLeft()) key_node = MultiAVLTree::maxKeyNode(key_node->left);
  elif(key_node->isRight()) key_node = key_node->parent;
  elif(key_node->isLeft()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isLeft() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    iterator = nullptr;
    return self;
  }
  if(key_node) iterator = key_node->rbegin();
  else iterator = nullptr;
  return *this;
}

size_t MultiAVLTree::Iterator::getElementCount() const noexcept {
  if(iterator->key_node) return iterator->key_node->getElementCount();
  else return 0;
}

MultiAVLTree::Iterator MultiAVLTree::Iterator::operator--(int) {Iterator tmp(self); --self; return tmp;}

const MultiAVLTree::Iterator& MultiAVLTree::Iterator::operator++() const {
  if(!iterator) return self;
  AVLKeyNode* key_node = iterator->key_node;
  ++iterator;
  if(iterator != nullptr) return self;
  elif(!key_node) {
    iterator = nullptr;
    return self;
  }
  if(key_node->hasRight()) key_node = MultiAVLTree::minKeyNode(key_node->right);
  elif(key_node->isLeft()) key_node = key_node->parent;
  elif(key_node->isRight()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isRight() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    iterator = nullptr;
    return self;
  }
  if(key_node) iterator = key_node->begin();
  else iterator = nullptr;
  return self;
}

const MultiAVLTree::Iterator MultiAVLTree::Iterator::operator++(int) const {Iterator tmp(self); ++self; return tmp;}

const MultiAVLTree::Iterator& MultiAVLTree::Iterator::operator--() const {
  if(!iterator) return self;
  AVLKeyNode* key_node = iterator->key_node;
  --iterator;
  if(iterator != nullptr) return self;
  elif(!key_node) {
    iterator = nullptr;
    return self;
  }
  elif(key_node->hasLeft()) key_node = MultiAVLTree::maxKeyNode(key_node->left);
  elif(key_node->isRight()) key_node = key_node->parent;
  elif(key_node->isLeft()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isLeft() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    iterator = nullptr;
    return self;
  }
  if(key_node) iterator = key_node->rbegin();
  else iterator = nullptr;
  return *this;
}

MultiAVLTree::AVLKeyNode& MultiAVLTree::Iterator::getKeyNode() {return *iterator->key_node;}

const MultiAVLTree::AVLKeyNode& MultiAVLTree::Iterator::getKeyNode() const {return *iterator->key_node;}

MultiAVLTree::AVLKeyNode::Iterator MultiAVLTree::Iterator::getKeyNodeIterator() {return iterator;}

const MultiAVLTree::AVLKeyNode::Iterator MultiAVLTree::Iterator::getKeyNodeIterator() const {return iterator;}

KeyValue& MultiAVLTree::Iterator::getKey() {return iterator->key_node->getKey();}

const KeyValue& MultiAVLTree::Iterator::getKey() const {return iterator->key_node->getKey();}

const MultiAVLTree::Iterator MultiAVLTree::Iterator::operator--(int) const {Iterator tmp(self); --self; return tmp;}

bool MultiAVLTree::Iterator::operator==(Iterator other) const noexcept {return iterator == other.iterator;}

bool MultiAVLTree::Iterator::operator!=(Iterator other) const noexcept {return iterator != other.iterator;}

MultiAVLTree::AVLNode& MultiAVLTree::Iterator::operator*() {return *iterator;}

MultiAVLTree::AVLNode* MultiAVLTree::Iterator::operator->() noexcept {return iterator.operator->();}

const MultiAVLTree::AVLNode& MultiAVLTree::Iterator::operator*() const {return *iterator;}

const MultiAVLTree::AVLNode* MultiAVLTree::Iterator::operator->() const noexcept {return iterator.operator->();}

MultiAVLTree::Iterator::operator ReverseIterator() {return ReverseIterator(iterator);}

MultiAVLTree::Iterator::operator const ReverseIterator() const {return ReverseIterator(iterator);}

MultiAVLTree::Iterator MultiAVLTree::Iterator::begin() noexcept {return (iterator)? iterator->key_node->begin() : nullptr;}

MultiAVLTree::Iterator MultiAVLTree::Iterator::end() noexcept {return Iterator(self).goToNextKey();}

MultiAVLTree::ReverseIterator MultiAVLTree::Iterator::rbegin() noexcept {return (iterator)? iterator->key_node->rbegin() : nullptr;}

MultiAVLTree::ReverseIterator MultiAVLTree::Iterator::rend() noexcept {return ReverseIterator(self).goToNextKey();}

const MultiAVLTree::Iterator MultiAVLTree::Iterator::begin() const noexcept {return (iterator)? iterator->key_node->begin() : nullptr;}

const MultiAVLTree::Iterator MultiAVLTree::Iterator::end() const noexcept {return Iterator(self).goToNextKey();}

const MultiAVLTree::ReverseIterator MultiAVLTree::Iterator::rbegin() const noexcept {return (iterator)? iterator->key_node->rbegin() : nullptr;}

const MultiAVLTree::ReverseIterator MultiAVLTree::Iterator::rend() const noexcept {return ReverseIterator(self).goToNextKey();}

const MultiAVLTree::Iterator MultiAVLTree::Iterator::cbegin() const noexcept {return (iterator)? iterator->key_node->begin() : nullptr;}

const MultiAVLTree::Iterator MultiAVLTree::Iterator::cend() const noexcept {return Iterator(self).goToNextKey();}

const MultiAVLTree::ReverseIterator MultiAVLTree::Iterator::crbegin() const noexcept {return (iterator)? iterator->key_node->rbegin() : nullptr;}

const MultiAVLTree::ReverseIterator MultiAVLTree::Iterator::crend() const noexcept {return ReverseIterator(self).goToNextKey();}


// ======================================================== MultiAVLTree::ReverseIterator

MultiAVLTree::ReverseIterator::ReverseIterator(AVLKeyNode* key_node)
  : reverse_iterator(key_node ? key_node->rbegin() : nullptr) {}

MultiAVLTree::ReverseIterator::ReverseIterator(AVLKeyNode::ReverseIterator reverse_iterator)
  : reverse_iterator(reverse_iterator) {}

MultiAVLTree::ReverseIterator::ReverseIterator(const ReverseIterator& other)
  : reverse_iterator(other.reverse_iterator) {}

MultiAVLTree::ReverseIterator::ReverseIterator(ReverseIterator&& other)
  : reverse_iterator(other.reverse_iterator) {}

MultiAVLTree::ReverseIterator& MultiAVLTree::ReverseIterator::goToNextKey() {
  if(!reverse_iterator) return self;
  AVLKeyNode* key_node = reverse_iterator->key_node;
  if(!key_node) {
    reverse_iterator = nullptr;
    return self;
  }
  elif(key_node->hasLeft()) key_node = MultiAVLTree::maxKeyNode(key_node->left);
  elif(key_node->isRight()) key_node = key_node->parent;
  elif(key_node->isLeft()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isLeft() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    reverse_iterator = nullptr;
    return self;
  }
  if(key_node) reverse_iterator = key_node->rbegin();
  else reverse_iterator = nullptr;
  return self;
}

MultiAVLTree::ReverseIterator& MultiAVLTree::ReverseIterator::goToPrevKey() {
  if(!reverse_iterator) return self;
  AVLKeyNode* key_node = reverse_iterator->key_node;
  if(!key_node) {
    reverse_iterator = nullptr;
    return self;
  }
  elif(key_node->hasRight()) key_node = MultiAVLTree::minKeyNode(key_node->right);
  elif(key_node->isLeft()) key_node = key_node->parent;
  elif(key_node->isRight()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isRight() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    reverse_iterator = nullptr;
    return self;
  }
  if(key_node) reverse_iterator = key_node->begin();
  else reverse_iterator = nullptr;
  return *this;
}

const MultiAVLTree::ReverseIterator& MultiAVLTree::ReverseIterator::goToNextKey() const {
  if(!reverse_iterator) return self;
  AVLKeyNode* key_node = reverse_iterator->key_node;
  if(!key_node) {
    reverse_iterator = nullptr;
    return self;
  }
  elif(key_node->hasLeft()) key_node = MultiAVLTree::maxKeyNode(key_node->left);
  elif(key_node->isRight()) key_node = key_node->parent;
  elif(key_node->isLeft()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isLeft() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    reverse_iterator = nullptr;
    return self;
  }
  if(key_node) reverse_iterator = key_node->rbegin();
  else reverse_iterator = nullptr;
  return self;
}

const MultiAVLTree::ReverseIterator& MultiAVLTree::ReverseIterator::goToPrevKey() const {
  if(!reverse_iterator) return self;
  AVLKeyNode* key_node = reverse_iterator->key_node;
  if(!key_node) {
    reverse_iterator = nullptr;
    return self;
  }
  elif(key_node->hasRight()) key_node = MultiAVLTree::minKeyNode(key_node->right);
  elif(key_node->isLeft()) key_node = key_node->parent;
  elif(key_node->isRight()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isRight() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    reverse_iterator = nullptr;
    return self;
  }
  if(key_node) reverse_iterator = key_node->begin();
  else reverse_iterator = nullptr;
  return *this;
}

MultiAVLTree::ReverseIterator& MultiAVLTree::ReverseIterator::operator=(ReverseIterator other) {reverse_iterator = other.reverse_iterator; return self;}

MultiAVLTree::ReverseIterator& MultiAVLTree::ReverseIterator::operator++() {
  if(!reverse_iterator) return self;
  AVLKeyNode* key_node = reverse_iterator->key_node;
  ++reverse_iterator;
  if(reverse_iterator != nullptr) return self;
  elif(!key_node) {
    reverse_iterator = nullptr;
    return self;
  }
  elif(key_node->hasLeft()) key_node = MultiAVLTree::maxKeyNode(key_node->left);
  elif(key_node->isRight()) key_node = key_node->parent;
  elif(key_node->isLeft()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isLeft() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    reverse_iterator = nullptr;
    return self;
  }
  if(key_node) reverse_iterator = key_node->rbegin();
  else reverse_iterator = nullptr;
  return self;
}

MultiAVLTree::ReverseIterator MultiAVLTree::ReverseIterator::operator++(int) {ReverseIterator tmp(self); ++self; return tmp;}

MultiAVLTree::ReverseIterator& MultiAVLTree::ReverseIterator::operator--() {
  if(!reverse_iterator) return self;
  AVLKeyNode* key_node = reverse_iterator->key_node;
  --reverse_iterator;
  if(reverse_iterator != nullptr) return self;
  elif(!key_node) {
    reverse_iterator = nullptr;
    return self;
  }
  elif(key_node->hasRight()) key_node = MultiAVLTree::minKeyNode(key_node->right);
  elif(key_node->isLeft()) key_node = key_node->parent;
  elif(key_node->isRight()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isRight() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    reverse_iterator = nullptr;
    return self;
  }
  if(key_node) reverse_iterator = key_node->begin();
  else reverse_iterator = nullptr;
  return *this;
}

size_t MultiAVLTree::ReverseIterator::getElementCount() const noexcept {
  if(reverse_iterator->key_node) return reverse_iterator->key_node->getElementCount();
  else return 0;
}

MultiAVLTree::ReverseIterator MultiAVLTree::ReverseIterator::operator--(int) {ReverseIterator tmp(self); --self; return tmp;}

const MultiAVLTree::ReverseIterator& MultiAVLTree::ReverseIterator::operator++() const {
  if(!reverse_iterator) return self;
  AVLKeyNode* key_node = reverse_iterator->key_node;
  ++reverse_iterator;
  if(reverse_iterator != nullptr) return self;
  elif(!key_node) {
    reverse_iterator = nullptr;
    return self;
  }
  elif(key_node->hasRight()) key_node = MultiAVLTree::minKeyNode(key_node->right);
  elif(key_node->isLeft()) key_node = key_node->parent;
  elif(key_node->isRight()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isRight() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    reverse_iterator = nullptr;
    return self;
  }
  if(key_node) reverse_iterator = key_node->begin();
  else reverse_iterator = nullptr;
  return self;
}

const MultiAVLTree::ReverseIterator MultiAVLTree::ReverseIterator::operator++(int) const {ReverseIterator tmp(self); ++self; return tmp;}

const MultiAVLTree::ReverseIterator& MultiAVLTree::ReverseIterator::operator--() const {
  if(!reverse_iterator) return self;
  AVLKeyNode* key_node = reverse_iterator->key_node;
  --reverse_iterator;
  if(reverse_iterator != nullptr) return self;
  elif(!key_node) {
    reverse_iterator = nullptr;
    return self;
  }
  if(key_node->hasRight()) key_node = MultiAVLTree::minKeyNode(key_node->right);
  elif(key_node->isLeft()) key_node = key_node->parent;
  elif(key_node->isRight()) {
    AVLKeyNode* tmp = key_node->parent;
    key_node = key_node->parent->parent;
    while (tmp->isRight() && key_node) {tmp = key_node; key_node = key_node->parent;}
  } elif(key_node->isRoot()) {
    reverse_iterator = nullptr;
    return self;
  }
  if(key_node) reverse_iterator = key_node->begin();
  else reverse_iterator = nullptr;
  return *this;
}

MultiAVLTree::AVLKeyNode& MultiAVLTree::ReverseIterator::getKeyNode() {return *reverse_iterator->key_node;}

const MultiAVLTree::AVLKeyNode& MultiAVLTree::ReverseIterator::getKeyNode() const {return *reverse_iterator->key_node;}

MultiAVLTree::AVLKeyNode::ReverseIterator MultiAVLTree::ReverseIterator::getKeyNodeIterator() {return reverse_iterator;}

const MultiAVLTree::AVLKeyNode::ReverseIterator MultiAVLTree::ReverseIterator::getKeyNodeIterator() const {return reverse_iterator;}

KeyValue& MultiAVLTree::ReverseIterator::getKey() {return reverse_iterator->key_node->getKey();}

const KeyValue& MultiAVLTree::ReverseIterator::getKey() const {return reverse_iterator->key_node->getKey();}

const MultiAVLTree::ReverseIterator MultiAVLTree::ReverseIterator::operator--(int) const {ReverseIterator tmp(self); --self; return tmp;}

bool MultiAVLTree::ReverseIterator::operator==(ReverseIterator other) const noexcept {return reverse_iterator == other.reverse_iterator;}

bool MultiAVLTree::ReverseIterator::operator!=(ReverseIterator other) const noexcept {return reverse_iterator != other.reverse_iterator;}

MultiAVLTree::AVLNode& MultiAVLTree::ReverseIterator::operator*() {return *reverse_iterator;}

MultiAVLTree::AVLNode* MultiAVLTree::ReverseIterator::operator->() noexcept {return reverse_iterator.operator->();}

const MultiAVLTree::AVLNode& MultiAVLTree::ReverseIterator::operator*() const {return *reverse_iterator;}

const MultiAVLTree::AVLNode* MultiAVLTree::ReverseIterator::operator->() const noexcept {return reverse_iterator.operator->();}

MultiAVLTree::ReverseIterator::operator Iterator() {return Iterator(reverse_iterator);}

MultiAVLTree::ReverseIterator::operator const Iterator() const {return Iterator(reverse_iterator);}

MultiAVLTree::Iterator MultiAVLTree::ReverseIterator::rbegin() noexcept {return (reverse_iterator)? reverse_iterator->key_node->begin() : nullptr;}

MultiAVLTree::Iterator MultiAVLTree::ReverseIterator::rend() noexcept {return Iterator(self).goToNextKey();}

MultiAVLTree::ReverseIterator MultiAVLTree::ReverseIterator::begin() noexcept {return (reverse_iterator)? reverse_iterator->key_node->rbegin() : nullptr;}

MultiAVLTree::ReverseIterator MultiAVLTree::ReverseIterator::end() noexcept {return ReverseIterator(self).goToNextKey();}

const MultiAVLTree::Iterator MultiAVLTree::ReverseIterator::rbegin() const noexcept {return (reverse_iterator)? reverse_iterator->key_node->begin() : nullptr;}

const MultiAVLTree::Iterator MultiAVLTree::ReverseIterator::rend() const noexcept {return Iterator(self).goToNextKey();}

const MultiAVLTree::ReverseIterator MultiAVLTree::ReverseIterator::begin() const noexcept {return (reverse_iterator)? reverse_iterator->key_node->rbegin() : nullptr;}

const MultiAVLTree::ReverseIterator MultiAVLTree::ReverseIterator::end() const noexcept {return ReverseIterator(self).goToNextKey();}

const MultiAVLTree::Iterator MultiAVLTree::ReverseIterator::crbegin() const noexcept {return (reverse_iterator)? reverse_iterator->key_node->begin() : nullptr;}

const MultiAVLTree::Iterator MultiAVLTree::ReverseIterator::crend() const noexcept {return Iterator(self).goToNextKey();}

const MultiAVLTree::ReverseIterator MultiAVLTree::ReverseIterator::cbegin() const noexcept {return (reverse_iterator)? reverse_iterator->key_node->rbegin() : nullptr;}

const MultiAVLTree::ReverseIterator MultiAVLTree::ReverseIterator::cend() const noexcept {return ReverseIterator(self).goToNextKey();}
