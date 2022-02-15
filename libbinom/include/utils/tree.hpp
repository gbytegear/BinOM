#ifndef TREE_HPP
#define TREE_HPP

#include "type_aliases.hpp"

namespace binom {

using namespace type_alias;

template<class T, class TCmp>
class AVLTree {

  struct Node {
    T value;
    ui8 height;
    Node* left;
    Node* right;
  };

  Node* root;
public:

};

}



#endif // TREE_HPP
