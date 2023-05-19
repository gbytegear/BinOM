#ifndef MULTIMAP_IMPL_HXX
#define MULTIMAP_IMPL_HXX

#include <map>
#include "../../utils/pseudo_pointer.hxx"
#include "../../variables/key_value.hxx"
#include "../../variables/variable.hxx"
#include "index_impl.hxx"

namespace binom::priv {

class MultiMapImplementation {
  friend class TableImplementation;
  friend class binom::index::Field;

  std::set<TableImplementation*>* table_list = nullptr;
  std::multiset<index::Field, index::MapComparator> data;

public:
  using NewNodePosition = MultiAVLTree::NewNodePosition;
  typedef std::multiset<index::Field, index::MapComparator> ContainerType;
  using Iterator = index::Iterator;
  using ConstIterator = index::ConstIterator;
  using ReverseIterator = index::ReverseIterator;
  using ConstReverseIterator = index::ConstReverseIterator;

  MultiMapImplementation(WeakLink owner, const literals::multimap& map);
  MultiMapImplementation(WeakLink owner, const MultiMapImplementation& other);
  MultiMapImplementation(const MultiMapImplementation& other) = delete;
  MultiMapImplementation(MultiMapImplementation&& other) = delete;
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

  void addTable(TableImplementation& table);
  Error removeTable(TableImplementation& table);
  bool updateKey(index::Field& field, const KeyValue& new_key);
};

}

#endif // MULTIMAP_IMPL_HXX
