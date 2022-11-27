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
  std::list<Link>* table_list = nullptr;
  std::set<index::Field, index::MapComparator> data;
public:
  typedef std::set<index::Field, index::MapComparator> ContainerType;
  using Iterator = index::Iterator;
  using ConstIterator = index::ConstIterator;
  using ReverseIterator = index::ReverseIterator;
  using ConstReverseIterator = index::ConstReverseIterator;

  MapImplementation(WeakLink owner, const literals::map& map);
  MapImplementation(WeakLink owner, const MapImplementation& other);
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

};

//class MapImplementation {
//public:
//  typedef std::map<KeyValue, Variable> VariableMap;
//  using FieldRef = MapNodeRef;

//  class Iterator : public VariableMap::iterator {
//  public:
//    Iterator(VariableMap::iterator map_it);
//    Iterator(const Iterator& other);
//    Iterator(Iterator&& other);
//    FieldRef operator*();
//    pseudo_ptr::PseudoPointer<FieldRef> operator->();
//    const FieldRef operator*() const;
//    pseudo_ptr::PseudoPointer<const FieldRef> operator->() const;
//    static Iterator nullit() noexcept {return  VariableMap::iterator(nullptr);}
//  };

//  class ReverseIterator : public VariableMap::reverse_iterator {
//  public:
//    ReverseIterator(VariableMap::reverse_iterator map_rit);
//    ReverseIterator(const ReverseIterator& other);
//    ReverseIterator(ReverseIterator&& other);
//    FieldRef operator*();
//    pseudo_ptr::PseudoPointer<FieldRef> operator->();
//    const FieldRef operator*() const;
//    pseudo_ptr::PseudoPointer<const FieldRef> operator->() const;
//    static ReverseIterator nullit() noexcept {return  VariableMap::reverse_iterator(Iterator::nullit());}
//  };

//  class ConstIterator : public VariableMap::const_iterator {
//  public:
//    ConstIterator(VariableMap::const_iterator map_it);
//    ConstIterator(const ConstIterator& other);
//    ConstIterator(ConstIterator&& other);
//    const FieldRef operator*() const;
//    pseudo_ptr::PseudoPointer<const FieldRef> operator->() const;
//    static ConstIterator nullit() noexcept {return  VariableMap::const_iterator(nullptr);}
//  };

//  class ConstReverseIterator : public VariableMap::const_reverse_iterator {
//  public:
//    ConstReverseIterator(VariableMap::const_reverse_iterator map_rit);
//    ConstReverseIterator(const ConstReverseIterator& other);
//    ConstReverseIterator(ConstReverseIterator&& other);
//    const FieldRef operator*() const;
//    pseudo_ptr::PseudoPointer<const FieldRef> operator->() const;
//    static ConstReverseIterator nullit() noexcept {return  VariableMap::const_reverse_iterator(ConstIterator::nullit());}
//  };

//private:
//  VariableMap storage;

//public:

//  MapImplementation(const literals::map& map);
//  MapImplementation(const MapImplementation& other);
//  MapImplementation(MapImplementation&& other);
//  ~MapImplementation();

//  bool isEmpty() const noexcept;
//  size_t getSize() const noexcept;
//  bool contains(KeyValue value) const noexcept;

//  err::ProgressReport<FieldRef> insert(KeyValue key, Variable variable);

//  err::Error remove(KeyValue key);

//  err::ProgressReport<FieldRef> rename(KeyValue old_key, KeyValue new_key);

//  FieldRef getOrInsertFieldRef(KeyValue key);

//  Variable getVariable(KeyValue key) const;

//  Iterator find(KeyValue key);
//  ReverseIterator rfind(KeyValue key);
//  ConstIterator find(KeyValue key) const;
//  ConstReverseIterator rfind(KeyValue key) const;

//  void clear();

//  Iterator begin() noexcept;
//  Iterator end() noexcept;

//  ReverseIterator rbegin() noexcept;
//  ReverseIterator rend() noexcept;

//  ConstIterator begin() const noexcept;
//  ConstIterator end() const noexcept;

//  ConstReverseIterator rbegin() const noexcept;
//  ConstReverseIterator rend() const noexcept;

//  ConstIterator cbegin() const noexcept;
//  ConstIterator cend() const noexcept;

//  ConstReverseIterator crbegin() const noexcept;
//  ConstReverseIterator crend() const noexcept;

//};

}

#endif // MAP_IMPL_HXX
