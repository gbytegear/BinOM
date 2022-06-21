#ifndef AVL_TREE_HXX
#define AVL_TREE_HXX

#include "types.hxx"
#include "../variables/key_value.hxx"

namespace binom {

using namespace type_alias;

class AVLTree {
public:
  class Node {
    friend class AVLTree;
    i64 depth = 1;
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent;
    KeyValue key;

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

  };

private:

  Node* root;

  static inline i64 max(i64 a, i64 b) {return (a > b) ? a : b;}

  static inline i64 depth(Node* node) {return node ? node->depth : 0;}

  static Node* rotateRight(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->depth = max(depth(y->left), depth(y->right)) + 1;
    x->depth = max(depth(x->left), depth(x->right)) + 1;

    x->parent = y->parent;
    y->parent = x;
    if(T2) T2->parent = y;

    return x;
  }

  static Node* rotateLeft(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    y->depth = max(depth(y->left), depth(y->right)) + 1;
    x->depth = max(depth(x->left), depth(x->right)) + 1;

    y->parent = x->parent;
    x->parent = y;
    if(T2) T2->parent = x;

    return y;
  }

  static i64 getBalance(Node* node) { return node ? depth(node->left) - depth(node->right) : 0; }

  Node* minKeyNode() {
    if(!root) return nullptr;
    Node* node = root;
    while(node->left) node = node->left;
    return node;
  }

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
        node->left = rotateLeft(node->left);
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
        node->left = rotateRight(node->left);
        rotateLeft(node);
        node = node->parent;
        continue;
      }

      node = node->parent;
    }

    return new_node;
  }

  Node* deleteKey(KeyValue key) {
    if(!root) return nullptr;
    // TODO ...
  }


};

}



#endif // AVL_TREE_HXX
