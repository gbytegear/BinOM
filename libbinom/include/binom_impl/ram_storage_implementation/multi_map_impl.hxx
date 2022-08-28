#ifndef MULTIMAP_IMPL_HXX
#define MULTIMAP_IMPL_HXX

#include "../../variables/variable.hxx"
#include "../multi_avl_tree.hxx"

namespace binom::priv {

class MultiMapImplementation {

  struct MultiMapNode;

public:

  template<typename T>
  struct PseudoPointer{
    T value;
    T& operator*() noexcept {return value;}
    const T& operator*() const noexcept {return value;}
    T* operator->() noexcept {return &value;}
    const T* operator->() const noexcept {return &value;}
  };

  class ReverseIterator;

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

    inline Iterator begin() noexcept {return iterator.begin();}
    inline Iterator end() noexcept {return iterator.end();}

    inline ReverseIterator rbegin() noexcept {return iterator.rbegin();}
    inline ReverseIterator rend() noexcept {return iterator.rend();}

    inline Iterator begin() const noexcept {return cbegin();}
    inline Iterator end() const noexcept {return cend();}

    inline ReverseIterator rbegin() const noexcept {return crbegin();}
    inline ReverseIterator rend() const noexcept {return crend();}

    inline Iterator cbegin() const noexcept {return iterator.cbegin();}
    inline Iterator cend() const noexcept {return iterator.cend();}

    inline ReverseIterator crbegin() const noexcept {return iterator.crbegin();}
    inline ReverseIterator crend() const noexcept {return iterator.crend();}

    static Iterator nulliterator() noexcept {return MultiAVLTree::Iterator::nulliterator();}
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

    inline ReverseIterator begin() noexcept {return iterator.begin();}
    inline ReverseIterator end() noexcept {return iterator.end();}

    inline Iterator rbegin() noexcept {return iterator.rbegin();}
    inline Iterator rend() noexcept {return iterator.rend();}

    inline ReverseIterator begin() const noexcept {return cbegin();}
    inline ReverseIterator end() const noexcept {return cend();}

    inline Iterator rbegin() const noexcept {return crbegin();}
    inline Iterator rend() const noexcept {return crend();}

    inline ReverseIterator cbegin() const noexcept {return iterator.cbegin();}
    inline ReverseIterator cend() const noexcept {return iterator.cend();}

    inline Iterator crbegin() const noexcept {return iterator.crbegin();}
    inline Iterator crend() const noexcept {return iterator.crend();}

    static ReverseIterator nulliterator() noexcept {return MultiAVLTree::ReverseIterator::nulliterator();}
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

  MultiMapImplementation(const literals::multimap& map, NewNodePosition pos = NewNodePosition::back);
  MultiMapImplementation(const MultiMapImplementation& other);
  MultiMapImplementation(MultiMapImplementation&& other);
  ~MultiMapImplementation();

  bool isEmpty() const noexcept;
  size_t getSize() const noexcept;
  bool contains(KeyValue key) const;

  NamedVariable insert(KeyValue key, Variable variable, NewNodePosition position = NewNodePosition::back);
  Error remove(Iterator it);
  Error remove(ReverseIterator it);
  Error removeAll(KeyValue key);
  err::ProgressReport<NamedVariable> rename(Iterator it, KeyValue new_key);


  Iterator find(KeyValue key);
  ReverseIterator rfind(KeyValue key);
  Iterator findLast(KeyValue key);
  ReverseIterator rfindLast(KeyValue key);
  ConstIterator find(KeyValue key) const;
  ConstReverseIterator rfind(KeyValue key) const;
  ConstIterator findLast(KeyValue key) const;
  ConstReverseIterator rfindLast(KeyValue key) const;

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
