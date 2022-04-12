#ifndef TREE_HPP
#define TREE_HPP

#include "type_aliases.hxx"

namespace binom {

using namespace type_alias;

template<class T, class K, class KCmp>
class AVL {

  struct Node {
    T value;
    K key;
    ui8 height;
    Node* left;
    Node* right;
  };

  Node* root = nullptr;

  static ui8 getHeight(Node* node) noexcept { return node ? node->height : 0; }

  static i8 getBFactor(Node* node) noexcept { return node ? getHeight(node->right) - getHeight(node->left) : 0; }

  static void fixHeight(Node* node) noexcept {
    ui8 height_right = getHeight(node->right),
        height_left = getHeight(node->left);
    node->height = (height_left > height_right ? height_left : height_right) + 1;
  }

  static Node* rotateRight(Node* node) noexcept {
    Node* tmp = node->left;
    node->left = tmp->right;
    tmp->right = node;
    fixHeight(node);
    fixHeight(tmp);
    return tmp;
  }

  static Node* rotateLeft(Node* node) noexcept {
    Node* tmp = node->right;
    node->right = tmp->left;
    tmp->left = node;
    fixHeight(node);
    fixHeight(tmp);
    return tmp;
  }

  static Node* balanceNode(Node* node) {
    fixHeight(node);

    if (getBFactor(node) == 2) {
      if (getBFactor(node->right) < 0) node->right = rotateRight(node->right);
      return rotateLeft(node);
    } elif (getBFactor(node) == -2) {
      if(getBFactor(node->left) < 0) node->left = rotateLeft(node->left);
      return rotateRight(node);
    }
    return node;
  }

public:
  AVL() noexcept : root(nullptr) {}

};

}



#endif // TREE_HPP
