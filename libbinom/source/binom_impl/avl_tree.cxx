#include "libbinom/include/binom_impl/avl_tree.hxx"

using namespace binom;

// ============================================================ AVLTree::Node


void AVLTree::Node::swapPosition(Node& other, AVLTree& avl_tree) {
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

void AVLTree::Node::unpin(AVLTree& avl_tree) {
  if(isLeft()) parent->left = nullptr;
  elif(isRight()) parent->right = nullptr;
  else avl_tree.root = nullptr;
  parent = nullptr;
}

AVLTree::Node::Node(KeyValue key, Node* parent) : parent(parent), key(std::move(key)) {}

AVLTree::Node::Node(Node& other)
  : depth(other.depth),
    left(other.left),
    right(other.right),
    parent(other.parent),
    key(other.key) {}

AVLTree::Node::Node(Node&& other)
  : depth(other.depth),
    left(other.left),
    right(other.right),
    parent(other.parent),
    key(std::move(other.key)) {}

AVLTree::Node& AVLTree::Node::operator=(Node other) { this->~Node(); return *new(this) Node(std::move(other)); }

bool AVLTree::Node::isRoot() const noexcept {return !parent;}

bool AVLTree::Node::isLeft() const noexcept {return isRoot() ? false : parent->left == this;}

bool AVLTree::Node::isRight() const noexcept {return isRoot() ? false : parent->right == this;}

bool AVLTree::Node::hasLeft() const noexcept {return left;}

bool AVLTree::Node::hasRight() const noexcept {return right;}

bool AVLTree::Node::hasChild() const noexcept {return left || right;}

KeyValue& AVLTree::Node::getKey() {return key;}

AVLTree::Node::NodePosition AVLTree::Node::getPosition() {
  if(isLeft()) return NodePosition::left;
  elif(isRight()) return NodePosition::right;
  else return NodePosition::root;
}


// ============================================================ AVLTree

i64 AVLTree::max(i64 a, i64 b) noexcept {return (a > b) ? a : b;}

i64 AVLTree::depth(Node* node) noexcept {return node ? node->depth : 0;}

AVLTree::Node* AVLTree::rotateRight(Node* y) {
  auto y_position = y->getPosition();
  Node* x = y->left;
  Node* T2 = x->right;

  x->right = y;
  y->left = T2;

  y->depth = max(depth(y->left), depth(y->right)) + 1;
  x->depth = max(depth(x->left), depth(x->right)) + 1;

  // Update parent poionters
  x->parent = y->parent;
  y->parent = x;
  if(T2) T2->parent = y;

  // Update parent's child poionter
  if(y_position == Node::NodePosition::left) x->parent->left = x;
  elif(y_position == Node::NodePosition::right) x->parent->right = x;
  else root = x;

  return x;
}

AVLTree::Node* AVLTree::rotateLeft(Node* x) {
  auto x_position = x->getPosition();
  Node* y = x->right;
  Node* T2 = y->left;

  y->left = x;
  x->right = T2;

  y->depth = max(depth(y->left), depth(y->right)) + 1;
  x->depth = max(depth(x->left), depth(x->right)) + 1;

  // Update parent poionters
  y->parent = x->parent;
  x->parent = y;
  if(T2) T2->parent = x;

  // Update parent's child poionter
  if(x_position == Node::NodePosition::left) y->parent->left = y;
  elif(x_position == Node::NodePosition::right) y->parent->right = y;
  else root = y;

  return y;
}

i64 AVLTree::getBalance(Node* node) { return node ? depth(node->left) - depth(node->right) : 0; }

AVLTree::Node* AVLTree::minKeyNode(Node* node) noexcept {
  if(!node) return nullptr;
  while(node->left) node = node->left;
  return node;
}

AVLTree::Node* AVLTree::minKeyNode() const noexcept {return minKeyNode(root);}

AVLTree::Node* AVLTree::maxKeyNode(Node* node) noexcept {
  if(!node) return nullptr;
  while (node->right) node = node->right;
  return node;
}

AVLTree::Node* AVLTree::maxKeyNode() const noexcept {return maxKeyNode(root);}

AVLTree::Node* AVLTree::insert(Node* new_node) {
  Node* node = root;
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

AVLTree::Node* AVLTree::extract(KeyValue key) {
  Node* result = nullptr;
  Node* node = root;

  forever {
    // If node isn't finded
    if(!node) return nullptr;

    // Searching key...
    auto cmp = key.getCompare(node->key);
    if(cmp == KeyValue::lower) {node = node->left; continue;}
    elif(cmp == KeyValue::highter) {node = node->right; continue;}
    else { // Node is finded
      forever if(!node->left || !node->right) {
        Node* tmp = node->left ? node->left : node->right;
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

        Node* tmp = minKeyNode(node->right);
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

AVLTree::Node* AVLTree::extract(Node* node) {
  Node* result = node;
  forever if(!node->left || !node->right) {
    Node* tmp = node->left ? node->left : node->right;
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

    Node* tmp = minKeyNode(node->right);
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

AVLTree::Node* AVLTree::get(KeyValue key) const {
  Node* node = root;
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
