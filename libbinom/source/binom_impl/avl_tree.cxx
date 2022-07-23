#include "libbinom/include/binom_impl/avl_tree.hxx"

using namespace binom;
using namespace binom::priv;

// ============================================================ Node


void AVLNode::swapPosition(AVLNode& other, AVLTree& avl_tree) {
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

void AVLNode::unpin(AVLTree& avl_tree) {
  if(isLeft()) parent->left = nullptr;
  elif(isRight()) parent->right = nullptr;
  else avl_tree.root = nullptr;
  parent = nullptr;
}

AVLNode::AVLNode(KeyValue key, AVLNode* parent) : parent(parent), key(std::move(key)) {}

AVLNode::AVLNode(AVLNode& other)
  : depth(other.depth),
    left(other.left),
    right(other.right),
    parent(other.parent),
    key(other.key) {}

AVLNode::AVLNode(AVLNode&& other)
  : depth(other.depth),
    left(other.left),
    right(other.right),
    parent(other.parent),
    key(std::move(other.key)) {}

AVLNode& AVLNode::operator=(AVLNode other) { this->~AVLNode(); return *new(this) AVLNode(std::move(other)); }

bool AVLNode::isRoot() const noexcept {return !parent;}

bool AVLNode::isLeft() const noexcept {return isRoot() ? false : parent->left == this;}

bool AVLNode::isRight() const noexcept {return isRoot() ? false : parent->right == this;}

bool AVLNode::hasLeft() const noexcept {return left;}

bool AVLNode::hasRight() const noexcept {return right;}

bool AVLNode::hasChild() const noexcept {return left || right;}

KeyValue& AVLNode::getKey() {return key;}

const KeyValue& AVLNode::getKey() const {return key;}

AVLNode::NodePosition AVLNode::getPosition() const {
  if(isLeft()) return NodePosition::left;
  elif(isRight()) return NodePosition::right;
  else return NodePosition::root;
}


// ============================================================ AVLTree

i64 AVLTree::max(i64 a, i64 b) noexcept {return (a > b) ? a : b;}

i64 AVLTree::depth(AVLNode* node) noexcept {return node ? node->depth : 0;}

AVLNode* AVLTree::rotateRight(AVLNode* y) {
  auto y_position = y->getPosition();
  AVLNode* x = y->left;
  AVLNode* T2 = x->right;

  x->right = y;
  y->left = T2;

  y->depth = max(depth(y->left), depth(y->right)) + 1;
  x->depth = max(depth(x->left), depth(x->right)) + 1;

  // Update parent poionters
  x->parent = y->parent;
  y->parent = x;
  if(T2) T2->parent = y;

  // Update parent's child poionter
  if(y_position == AVLNode::NodePosition::left) x->parent->left = x;
  elif(y_position == AVLNode::NodePosition::right) x->parent->right = x;
  else root = x;

  return x;
}

AVLNode* AVLTree::rotateLeft(AVLNode* x) {
  auto x_position = x->getPosition();
  AVLNode* y = x->right;
  AVLNode* T2 = y->left;

  y->left = x;
  x->right = T2;

  y->depth = max(depth(y->left), depth(y->right)) + 1;
  x->depth = max(depth(x->left), depth(x->right)) + 1;

  // Update parent poionters
  y->parent = x->parent;
  x->parent = y;
  if(T2) T2->parent = x;

  // Update parent's child poionter
  if(x_position == AVLNode::NodePosition::left) y->parent->left = y;
  elif(x_position == AVLNode::NodePosition::right) y->parent->right = y;
  else root = y;

  return y;
}

i64 AVLTree::getBalance(AVLNode* node) { return node ? depth(node->left) - depth(node->right) : 0; }

AVLNode* AVLTree::minKeyNode(AVLNode* node) noexcept {
  if(!node) return nullptr;
  while(node->left) node = node->left;
  return node;
}

AVLNode* AVLTree::minKeyNode() const noexcept {return minKeyNode(root);}

AVLNode* AVLTree::maxKeyNode(AVLNode* node) noexcept {
  if(!node) return nullptr;
  while (node->right) node = node->right;
  return node;
}

AVLNode* AVLTree::maxKeyNode() const noexcept {return maxKeyNode(root);}

AVLNode* AVLTree::insert(AVLNode* new_node) {
  AVLNode* node = root;
  if(!node) return root = new_node;

  forever {
    auto cmp = new_node->key.getCompare(node->key);
    if(cmp == KeyValue::lower) {
      if(node->left) { node = node->left; continue; }
      else { node->left = new_node; new_node->parent = node; break; }
    } elif(cmp == KeyValue::highter) {
      if(node->right) { node = node->right; continue; }
      else { node->right = new_node; new_node->parent = node; break; }
    } else return nullptr;
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

  return new_node;
}

AVLNode* AVLTree::extract(KeyValue key) {
  AVLNode* result = nullptr;
  AVLNode* node = root;

  forever {
    // If node isn't finded
    if(!node) return nullptr;

    // Searching key...
    auto cmp = key.getCompare(node->key);
    if(cmp == KeyValue::lower) {node = node->left; continue;}
    elif(cmp == KeyValue::highter) {node = node->right; continue;}
    else { // Node is finded
      forever if(!node->left || !node->right) {
        AVLNode* tmp = node->left ? node->left : node->right;
        if(!tmp) { // If node hasn't child
          tmp = node;
          node = node->parent;
          tmp->unpin(self);
          result = tmp;
        } else { // If node has 1 child
          node->swapPosition(*tmp, self);
          node->unpin(self);
          result = node;
          node = tmp; // For balancing
        }
        break;
      } else { // If node has 2 childs (Test it!!!)

        // Change the position of the node
        // to be deleted whith the position
        // of the leftmost node in th right branch

        AVLNode* tmp = minKeyNode(node->right);
        node->swapPosition(*tmp, self);
        continue;
      }
      break;
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

AVLNode* AVLTree::extract(AVLNode* node) {
  AVLNode* result = node;
  forever if(!node->left || !node->right) {
    AVLNode* tmp = node->left ? node->left : node->right;
    if(!tmp) { // If node hasn't child
      tmp = node;
      node = nullptr;
      tmp->unpin(self);
      result = tmp;
    } else { // If node has 1 child
      node->swapPosition(*tmp, self);
      node->unpin(self);
      result = node;
      node = tmp; // For balancing
    }
    break;
  } else { // If node has 2 childs (Test it!!!)

    // Change the position of the node
    // to be deleted whith the position
    // of the leftmost node in th right branch

    AVLNode* tmp = minKeyNode(node->right);
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

  return result;
}

AVLNode* AVLTree::get(KeyValue key) const {
  AVLNode* node = root;
  forever {
    // If node isn't finded
    if(!node) return nullptr;

    // Searching key...
    auto cmp = key.getCompare(node->key);
    if(cmp == KeyValue::lower) {node = node->left; continue;}
    elif(cmp == KeyValue::highter) {node = node->left; continue;}
    else return node; // Node is finded
  }
}

AVLTree::Iterator AVLTree::begin() noexcept {return minKeyNode();}

AVLTree::Iterator AVLTree::end() {return nullptr;}

AVLTree::ReverseIterator AVLTree::rbegin() noexcept {return maxKeyNode();}

AVLTree::ReverseIterator AVLTree::rend() {return nullptr;}





AVLTree::Iterator::Iterator(AVLNode* node) : node(node) {}

AVLTree::Iterator::Iterator(const Iterator& other) : node(other.node) {}

AVLTree::Iterator::Iterator(Iterator&& other) : node(other.node) {}

AVLTree::Iterator& AVLTree::Iterator::operator=(Iterator& other) noexcept {return *new(this) Iterator(other);}

AVLTree::Iterator& AVLTree::Iterator::operator=(Iterator&& other) noexcept {return *new(this) Iterator(std::move(other));}

AVLTree::Iterator& AVLTree::Iterator::operator=(AVLNode* node) noexcept {return *new(this) Iterator(node);}

AVLTree::Iterator& AVLTree::Iterator::operator=(AVLNode& node) noexcept {return *new(this) Iterator(&node);}

AVLTree::Iterator& AVLTree::Iterator::operator++() {
  if(node->hasRight()) node = AVLTree::minKeyNode(node->right);
  elif(node->isLeft()) node = node->parent;
  elif(node->isRight()) {
    AVLNode* tmp = node->parent;
    node = node->parent->parent;
    while (tmp->isRight()) {tmp = node; node = node->parent;}
  }
  return *this;
}

AVLTree::Iterator& AVLTree::Iterator::operator--() {
  if(node->hasLeft()) node = AVLTree::maxKeyNode(node->left);
  elif(node->isRight()) node = node->parent;
  elif(node->isLeft()) {
    AVLNode* tmp = node->parent;
    node = node->parent->parent;
    while (tmp->isLeft()) {tmp = node; node = node->parent;}
  }
  return *this;
}

AVLTree::Iterator AVLTree::Iterator::operator++(int) {Iterator tmp(self); ++self; return tmp;}

AVLTree::Iterator AVLTree::Iterator::operator--(int) {Iterator tmp(self); --self; return tmp;}

const AVLTree::Iterator& AVLTree::Iterator::operator++() const {
  if(node->hasRight()) node = AVLTree::minKeyNode(node->right);
  elif(node->isLeft()) node = node->parent;
  elif(node->isRight()) {
    AVLNode* tmp = node->parent;
    node = node->parent->parent;
    while (tmp->isRight()) {tmp = node; node = node->parent;}
  }
  return *this;
}

const AVLTree::Iterator& AVLTree::Iterator::operator--() const {
  if(node->hasLeft()) node = AVLTree::maxKeyNode(node->left);
  elif(node->isRight()) node = node->parent;
  elif(node->isLeft()) {
    AVLNode* tmp = node->parent;
    node = node->parent->parent;
    while (tmp->isLeft()) {tmp = node; node = node->parent;}
  }
  return *this;
}

const AVLTree::Iterator AVLTree::Iterator::operator++(int) const {Iterator tmp(self); ++self; return tmp;}

const AVLTree::Iterator AVLTree::Iterator::operator--(int) const {Iterator tmp(self); --self; return tmp;}

bool AVLTree::Iterator::operator==(Iterator other) const noexcept {return node == other.node;}

bool AVLTree::Iterator::operator!=(Iterator other) const noexcept {return node != other.node;}

AVLNode& AVLTree::Iterator::operator*() {return *node;}

AVLNode* AVLTree::Iterator::operator->() {return node;}

const AVLNode& AVLTree::Iterator::operator*() const {return *node;}

const AVLNode* AVLTree::Iterator::operator->() const {return node;}





AVLTree::ReverseIterator::ReverseIterator(AVLNode* node) : node(node) {}

AVLTree::ReverseIterator::ReverseIterator(const ReverseIterator& other) : node(other.node) {}

AVLTree::ReverseIterator::ReverseIterator(ReverseIterator&& other) : node(other.node) {}

AVLTree::ReverseIterator& AVLTree::ReverseIterator::operator=(ReverseIterator& other) noexcept {return *new(this) ReverseIterator(other);}

AVLTree::ReverseIterator& AVLTree::ReverseIterator::operator=(ReverseIterator&& other) noexcept {return *new(this) ReverseIterator(std::move(other));}

AVLTree::ReverseIterator& AVLTree::ReverseIterator::operator=(AVLNode* node) noexcept {return *new(this) ReverseIterator(node);}

AVLTree::ReverseIterator& AVLTree::ReverseIterator::operator=(AVLNode& node) noexcept {return *new(this) ReverseIterator(&node);}

AVLTree::ReverseIterator& AVLTree::ReverseIterator::operator++() {
  if(node->hasLeft()) node = AVLTree::maxKeyNode(node->left);
  elif(node->isRight()) node = node->parent;
  elif(node->isLeft()) {
    AVLNode* tmp = node->parent;
    node = node->parent->parent;
    while (tmp->isLeft()) {tmp = node; node = node->parent;}
  }
  return *this;
}

AVLTree::ReverseIterator& AVLTree::ReverseIterator::operator--() {
  if(node->hasRight()) node = AVLTree::minKeyNode(node->right);
  elif(node->isLeft()) node = node->parent;
  elif(node->isRight()) {
    AVLNode* tmp = node->parent;
    node = node->parent->parent;
    while (tmp->isRight()) {tmp = node; node = node->parent;}
  }
  return *this;
}

AVLTree::ReverseIterator AVLTree::ReverseIterator::operator++(int) {ReverseIterator tmp(self); ++self; return tmp;}

AVLTree::ReverseIterator AVLTree::ReverseIterator::operator--(int) {ReverseIterator tmp(self); --self; return tmp;}

const AVLTree::ReverseIterator& AVLTree::ReverseIterator::operator++() const {
  if(node->hasLeft()) node = AVLTree::maxKeyNode(node->left);
  elif(node->isRight()) node = node->parent;
  elif(node->isLeft()) {
    AVLNode* tmp = node->parent;
    node = node->parent->parent;
    while (tmp->isLeft()) {tmp = node; node = node->parent;}
  }
  return *this;
}

const AVLTree::ReverseIterator& AVLTree::ReverseIterator::operator--() const {
  if(node->hasRight()) node = AVLTree::minKeyNode(node->right);
  elif(node->isLeft()) node = node->parent;
  elif(node->isRight()) {
    AVLNode* tmp = node->parent;
    node = node->parent->parent;
    while (tmp->isRight()) {tmp = node; node = node->parent;}
  }
  return *this;
}

const AVLTree::ReverseIterator AVLTree::ReverseIterator::operator++(int) const {ReverseIterator tmp(self); ++self; return tmp;}

const AVLTree::ReverseIterator AVLTree::ReverseIterator::operator--(int) const {ReverseIterator tmp(self); --self; return tmp;}

bool AVLTree::ReverseIterator::operator==(ReverseIterator other) const noexcept {return node == other.node;}

bool AVLTree::ReverseIterator::operator!=(ReverseIterator other) const noexcept {return node != other.node;}

AVLNode& AVLTree::ReverseIterator::operator*() {return *node;}

AVLNode* AVLTree::ReverseIterator::operator->() {return node;}

const AVLNode& AVLTree::ReverseIterator::operator*() const {return *node;}

const AVLNode* AVLTree::ReverseIterator::operator->() const {return node;}
