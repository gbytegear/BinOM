#ifndef MULTIMAP_IMPL_HXX
#define MULTIMAP_IMPL_HXX

//#include "../multi_avl_tree.hxx"
//#include "../../variables/variable.hxx"
//#include "../../utils/pseudo_pointer.hxx"
#include <map>
#include "../../utils/pseudo_pointer.hxx"
#include "../../variables/key_value.hxx"
#include "../../variables/variable.hxx"
#include "index_impl.hxx"

namespace binom::priv {

class MultiMapImplementation {
  std::list<Link>* table_list = nullptr;
  std::set<index::Field, index::MapComparator> data;
public:
  using NewNodePosition = MultiAVLTree::NewNodePosition;
  typedef std::set<index::Field, index::MapComparator> ContainerType;
  using Iterator = index::Iterator;
  using ConstIterator = index::ConstIterator;
  using ReverseIterator = index::ReverseIterator;
  using ConstReverseIterator = index::ConstReverseIterator;

  MultiMapImplementation(WeakLink owner, const literals::multimap& map);
  MultiMapImplementation(WeakLink owner, const MultiMapImplementation& other);
  ~MultiMapImplementation();

  bool isEmpty() const noexcept {return data.empty();}
  size_t getSize() const noexcept {return data.size();}
  bool contains(KeyValue value) const noexcept {return data.contains(std::move(value));}

  FieldRef insert(WeakLink owner, KeyValue key, Variable variable, NewNodePosition position = NewNodePosition::back);

  void remove(Iterator it);
  void remove(ReverseIterator it);
  void removeAll(KeyValue key);
  err::ProgressReport<FieldRef> rename(Iterator it, KeyValue new_key);


  std::pair<Iterator, Iterator> getRange(KeyValue key);
  std::pair<ReverseIterator, ReverseIterator> getReverseRange(KeyValue key);

  Iterator find(KeyValue key);
  ReverseIterator rfind(KeyValue key);
  Iterator findLast(KeyValue key);
  ReverseIterator rfindLast(KeyValue key);

  std::pair<ConstIterator, ConstIterator> getRange(KeyValue key) const;
  std::pair<ConstReverseIterator, ConstReverseIterator> getReverseRange(KeyValue key) const;

  ConstIterator find(KeyValue key) const;
  ConstReverseIterator rfind(KeyValue key) const;
  ConstIterator findLast(KeyValue key) const;
  ConstReverseIterator rfindLast(KeyValue key) const;

  void clear();

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


};




/*

class MultiMapImplementation {

public:
  typedef std::multimap<KeyValue, Variable> VariableMultiMap;
  using NamedVariable = MapNodeRef;

  class Iterator : public VariableMultiMap::iterator {
  public:
    Iterator(VariableMultiMap::iterator map_it);
    Iterator(const Iterator& other);
    Iterator(Iterator&& other);
    NamedVariable operator*();
    pseudo_ptr::PseudoPointer<NamedVariable> operator->();
    const NamedVariable operator*() const;
    pseudo_ptr::PseudoPointer<const NamedVariable> operator->() const;
    static Iterator nullit() noexcept;
  };

  class ReverseIterator : public VariableMultiMap::reverse_iterator {
  public:
    ReverseIterator(VariableMultiMap::reverse_iterator map_rit);
    ReverseIterator(const ReverseIterator& other);
    ReverseIterator(ReverseIterator&& other);
    NamedVariable operator*();
    pseudo_ptr::PseudoPointer<NamedVariable> operator->();
    const NamedVariable operator*() const;
    pseudo_ptr::PseudoPointer<const NamedVariable> operator->() const;
    static ReverseIterator nullit() noexcept;
  };

  class ConstIterator : public VariableMultiMap::const_iterator {
  public:
    ConstIterator(VariableMultiMap::const_iterator map_it);
    ConstIterator(const ConstIterator& other);
    ConstIterator(ConstIterator&& other);
    const NamedVariable operator*() const;
    pseudo_ptr::PseudoPointer<const NamedVariable> operator->() const;
    static ConstIterator nullit() noexcept;
  };

  class ConstReverseIterator : public VariableMultiMap::const_reverse_iterator {
  public:
    ConstReverseIterator(VariableMultiMap::const_reverse_iterator map_rit);
    ConstReverseIterator(const ConstReverseIterator& other);
    ConstReverseIterator(ConstReverseIterator&& other);
    const NamedVariable operator*() const;
    pseudo_ptr::PseudoPointer<const NamedVariable> operator->() const;
    static ConstReverseIterator nullit() noexcept;
  };

private:
  VariableMultiMap storage;

public:
  using NewNodePosition = MultiAVLTree::NewNodePosition;

  MultiMapImplementation(const literals::multimap& map, NewNodePosition pos = NewNodePosition::back);
  MultiMapImplementation(const MultiMapImplementation& other);
  MultiMapImplementation(MultiMapImplementation&& other);
  ~MultiMapImplementation() = default;

  bool isEmpty() const noexcept;
  size_t getSize() const noexcept;
  bool contains(KeyValue key) const;

  NamedVariable insert(KeyValue key, Variable variable, NewNodePosition position = NewNodePosition::back);

  void remove(Iterator it);
  void remove(ReverseIterator it);
  size_t removeAll(KeyValue key);
  err::ProgressReport<NamedVariable> rename(Iterator it, KeyValue new_key);


  std::pair<Iterator, Iterator> getRange(KeyValue key);
  std::pair<ReverseIterator, ReverseIterator> getReverseRange(KeyValue key);

  Iterator find(KeyValue key);
  ReverseIterator rfind(KeyValue key);
  Iterator findLast(KeyValue key);
  ReverseIterator rfindLast(KeyValue key);

  std::pair<ConstIterator, ConstIterator> getRange(KeyValue key) const;
  std::pair<ConstReverseIterator, ConstReverseIterator> getReverseRange(KeyValue key) const;

  ConstIterator find(KeyValue key) const;
  ConstReverseIterator rfind(KeyValue key) const;
  ConstIterator findLast(KeyValue key) const;
  ConstReverseIterator rfindLast(KeyValue key) const;

  void clear();

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

};

/**/

}

#endif // MULTIMAP_IMPL_HXX
