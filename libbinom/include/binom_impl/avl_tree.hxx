#ifndef AVL_TREE_HXX
#define AVL_TREE_HXX

#include "types.hxx"
#include "../variables/key_value.hxx"
#include "../utils/reverse_iterator.hxx"

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

    void swapPosition(Node& other, AVLTree& avl_tree);
    void unpin(AVLTree& avl_tree);

  public:

    Node(KeyValue key, Node* parent = nullptr);
    Node(Node& other);
    Node(Node&& other);

    Node& operator=(Node other);

    bool isRoot() const noexcept;
    bool isLeft() const noexcept;
    bool isRight() const noexcept;

    bool hasLeft() const noexcept;
    bool hasRight() const noexcept;
    bool hasChild() const noexcept;

    KeyValue& getKey();
    NodePosition getPosition();

  };

  class Iterator {
    friend class AVLTree;
    Node* node = nullptr;
    Iterator(Node* node = nullptr) : node(node) {}
  public:
    Iterator(const Iterator& other) : node(other.node) {}
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

    Iterator operator++(int) {Iterator tmp(self); ++self; return tmp;}
    Iterator operator--(int) {Iterator tmp(self); --self; return tmp;}

    bool operator==(Iterator other) noexcept {return node == other.node;}
    bool operator!=(Iterator other) noexcept {return node != other.node;}

    Node& operator*() {return *node;}
    Node* operator->() {return node;}

  };

  class ReverseIterator {
    friend class AVLTree;
    Node* node = nullptr;
    ReverseIterator(Node* node = nullptr) : node(node) {}
  public:
    ReverseIterator(ReverseIterator& other) : node(other.node) {}
    ReverseIterator(ReverseIterator&& other) : node(other.node) {}

    ReverseIterator& operator=(ReverseIterator& other) noexcept {return *new(this) ReverseIterator(other);}
    ReverseIterator& operator=(ReverseIterator&& other) noexcept {return *new(this) ReverseIterator(std::move(other));}
    ReverseIterator& operator=(Node* node) noexcept {return *new(this) ReverseIterator(node);}
    ReverseIterator& operator=(Node& node) noexcept {return *new(this) ReverseIterator(&node);}

    ReverseIterator& operator++() {
      if(node->hasLeft()) node = AVLTree::maxKeyNode(node->left);
      elif(node->isRight()) node = node->parent;
      elif(node->isLeft()) {
        Node* tmp = node->parent;
        node = node->parent->parent;
        while (tmp->isLeft()) {tmp = node; node = node->parent;}
      }
      return *this;
    }

    ReverseIterator& operator--() {
      if(node->hasRight()) node = AVLTree::minKeyNode(node->right);
      elif(node->isLeft()) node = node->parent;
      elif(node->isRight()) {
        Node* tmp = node->parent;
        node = node->parent->parent;
        while (tmp->isRight()) {tmp = node; node = node->parent;}
      }
      return *this;
    }

    ReverseIterator operator++(int) {ReverseIterator tmp(self); ++self; return tmp;}
    ReverseIterator operator--(int) {ReverseIterator tmp(self); --self; return tmp;}

    bool operator==(ReverseIterator other) noexcept {return node == other.node;}
    bool operator!=(ReverseIterator other) noexcept {return node != other.node;}

    Node& operator*() {return *node;}
    Node* operator->() {return node;}

  };

private:

  Node* root = nullptr;

  static i64 max(i64 a, i64 b) noexcept;
  static i64 depth(Node* node) noexcept;

  Node* rotateRight(Node* y);
  Node* rotateLeft(Node* x);

  static i64 getBalance(Node* node);

  static Node* minKeyNode(Node* node) noexcept;
  Node* minKeyNode() const noexcept;
  static Node* maxKeyNode(Node* node) noexcept;
  Node* maxKeyNode() const noexcept;

public:

  inline bool isEmpty() const noexcept {return !root;}

  Node* insert(Node* new_node);

  [[nodiscard]] Node* extract(KeyValue key);
  Node* extract(Node* node);

  Node* get(KeyValue key) const;

  Iterator begin() noexcept;
  Iterator end();

  ReverseIterator rbegin() noexcept;
  ReverseIterator rend();

};

}


#endif // AVL_TREE_HXX
