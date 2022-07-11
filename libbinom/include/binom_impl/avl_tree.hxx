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
      auto other_position = getPosition();
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

    bool isRoot() const {return !parent;}
    bool isLeft() const {return isRoot() ? false : parent->left == this;}
    bool isRight() const {return isRoot() ? false : parent->right == this;}

    bool hasLeft() const {return left;}
    bool hasRight() const {return right;}
    bool hasChild() const {return left || right;}

    NodePosition getPosition() {
      if(isLeft()) return NodePosition::left;
      elif(isRight()) return NodePosition::right;
      else return NodePosition::root;
    }

  };

private:

  Node* root = nullptr;

  static inline i64 max(i64 a, i64 b) {return (a > b) ? a : b;}

  static inline i64 depth(Node* node) {return node ? node->depth : 0;}

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

  static Node* minKeyNode(Node* node) {
    if(!node) return nullptr;
    while(node->left) node = node->left;
    return node;
  }

  Node* minKeyNode() const {return minKeyNode(root);}

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

    while(node) {
      i64 balance = getBalance(node);

      auto cmp = new_node->key.getCompare(node->left->key);

      if(balance > 1 && cmp == KeyValue::lower) {
        rotateRight(node);
        node = node->parent;
        continue;
      } elif(balance > 1 && cmp == KeyValue::highter) {
        rotateLeft(node->left);
        rotateRight(node);
        node = node->parent;
        continue;
      }

      cmp = new_node->key.getCompare(node->right->key);

      if(balance < -1 && cmp == KeyValue::highter) {
        rotateLeft(node);
        node = node->parent;
        continue;
      } elif(balance < -1 && cmp == KeyValue::lower) {
        rotateRight(node->left);
        rotateLeft(node);
        node = node->parent;
        continue;
      }

      node = node->parent;
    }

    return new_node;
  }

  Node* extract(KeyValue key) {
    Node* result = nullptr;
    Node* node = root;

    forever {
      // If node isn't finded
      if(!node) return nullptr;

      // Searching key...
      auto cmp = key.getCompare(node->key);
      if(cmp == KeyValue::lower) {node = node->left; continue;}
      elif(cmp == KeyValue::highter) {node = node->left; continue;}
      else { // Node is finded
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

};

}


#endif // AVL_TREE_HXX
