#ifndef MAP_IMPL_HXX
#define MAP_IMPL_HXX

#include "../../variables/variable.hxx"
#include "../avl_tree.hxx"

namespace binom::priv {

class MapImplementation {
public:

  class NamedVariable;

  class Iterator {
    friend class priv::MapImplementation;
    AVLTree::Iterator iterator;
    Iterator(AVLTree::Iterator iterator);
  public:
    Iterator(const Iterator& iterator);
    Iterator(Iterator&& iterator);

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

    bool operator==(const Iterator other) const noexcept;
    bool operator!=(const Iterator other) const noexcept;

    NamedVariable& operator*();
    NamedVariable* operator->();

    const NamedVariable& operator*() const;
    const NamedVariable* operator->() const;
  };


  class ReverseIterator {
    friend class priv::MapImplementation;
    AVLTree::ReverseIterator iterator;
    ReverseIterator(AVLTree::ReverseIterator iterator);
  public:
    ReverseIterator(const ReverseIterator& iterator);
    ReverseIterator(ReverseIterator&& iterator);

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

    NamedVariable& operator*();
    NamedVariable* operator->();

    const NamedVariable& operator*() const;
    const NamedVariable* operator->() const;
  };

  typedef const Iterator ConstIterator;
  typedef const ReverseIterator ConstReverseIterator;

private:
  size_t size;
  AVLTree avl_tree;

  static NamedVariable* convert(AVLNode* node);
  static const NamedVariable* convert(const AVLNode* node);

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

  NamedVariable& getOrInsertNamedVariable(KeyValue key);
  Variable getVariable(KeyValue key);

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
