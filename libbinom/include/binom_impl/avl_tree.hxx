#ifndef AVL_TREE_HXX
#define AVL_TREE_HXX

#include "../variables/key_value.hxx"
#include <functional>

namespace binom::priv {

using namespace type_alias;

class AVLNode {
public:

  enum class NodePosition : i8 {
    root = 0,
    left = -1,
    right = 1
  };

private:
  friend class AVLTree;
  i64 depth = 1;
  AVLNode* left = nullptr;
  AVLNode* right = nullptr;
  AVLNode* parent;
  KeyValue key;

  void swapPosition(AVLNode& other, AVLTree& avl_tree);
  void unpin(AVLTree& avl_tree);

public:

  AVLNode(KeyValue key, AVLNode* parent = nullptr);
  AVLNode(AVLNode& other);
  AVLNode(AVLNode&& other);

  AVLNode& operator=(AVLNode other);

  bool isRoot() const noexcept;
  bool isLeft() const noexcept;
  bool isRight() const noexcept;

  bool hasLeft() const noexcept;
  bool hasRight() const noexcept;
  bool hasChild() const noexcept;

  KeyValue& getKey();
  const KeyValue& getKey() const;
  NodePosition getPosition() const;

};

class AVLTree {
public:

  class Iterator {
    friend class AVLTree;
    mutable AVLNode* node = nullptr;
  public:
    Iterator(AVLNode* node = nullptr);
    Iterator(const Iterator& other);
    Iterator(Iterator&& other);

    Iterator& operator=(Iterator& other) noexcept;
    Iterator& operator=(Iterator&& other) noexcept;
    Iterator& operator=(AVLNode* node) noexcept;
    Iterator& operator=(AVLNode& node) noexcept;

    Iterator& operator++();
    Iterator& operator--();

    Iterator operator++(int);
    Iterator operator--(int);

    const Iterator& operator++() const;
    const Iterator& operator--() const;

    const Iterator operator++(int) const;
    const Iterator operator--(int) const;

    bool operator==(Iterator other) const noexcept;
    bool operator!=(Iterator other) const noexcept;

    AVLNode& operator*();
    AVLNode* operator->();

    const AVLNode& operator*() const;
    const AVLNode* operator->() const;

    static Iterator nulliterator() noexcept {return {nullptr};}

  };

  class ReverseIterator {
    friend class AVLTree;
    mutable AVLNode* node = nullptr;
  public:
    ReverseIterator(AVLNode* node = nullptr);
    ReverseIterator(const ReverseIterator& other);
    ReverseIterator(ReverseIterator&& other);

    ReverseIterator& operator=(ReverseIterator& other) noexcept;
    ReverseIterator& operator=(ReverseIterator&& other) noexcept;
    ReverseIterator& operator=(AVLNode* node) noexcept;
    ReverseIterator& operator=(AVLNode& node) noexcept;

    ReverseIterator& operator++();
    ReverseIterator& operator--();

    ReverseIterator operator++(int);
    ReverseIterator operator--(int);

    const ReverseIterator& operator++() const;
    const ReverseIterator& operator--() const;

    const ReverseIterator operator++(int) const;
    const ReverseIterator operator--(int) const;

    bool operator==(ReverseIterator other) const noexcept;
    bool operator!=(ReverseIterator other) const noexcept;

    AVLNode& operator*();
    AVLNode* operator->();

    const AVLNode& operator*() const;
    const AVLNode* operator->() const;

    static ReverseIterator nulliterator() noexcept {return {nullptr};}

  };

  typedef const Iterator ConstIterator;
  typedef const ReverseIterator ConstReverseIterator;

private:
  friend class AVLNode;
  friend class MapImplementation;

  AVLNode* root = nullptr;

  static i64 max(i64 a, i64 b) noexcept;
  static i64 depth(AVLNode* node) noexcept;

  AVLNode* rotateRight(AVLNode* y);
  AVLNode* rotateLeft(AVLNode* x);

  static i64 getBalance(AVLNode* node);

  static AVLNode* minKeyNode(AVLNode* node) noexcept;
  AVLNode* minKeyNode() const noexcept;
  static AVLNode* maxKeyNode(AVLNode* node) noexcept;
  AVLNode* maxKeyNode() const noexcept;

public:

  inline bool isEmpty() const noexcept {return !root;}

  AVLNode* insert(AVLNode* new_node);

  [[nodiscard]] AVLNode* extract(KeyValue key);
  AVLNode* extract(AVLNode* node);

  AVLNode* get(KeyValue key) const;

  Iterator begin() noexcept;
  Iterator end() noexcept;

  ReverseIterator rbegin() noexcept;
  ReverseIterator rend() noexcept;

  inline ConstIterator begin() const noexcept {return cbegin();}
  inline ConstIterator end() const noexcept {return cend();}

  inline ConstReverseIterator rbegin() const noexcept {return crbegin();}
  inline ConstReverseIterator rend() const noexcept {return crend();}

  ConstIterator cbegin() const noexcept;
  ConstIterator cend() const noexcept;

  ConstReverseIterator crbegin() const noexcept;
  ConstReverseIterator crend() const noexcept;

  void clear(std::function<void(AVLNode* deletable_element)> destructor = [](AVLNode* n){delete n;});

};

}


#endif // AVL_TREE_HXX
