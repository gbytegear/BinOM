#ifndef MAP_IMPL_HXX
#define MAP_IMPL_HXX

#include "../../variables/variable.hxx"
#include "../avl_tree.hxx"
#include "../../utils/pseudo_pointer.hxx"

namespace binom::priv {

class MapImplementation {
  using AVLNode = AVLTree::AVLNode;
  using ValueStoringAVLNode = ValueStoringAVLTree<Variable>::ValueStoringAVLNode;
  typedef ValueStoringAVLTree<Variable> VariableAVLTree;
public:

  class NamedVariable;

  class Iterator {
    friend class priv::MapImplementation;
    VariableAVLTree::Iterator iterator;
  public:
    Iterator(VariableAVLTree::Iterator iterator);
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
    pseudo_ptr::PseudoPointer<NamedVariable> operator ->();

    const NamedVariable operator*() const;
    const pseudo_ptr::PseudoPointer<NamedVariable> operator->() const;

    static Iterator nulliterator() noexcept {return {nullptr};}
  };


  class ReverseIterator {
    friend class priv::MapImplementation;
    VariableAVLTree::ReverseIterator iterator;
  public:
    ReverseIterator(VariableAVLTree::ReverseIterator iterator);
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

    bool operator==(const ReverseIterator other) const noexcept;
    bool operator!=(const ReverseIterator other) const noexcept;

    NamedVariable operator*();
    pseudo_ptr::PseudoPointer<NamedVariable> operator ->();

    const NamedVariable operator*() const;
    const pseudo_ptr::PseudoPointer<NamedVariable> operator->() const;

    static ReverseIterator nulliterator() noexcept {return {nullptr};}
  };

  typedef const Iterator ConstIterator;
  typedef const ReverseIterator ConstReverseIterator;

private:
  size_t size;
  VariableAVLTree avl_tree;

public:

  MapImplementation(const literals::map& map);
  MapImplementation(const MapImplementation& other);
  ~MapImplementation();

  bool isEmpty() const noexcept;
  size_t getSize() const noexcept;
  bool contains(KeyValue value) const;

  err::ProgressReport<NamedVariable> insert(KeyValue key, Variable variable);
  err::Error remove(KeyValue key);
  err::ProgressReport<NamedVariable> rename(KeyValue old_key, KeyValue new_key);

  NamedVariable getOrInsertNamedVariable(KeyValue key);
  Variable getVariable(KeyValue key);

  Iterator find(KeyValue key);
  ReverseIterator rfind(KeyValue key);
  ConstIterator find(KeyValue key) const;
  ConstReverseIterator rfind(KeyValue key) const;

  void clear();

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
  inline ConstReverseIterator crend() const noexcept {return avl_tree.crend();}

};

}

#endif // MAP_IMPL_HXX
