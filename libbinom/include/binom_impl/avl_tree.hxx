#ifndef AVL_TREE_HXX
#define AVL_TREE_HXX

#include "types.hxx"
#include "../variables/key_value.hxx"

namespace binom {

using namespace type_alias;

class AVLTree {
public:

  class Node {
  public:

    enum class NodePosition : i8 {
      root = 0,
      left = -1,
      right = 1
    };

  private:
    friend class AVLTree;
    i64 depth = 1;
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent;
    KeyValue key;

    void swapPosition(Node& other, AVLTree& avl_tree) {
      auto this_position = getPosition();
      auto other_position = other.getPosition();

      std::swap(parent, other.parent);
      std::swap(left, other.left);
      std::swap(right, other.right);
      std::swap(depth, other.depth);

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

    void unpin() {
      if(isLeft()) parent->left = nullptr;
      elif(isRight()) parent->right = nullptr;
      parent = nullptr;
    }

  public:

    Node(KeyValue key, Node* parent = nullptr) : parent(parent), key(std::move(key)) {}

    Node(Node& other)
      : depth(other.depth),
        left(other.left),
        right(other.right),
        parent(other.parent),
        key(other.key) {}

    Node(Node&& other)
      : depth(other.depth),
        left(other.left),
        right(other.right),
        parent(other.parent),
        key(std::move(other.key)) {}

    Node& operator=(Node other) { this->~Node(); return *new(this) Node(std::move(other)); }

    bool isRoot() const noexcept {return !parent;}
    bool isLeft() const noexcept {return isRoot() ? false : parent->left == this;}
    bool isRight() const noexcept {return isRoot() ? false : parent->right == this;}

    bool hasLeft() const noexcept {return left;}
    bool hasRight() const noexcept {return right;}
    bool hasChild() const noexcept {return left || right;}

    KeyValue getKey() const {return key;}

    NodePosition getPosition() {
      if(isLeft()) return NodePosition::left;
      elif(isRight()) return NodePosition::right;
      else return NodePosition::root;
    }

  };

  class Iterator {
    Node* node = nullptr;
  public:
    Iterator(Node* node = nullptr) : node(node) {}
    Iterator(Iterator& other) : node(other.node) {}
    Iterator(Iterator&& other) : node(other.node) {}
    Iterator& operator=(Iterator& other) noexcept {return *new(this) Iterator(other);}
    Iterator& operator=(Iterator&& other) noexcept {return *new(this) Iterator(std::move(other));}
    Iterator& operator=(Node* node) noexcept {return *new(this) Iterator(node);}
    Iterator& operator=(Node& node) noexcept {return *new(this) Iterator(&node);}

    Iterator& operator++() {
      if(node->hasRight()) node = AVLTree::minKeyNode(node->right);
      elif(node->isLeft()) node = node->parent;
      elif(node->isRight()) {
        Node* tmp = node->parent;
        node = node->parent->parent;
        while (tmp->isRight()) {tmp = node; node = node->parent;}
      }
      return *this;
    }

    Iterator& operator--() {
      if(node->hasLeft()) node = AVLTree::maxKeyNode(node->left);
      elif(node->isRight()) node = node->parent;
      elif(node->isLeft()) {
        Node* tmp = node->parent;
        node = node->parent->parent;
        while (tmp->isLeft()) {tmp = node; node = node->parent;}
      }
      return *this;
    }

    bool operator==(Iterator other) noexcept {return node == other.node;}
    bool operator!=(Iterator other) noexcept {return node != other.node;}

    Node& operator*() {return *node;}
    Node* operator->() {return node;}

  };

private:

  Node* root = nullptr;

  static inline i64 max(i64 a, i64 b) noexcept {return (a > b) ? a : b;}

  static inline i64 depth(Node* node) noexcept {return node ? node->depth : 0;}

  Node* rotateRight(Node* y) {
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

  Node* rotateLeft(Node* x) {
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

  static i64 getBalance(Node* node) { return node ? depth(node->left) - depth(node->right) : 0; }

  static Node* minKeyNode(Node* node) noexcept {
    if(!node) return nullptr;
    while(node->left) node = node->left;
    return node;
  }

  Node* minKeyNode() const noexcept {return minKeyNode(root);}

  static Node* maxKeyNode(Node* node) noexcept {
    if(!node) return nullptr;
    while (node->right) node = node->right;
    return node;
  }

  Node* maxKeyNode() const noexcept {return maxKeyNode(root);}

public:

  Node* insert(Node* new_node) {
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

  [[nodiscard]] Node* extract(KeyValue key) {
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
            tmp->unpin();
            result = tmp;
          } else { // If node has 1 child
            node->swapPosition(*tmp, self);
            node->unpin();
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

  Node* extract(Node* node) {
    Node* result = node;
    forever if(!node->left || !node->right) {
      Node* tmp = node->left ? node->left : node->right;
      if(!tmp) { // If node hasn't child
        tmp = node;
        node = nullptr;
        tmp->unpin();
        result = tmp;
      } else { // If node has 1 child
        node->swapPosition(*tmp, self);
        node->unpin();
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

  Node* get(KeyValue key) {
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

  Iterator begin() noexcept {return minKeyNode();}
  Iterator end() {return nullptr;}

};

}


#endif // AVL_TREE_HXX
