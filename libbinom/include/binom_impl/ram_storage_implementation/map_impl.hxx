#ifndef MAP_IMPL_HXX
#define MAP_IMPL_HXX

#include <map>
#include <set>
#include <list>
#include "../../variables/key_value.hxx"
#include "../../variables/variable.hxx"
#include "index_impl.hxx"

namespace binom::priv {

class MapImplementation {
  std::set<TableImplementation*>* table_list = nullptr;
  std::set<index::Field, index::MapComparator> data;
public:
  typedef std::set<index::Field, index::MapComparator> ContainerType;
  using Iterator = index::Iterator;
  using ConstIterator = index::ConstIterator;
  using ReverseIterator = index::ReverseIterator;
  using ConstReverseIterator = index::ConstReverseIterator;

  MapImplementation(WeakLink owner, const literals::map& map);
  MapImplementation(WeakLink owner, const MapImplementation& other);
  MapImplementation(WeakLink owner, MapImplementation&& other);
  MapImplementation(const MapImplementation& other) = delete;
  MapImplementation(MapImplementation&& other) = delete;
  ~MapImplementation();

  bool isEmpty() const noexcept {return data.empty();}
  size_t getSize() const noexcept {return data.size();}
  bool contains(KeyValue value) const noexcept {return data.contains(std::move(value));}

  err::ProgressReport<FieldRef> insert(WeakLink owner, KeyValue key, Variable variable);

  err::Error remove(KeyValue key);

  err::ProgressReport<FieldRef> rename(KeyValue old_key, KeyValue new_key);

  FieldRef getOrInsertFieldRef(WeakLink owner, KeyValue key);

  Variable getVariable(KeyValue key) const;
  FieldRef getField(KeyValue key) const;

  Iterator find(KeyValue key);
  ReverseIterator rfind(KeyValue key);
  ConstIterator find(KeyValue key) const;
  ConstReverseIterator rfind(KeyValue key) const;

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

#endif // MAP_IMPL_HXX
