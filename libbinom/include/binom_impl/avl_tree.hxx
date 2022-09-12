#ifndef AVL_TREE_HXX
#define AVL_TREE_HXX

#include "../variables/key_value.hxx"
#include <functional>

namespace binom::priv {

using namespace type_alias;

class AVLTree {
  template<typename ValueType>
  friend class ValueStoringAVLTree;
public:

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

template <typename ValueType>
class ValueStoringAVLTree {
public:
  struct ValueStoringAVLNode;
private:
  AVLTree avl_tree;

  static inline ValueStoringAVLNode& convert(AVLTree::AVLNode& node) noexcept {return reinterpret_cast<ValueStoringAVLNode&>(node);}
  static inline const ValueStoringAVLNode& convert(const AVLTree::AVLNode& node) noexcept {return reinterpret_cast<const ValueStoringAVLNode&>(node);}
  static inline AVLTree::AVLNode& convert(ValueStoringAVLNode& node) noexcept {return reinterpret_cast<AVLTree::AVLNode&>(node);}
  static inline const AVLTree::AVLNode& convert(const ValueStoringAVLNode& node) noexcept {return reinterpret_cast<const AVLTree::AVLNode&>(node);}

  static inline ValueStoringAVLNode* convert(AVLTree::AVLNode* node) noexcept {return reinterpret_cast<ValueStoringAVLNode*>(node);}
  static inline const ValueStoringAVLNode* convert(const AVLTree::AVLNode* node) noexcept {return reinterpret_cast<const ValueStoringAVLNode*>(node);}
  static inline AVLTree::AVLNode* convert(ValueStoringAVLNode* node) noexcept {return reinterpret_cast<AVLTree::AVLNode*>(node);}
  static inline const AVLTree::AVLNode* convert(const ValueStoringAVLNode* node) noexcept {return reinterpret_cast<const AVLTree::AVLNode*>(node);}

public:

  class ValueStoringAVLNode {
  protected:
    AVLTree::AVLNode node;
    ValueType value;
  public:

    ValueStoringAVLNode(KeyValue key, ValueType value)
      : node(key), value(std::move(value)) {}
    ValueStoringAVLNode(ValueStoringAVLNode& other)
      : node(other.node), value(other.value) {}
    ValueStoringAVLNode(ValueStoringAVLNode&& other)
      : node(std::move(other.node)), value(std::move(other.value)) {}

    inline ValueStoringAVLNode& operator=(ValueStoringAVLNode other) {
      node = other.node;
      value = std::move(other.value);
      return self;
    }

    inline bool isRoot() const noexcept {return node.isRoot();}
    inline bool isLeft() const noexcept {return node.isLeft();}
    inline bool isRight() const noexcept {return node.isRight();}

    inline bool hasLeft() const noexcept {return node.hasLeft();}
    inline bool hasRight() const noexcept {return node.hasRight();}
    inline bool hasChild() const noexcept {return node.hasChild();}

    inline KeyValue& getKey() noexcept {return node.getKey();}
    inline const KeyValue& getKey() const noexcept {return node.getKey();}
    inline AVLTree::AVLNode::NodePosition getPosition() const noexcept {return node.getPosition();}

    inline ValueType& getValue() noexcept {return value;}
    inline const ValueType& getValue() const noexcept {return value;}
  };

  class Iterator {
    AVLTree::Iterator iterator;
  public:
    Iterator(ValueStoringAVLNode* node) : iterator(AVLTree::Iterator(convert(node))) {}
    Iterator(AVLTree::Iterator iterator) : iterator(iterator) {}
    Iterator(const Iterator& other) : iterator(other.iterator) {}
    Iterator(Iterator&& other) : iterator(std::move(other.iterator)) {}

    inline Iterator& operator=(Iterator& other) noexcept {iterator = other.iterator; return self;}
    inline Iterator& operator=(Iterator&& other) noexcept {iterator = std::move(other.iterator); return self;}

    inline Iterator& operator++() {++iterator; return self;}
    inline Iterator& operator--() {--iterator; return self;}

    inline Iterator operator++(int) {Iterator tmp(self); ++iterator; return tmp;}
    inline Iterator operator--(int) {Iterator tmp(self); --iterator; return tmp;}

    inline const Iterator& operator++() const {++iterator; return self;}
    inline const Iterator& operator--() const {--iterator; return self;}

    inline const Iterator operator++(int) const {Iterator tmp(self); ++iterator; return tmp;}
    inline const Iterator operator--(int) const {Iterator tmp(self); --iterator; return tmp;}

    inline bool operator==(Iterator other) const noexcept {return iterator == other.iterator;}
    inline bool operator!=(Iterator other) const noexcept {return iterator != other.iterator;}

    inline ValueStoringAVLNode& operator*() {return convert(*iterator);}
    inline ValueStoringAVLNode* operator->() {return &convert(*iterator);}

    inline const ValueStoringAVLNode& operator*() const {return convert(*iterator);}
    inline const ValueStoringAVLNode* operator->() const {return &convert(*iterator);}

    inline KeyValue& getKey() noexcept {return iterator->getKey();}
    inline const KeyValue& getKey() const noexcept {return iterator->getKey();}

    inline ValueType& getValue() noexcept {return self->getValue();}
    inline const ValueType& getValue() const noexcept {return self->getValue();}

    static Iterator nulliterator() noexcept {return {nullptr};}
  };

  class ReverseIterator {
    AVLTree::ReverseIterator iterator;
  public:
    ReverseIterator(ValueStoringAVLNode* node) : iterator(AVLTree::ReverseIterator(convert(node))) {}
    ReverseIterator(AVLTree::ReverseIterator iterator) : iterator(iterator) {}
    ReverseIterator(const ReverseIterator& other) : iterator(other.iterator) {}
    ReverseIterator(ReverseIterator&& other) : iterator(std::move(other.iterator)) {}

    inline ReverseIterator& operator=(ReverseIterator& other) noexcept {iterator = other.iterator; return self;}
    inline ReverseIterator& operator=(ReverseIterator&& other) noexcept {iterator = std::move(other.iterator); return self;}

    inline ReverseIterator& operator++() {++iterator; return self;}
    inline ReverseIterator& operator--() {--iterator; return self;}

    inline ReverseIterator operator++(int) {ReverseIterator tmp(self); ++iterator; return tmp;}
    inline ReverseIterator operator--(int) {ReverseIterator tmp(self); --iterator; return tmp;}

    inline const ReverseIterator& operator++() const {++iterator; return self;}
    inline const ReverseIterator& operator--() const {--iterator; return self;}

    inline const ReverseIterator operator++(int) const {ReverseIterator tmp(self); ++iterator; return tmp;}
    inline const ReverseIterator operator--(int) const {ReverseIterator tmp(self); --iterator; return tmp;}

    inline bool operator==(ReverseIterator other) const noexcept {return iterator == other.iterator;}
    inline bool operator!=(ReverseIterator other) const noexcept {return iterator != other.iterator;}

    inline ValueStoringAVLNode& operator*() {return convert(*iterator);}
    inline ValueStoringAVLNode* operator->() {return &convert(*iterator);}

    inline const ValueStoringAVLNode& operator*() const {return convert(*iterator);}
    inline const ValueStoringAVLNode* operator->() const {return &convert(*iterator);}

    inline KeyValue& getKey() noexcept {return iterator->getKey();}
    inline const KeyValue& getKey() const noexcept {return iterator->getKey();}

    inline ValueType& getValue() noexcept {return self->getValue();}
    inline const ValueType& getValue() const noexcept {return self->getValue();}

    static ReverseIterator nulliterator() noexcept {return {nullptr};}
  };

  typedef const Iterator ConstIterator;
  typedef const ReverseIterator ConstReverseIterator;

  inline void clear() { return avl_tree.clear([](AVLTree::AVLNode* node){delete convert(node);}); }
  ~ValueStoringAVLTree() { clear(); }

  inline bool isEmpty() const noexcept {return avl_tree.isEmpty();}

  inline ValueStoringAVLNode* insert(ValueStoringAVLNode& new_node) {return convert(avl_tree.insert(&convert(new_node)));}
  inline ValueStoringAVLNode* insert(KeyValue key, ValueType value) {return convert(avl_tree.insert(convert(new ValueStoringAVLNode(std::move(key), std::move(value)))));}

  [[nodiscard]] inline ValueStoringAVLNode* extract(KeyValue key) {return &convert(*avl_tree.extract(std::move(key)));}
  inline ValueStoringAVLNode* extract(ValueStoringAVLNode* node) {return &convert(*avl_tree.extract(&convert(node)));}

  inline ValueStoringAVLNode* get(KeyValue key) const {return &convert(*avl_tree.get(std::move(key)));}

  inline Iterator begin() noexcept {return avl_tree.begin();}
  inline Iterator end() noexcept {return avl_tree.end();}

  inline ReverseIterator rbegin() noexcept {return avl_tree.rbegin();}
  inline ReverseIterator rend() noexcept {return avl_tree.rend();}

  inline ConstIterator begin() const noexcept {return cbegin();}
  inline ConstIterator end() const noexcept {return cend();}

  inline ConstReverseIterator rbegin() const noexcept {return crbegin();}
  inline ConstReverseIterator rend() const noexcept {return crend();}

  inline ConstIterator cbegin() const noexcept {return avl_tree.cbegin();}
  inline ConstIterator cend() const noexcept {return avl_tree.cend();}

  inline ConstReverseIterator crbegin() const noexcept {return avl_tree.crbegin();}
  inline ConstReverseIterator crend() const noexcept {return avl_tree.crbegin();}

};


}


#endif // AVL_TREE_HXX
