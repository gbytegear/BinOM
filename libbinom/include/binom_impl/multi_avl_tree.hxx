#ifndef MULTI_AVL_TREE_HXX
#define MULTI_AVL_TREE_HXX

#include "../variables/key_value.hxx"
#include <new>
#include <functional>

namespace binom::priv {

class MultiAVLTree {
  class AVLKeyNode;
public:
  class ReverseIterator;
  class Iterator;

  class AVLNode {
    friend class MultiAVLTree::AVLKeyNode;
    friend class MultiAVLTree::Iterator;
    friend class MultiAVLTree::ReverseIterator;
    AVLKeyNode* key_node = nullptr;
    AVLNode* next = nullptr;
    AVLNode* prev = nullptr;
  public:
    inline bool isIndexed() {return key_node;}
  };

private:

  class AVLKeyNode {
  public:

    enum class NodePosition : i8 {
      root = 0,
      left = -1,
      right = 1
    };

  private:
    friend class MultiAVLTree;
    i64 depth = 1;
    AVLKeyNode* left = nullptr;
    AVLKeyNode* right = nullptr;
    AVLKeyNode* parent = nullptr;
    KeyValue key;
    size_t size = 0;
    AVLNode* first = nullptr;
    AVLNode* last = nullptr;

    void swapPosition(AVLKeyNode& other, MultiAVLTree& avl_tree);
    void unpin(MultiAVLTree& avl_tree);

  public:

    class ReverseIterator;

    class Iterator {
      mutable AVLNode* node = nullptr;
    public:
      Iterator() = default;
      Iterator(AVLNode* node) noexcept;
      Iterator(const Iterator& other) noexcept;
      Iterator(Iterator&& other) noexcept;

      Iterator& operator=(Iterator other);

      Iterator& operator++();
      Iterator operator++(int);
      Iterator& operator--();
      Iterator operator--(int);

      const Iterator& operator++() const;
      const Iterator operator++(int) const;
      const Iterator& operator--() const;
      const Iterator operator--(int) const;

      operator bool() const noexcept {return node;}

      bool operator<=>(const Iterator&) const noexcept = default;

      bool operator==(Iterator other) const noexcept;
      bool operator!=(Iterator other) const noexcept;

      AVLNode& operator*();
      AVLNode* operator->() noexcept;

      const AVLNode& operator*() const;
      const AVLNode* operator->() const noexcept;

      operator ReverseIterator() noexcept;
      operator const ReverseIterator() const noexcept;
    };

    class ReverseIterator {
      mutable AVLNode* node = nullptr;
    public:
      ReverseIterator() = default;
      ReverseIterator(AVLNode* node) noexcept;
      ReverseIterator(const ReverseIterator& other) noexcept;
      ReverseIterator(ReverseIterator&& other) noexcept;

      ReverseIterator& operator=(ReverseIterator other);

      ReverseIterator& operator++();
      ReverseIterator operator++(int);
      ReverseIterator& operator--();
      ReverseIterator operator--(int);

      const ReverseIterator& operator++() const;
      const ReverseIterator operator++(int) const;
      const ReverseIterator& operator--() const;
      const ReverseIterator operator--(int) const;

      operator bool() const noexcept {return node;}

      bool operator<=>(const ReverseIterator&) const noexcept = default;

      bool operator==(ReverseIterator other) const noexcept;
      bool operator!=(ReverseIterator other) const noexcept;

      AVLNode& operator*();
      AVLNode* operator->() noexcept;

      const AVLNode& operator*() const;
      const AVLNode* operator->() const noexcept;

      operator Iterator() noexcept;
      operator const Iterator() const noexcept;
    };

    typedef const Iterator ConstIterator;
    typedef const ReverseIterator ConstReverseIterator;

    AVLKeyNode(KeyValue key, AVLKeyNode* parent = nullptr);
    AVLKeyNode(AVLKeyNode& other);
    AVLKeyNode(AVLKeyNode&& other);
    AVLKeyNode& operator=(AVLKeyNode other);

    void pushBack(AVLNode* node);
    void pushFront(AVLNode* node);
    AVLNode& extract(Iterator it);

    size_t getElementCount() const noexcept;
    bool isEmpty() const noexcept;

    bool isRoot() const noexcept;
    bool isLeft() const noexcept;
    bool isRight() const noexcept;
    bool hasLeft() const noexcept;
    bool hasRight() const noexcept;
    bool hasChild() const noexcept;

    KeyValue& getKey();

    const KeyValue& getKey() const;

    NodePosition getPosition() const;

    Iterator begin() noexcept;
    Iterator end() noexcept;
    ConstIterator begin() const noexcept;
    ConstIterator end() const noexcept;
    ConstIterator cbegin() const noexcept;
    ConstIterator cend() const noexcept;

    ReverseIterator rbegin() noexcept;
    ReverseIterator rend() noexcept;
    ConstReverseIterator rbegin() const noexcept;
    ConstReverseIterator rend() const noexcept;
    ConstReverseIterator crbegin() const noexcept;
    ConstReverseIterator crend() const noexcept;


  };

public:

  class Iterator {
    friend class MultiAVLTree;
    mutable AVLKeyNode::Iterator iterator;
  public:
    Iterator(AVLKeyNode* key_node);
    Iterator(AVLKeyNode::Iterator iterator);
    Iterator(const Iterator& other);
    Iterator(Iterator&& other);

    Iterator& goToNextKey();
    Iterator& goToPrevKey();
    const Iterator& goToNextKey() const;
    const Iterator& goToPrevKey() const;

    Iterator& operator=(Iterator other);
    Iterator& operator++();
    Iterator operator++(int);
    Iterator& operator--();
    Iterator operator--(int);

    const Iterator& operator++() const;
    const Iterator operator++(int) const;
    const Iterator& operator--() const;
    const Iterator operator--(int) const;

    operator bool() const {return iterator;}

    size_t getElementCount() const noexcept;

    AVLKeyNode& getKeyNode();
    const AVLKeyNode& getKeyNode() const;
    AVLKeyNode::Iterator getKeyNodeIterator();
    const AVLKeyNode::Iterator getKeyNodeIterator() const;
    KeyValue& getKey();
    const KeyValue& getKey() const;

    bool operator<=>(const Iterator&) const noexcept = default;

    bool operator==(Iterator other) const noexcept;
    bool operator!=(Iterator other) const noexcept;

    AVLNode& operator*();
    AVLNode* operator->() noexcept;

    const AVLNode& operator*() const;
    const AVLNode* operator->() const noexcept;

    operator ReverseIterator();
    operator const ReverseIterator() const;

    Iterator begin() noexcept;
    Iterator end() noexcept;

    ReverseIterator rbegin() noexcept;
    ReverseIterator rend() noexcept;

    const Iterator begin() const noexcept;
    const Iterator end() const noexcept;

    const ReverseIterator rbegin() const noexcept;
    const ReverseIterator rend() const noexcept;

    const Iterator cbegin() const noexcept;
    const Iterator cend() const noexcept;

    const ReverseIterator crbegin() const noexcept;
    const ReverseIterator crend() const noexcept;


  };

  class ReverseIterator {
    friend class MultiAVLTree;
    mutable AVLKeyNode::ReverseIterator reverse_iterator = nullptr;
  public:
    ReverseIterator(AVLKeyNode* key_node);
    ReverseIterator(AVLKeyNode::ReverseIterator iterator);
    ReverseIterator(const ReverseIterator& other);
    ReverseIterator(ReverseIterator&& other);

    ReverseIterator& goToNextKey();
    ReverseIterator& goToPrevKey();
    const ReverseIterator& goToNextKey() const;
    const ReverseIterator& goToPrevKey() const;

    ReverseIterator& operator=(ReverseIterator other);
    ReverseIterator& operator++();
    ReverseIterator operator++(int);
    ReverseIterator& operator--();
    ReverseIterator operator--(int);

    operator bool() const {return reverse_iterator;}

    const ReverseIterator& operator++() const;
    const ReverseIterator operator++(int) const;
    const ReverseIterator& operator--() const;
    const ReverseIterator operator--(int) const;

    size_t getElementCount() const noexcept;

    AVLKeyNode& getKeyNode();
    const AVLKeyNode& getKeyNode() const;
    AVLKeyNode::ReverseIterator getKeyNodeIterator();
    const AVLKeyNode::ReverseIterator getKeyNodeIterator() const;
    KeyValue& getKey();
    const KeyValue& getKey() const;

    bool operator<=>(const ReverseIterator&) const noexcept = default;

    bool operator==(ReverseIterator other) const noexcept;
    bool operator!=(ReverseIterator other) const noexcept;

    AVLNode& operator*();
    AVLNode* operator->() noexcept;

    const AVLNode& operator*() const;
    const AVLNode* operator->() const noexcept;

    operator Iterator();
    operator const Iterator() const;

    Iterator rbegin() noexcept;
    Iterator rend() noexcept;

    ReverseIterator begin() noexcept;
    ReverseIterator end() noexcept;

    const Iterator rbegin() const noexcept;
    const Iterator rend() const noexcept;

    const ReverseIterator begin() const noexcept;
    const ReverseIterator end() const noexcept;

    const Iterator crbegin() const noexcept;
    const Iterator crend() const noexcept;

    const ReverseIterator cbegin() const noexcept;
    const ReverseIterator cend() const noexcept;

  };

  typedef const Iterator ConstIterator;
  typedef const ReverseIterator ConstReverseIterator;

private:
  friend class AVLKeyNode;

  AVLKeyNode* root = nullptr;

  static i64 max(i64 a, i64 b) noexcept;
  static i64 depth(AVLKeyNode* node) noexcept;

  AVLKeyNode* rotateRight(AVLKeyNode* y);

  AVLKeyNode* rotateLeft(AVLKeyNode* x);

  static i64 getBalance(AVLKeyNode* node);

  static AVLKeyNode* minKeyNode(AVLKeyNode* node) noexcept;

  AVLKeyNode* minKeyNode() const noexcept;


  static AVLKeyNode* maxKeyNode(AVLKeyNode* node) noexcept;

  AVLKeyNode* maxKeyNode() const noexcept;

public:

  struct NodePair {
    AVLKeyNode* key_node;
    AVLNode* node;
  };

  enum class NewNodePosition : i8 {front = -1, back = 1};

  bool isEmpty() const noexcept;

  NodePair insert(KeyValue key, AVLNode* new_node, NewNodePosition position = NewNodePosition::back);

  AVLNode* extract(Iterator it);

  AVLNode* extract(ReverseIterator r_it);

  bool removeKey(KeyValue key, std::function<void(AVLNode* deletable_element)> destructor = [](AVLNode* n){delete n;});

  Iterator find(KeyValue key) const;

  Iterator findLast(KeyValue key) const;

  ReverseIterator rfind(KeyValue key) const;

  ReverseIterator rfindLast(KeyValue key) const;

  Iterator begin() noexcept;
  Iterator end() noexcept;

  ReverseIterator rbegin() noexcept;
  ReverseIterator rend() noexcept;

  ConstIterator begin() const noexcept;
  ConstIterator end() const noexcept;

  ConstReverseIterator rbegin() const noexcept;
  ConstReverseIterator rend() const noexcept;

  ConstIterator cbegin() const noexcept;
  ConstIterator cend() const noexcept;

  ConstReverseIterator crbegin() const noexcept;
  ConstReverseIterator crend() const noexcept;

  void clear(std::function<void(AVLNode* deletable_element)> destructor = [](AVLNode* n){delete n;});

};

}

#endif // MULTI_AVL_TREE_HXX
