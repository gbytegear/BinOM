#ifndef MULTIMAP_IMPL_HXX
#define MULTIMAP_IMPL_HXX

#include "../../variables/variable.hxx"
#include "../multi_avl_tree.hxx"

namespace binom::priv {

class MultiMapImplementation {

  struct MultiMapNode {
    MultiAVLTree::AVLNode node;
    Variable variable;
  };

public:

  template<typename T>
  struct PseudoPointer{
    T value;
    T& operator*() noexcept {return value;}
    const T& operator*() const noexcept {return value;}
    T* operator->() noexcept {return &value;}
    const T* operator->() const noexcept {return &value;}
  }

  class NamedVariable;

  class Iterator {
    friend class priv::MultiMapImplementation;
    MultiAVLTree::Iterator iterator;
    Iterator(MultiAVLTree::Iterator iterator);
  public:
    Iterator(const Iterator& iterator);
    Iterator(Iterator&& iterator);

    Iterator& operator=(Iterator& other) noexcept;
    Iterator& operator=(Iterator&& other) noexcept;

    Iterator& operator++();
    Iterator& operator--();

    Iterator operator++(int);
    Iterator operator--(int);

    const Iterator& operator++() const;
    const Iterator& operator--() const;

    const Iterator operator++(int) const;
    const Iterator operator--(int) const;

    bool operator==(const Iterator other) const noexcept;
    bool operator!=(const Iterator other) const noexcept;

    NamedVariable operator*();
    PseudoPointer<NamedVariable> operator->();

    const NamedVariable operator*() const;
    const PseudoPointer<NamedVariable> operator->() const;
  };


  class ReverseIterator {
    friend class priv::MultiMapImplementation;
    MultiAVLTree::ReverseIterator iterator;
    ReverseIterator(MultiAVLTree::ReverseIterator iterator);
  public:
    ReverseIterator(const ReverseIterator& iterator);
    ReverseIterator(ReverseIterator&& iterator);

    ReverseIterator& operator=(ReverseIterator& other) noexcept;
    ReverseIterator& operator=(ReverseIterator&& other) noexcept;

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

    NamedVariable operator*();
    PseudoPointer<NamedVariable> operator->();

    const NamedVariable operator*() const;
    const PseudoPointer<NamedVariable> operator->() const;
  };

  typedef const Iterator ConstIterator;
  typedef const ReverseIterator ConstReverseIterator;

private:
  size_t size;
  MultiAVLTree multi_avl_tree;

  static MultiMapNode* convert(MultiAVLTree::AVLNode* node);
  static const MultiMapNode* convert(const MultiAVLTree::AVLNode* node);

public:
  using NewNodePosition = MultiAVLTree::NewNodePosition;

  MultiMapImplementation(const literals::multimap& map);
  MultiMapImplementation(const MultiMapImplementation& other);
  ~MultiMapImplementation();

  bool isEmpty() const noexcept;
  size_t getSize() const noexcept;
  bool contains(KeyValue value) const;

  NamedVariable insert(KeyValue key, Variable variable, NewNodePosition position = NewNodePosition::back);
  void remove(Iterator it);
  void remove(ReverseIterator it);
  err::ProgressReport<NamedVariable> rename(Iterator it, KeyValue new_key);

  NamedVariable& getOrInsertNamedVariable(KeyValue key);
  Variable getVariable(KeyValue key);

  void clear();

  inline Iterator begin() noexcept {return multi_avl_tree.begin();}
  inline Iterator end() noexcept {return multi_avl_tree.end();}

  inline ReverseIterator rbegin() noexcept {return multi_avl_tree.rbegin();}
  inline ReverseIterator rend() noexcept {return multi_avl_tree.rend();}

  inline ConstIterator begin() const noexcept {return cbegin();}
  inline ConstIterator end() const noexcept {return cend();}

  inline ConstReverseIterator rbegin() const noexcept {return crbegin();}
  inline ConstReverseIterator rend() const noexcept {return crend();}

  inline ConstIterator cbegin() const noexcept {return multi_avl_tree.cbegin();}
  inline ConstIterator cend() const noexcept {return multi_avl_tree.cend();}

  inline ConstReverseIterator crbegin() const noexcept {return multi_avl_tree.crbegin();}
  inline ConstReverseIterator crend() const noexcept {return multi_avl_tree.crend();}

};

/**/

}

#endif // MULTIMAP_IMPL_HXX
