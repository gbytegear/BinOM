#ifndef MAP_IMPL_HXX
#define MAP_IMPL_HXX

#include <map>
#include <set>
#include <list>
#include "../../utils/pseudo_pointer.hxx"
#include "../../variables/key_value.hxx"
#include "../../variables/variable.hxx"
#include "index_impl.hxx"

namespace binom::priv {

class MapImplementation {
  std::list<Link>* index_list = nullptr;
  std::set<index::Field, index::MapComparator> data;
public:
  typedef std::set<index::Field, index::MapComparator> ContainerType;

  class Iterator : public ContainerType::iterator {
  public:
    Iterator(ContainerType::iterator map_it);
    Iterator(const Iterator& other);
    Iterator(Iterator&& other);
    NamedVariable operator*();
    pseudo_ptr::PseudoPointer<NamedVariable> operator->();
    const NamedVariable operator*() const;
    pseudo_ptr::PseudoPointer<const NamedVariable> operator->() const;
    static Iterator nullit() noexcept {return  ContainerType::iterator(nullptr);}
  };

  class ReverseIterator : public ContainerType::reverse_iterator {
  public:
    ReverseIterator(ContainerType::reverse_iterator map_rit);
    ReverseIterator(const ReverseIterator& other);
    ReverseIterator(ReverseIterator&& other);
    NamedVariable operator*();
    pseudo_ptr::PseudoPointer<NamedVariable> operator->();
    const NamedVariable operator*() const;
    pseudo_ptr::PseudoPointer<const NamedVariable> operator->() const;
    static ReverseIterator nullit() noexcept {return  ContainerType::reverse_iterator(Iterator::nullit());}
  };

  class ConstIterator : public ContainerType::const_iterator {
  public:
    ConstIterator(ContainerType::const_iterator map_it);
    ConstIterator(const ConstIterator& other);
    ConstIterator(ConstIterator&& other);
    const NamedVariable operator*() const;
    pseudo_ptr::PseudoPointer<const NamedVariable> operator->() const;
    static ConstIterator nullit() noexcept {return  ContainerType::const_iterator(nullptr);}
  };

  class ConstReverseIterator : public ContainerType::const_reverse_iterator {
  public:
    ConstReverseIterator(ContainerType::const_reverse_iterator map_rit);
    ConstReverseIterator(const ConstReverseIterator& other);
    ConstReverseIterator(ConstReverseIterator&& other);
    const NamedVariable operator*() const;
    pseudo_ptr::PseudoPointer<const NamedVariable> operator->() const;
    static ConstReverseIterator nullit() noexcept {return  ContainerType::const_reverse_iterator(ConstIterator::nullit());}
  };

  MapImplementation(WeakLink owner, const literals::map& map);
//  MapImplementation(const MapImplementation& other);
//  MapImplementation(MapImplementation&& other);
  ~MapImplementation();

  bool isEmpty() const noexcept {return data.empty();}
  size_t getSize() const noexcept {return data.size();}
  bool contains(KeyValue value) const noexcept {return data.contains(std::move(value));}

  err::ProgressReport<NamedVariable> insert(WeakLink owner, KeyValue key, Variable variable) {
    auto insert_result = data.emplace(owner, std::move(key), variable.move());
    if(insert_result.second) return MapImplementation::NamedVariable(*insert_result.first);
    return err::ErrorType::binom_key_unique_error;
  }

  err::Error remove(KeyValue key);

  err::ProgressReport<NamedVariable> rename(KeyValue old_key, KeyValue new_key);

  NamedVariable getOrInsertNamedVariable(KeyValue key);

  Variable getVariable(KeyValue key) const;

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
//  using NamedVariable = MapNodeRef;

//  class Iterator : public VariableMap::iterator {
//  public:
//    Iterator(VariableMap::iterator map_it);
//    Iterator(const Iterator& other);
//    Iterator(Iterator&& other);
//    NamedVariable operator*();
//    pseudo_ptr::PseudoPointer<NamedVariable> operator->();
//    const NamedVariable operator*() const;
//    pseudo_ptr::PseudoPointer<const NamedVariable> operator->() const;
//    static Iterator nullit() noexcept {return  VariableMap::iterator(nullptr);}
//  };

//  class ReverseIterator : public VariableMap::reverse_iterator {
//  public:
//    ReverseIterator(VariableMap::reverse_iterator map_rit);
//    ReverseIterator(const ReverseIterator& other);
//    ReverseIterator(ReverseIterator&& other);
//    NamedVariable operator*();
//    pseudo_ptr::PseudoPointer<NamedVariable> operator->();
//    const NamedVariable operator*() const;
//    pseudo_ptr::PseudoPointer<const NamedVariable> operator->() const;
//    static ReverseIterator nullit() noexcept {return  VariableMap::reverse_iterator(Iterator::nullit());}
//  };

//  class ConstIterator : public VariableMap::const_iterator {
//  public:
//    ConstIterator(VariableMap::const_iterator map_it);
//    ConstIterator(const ConstIterator& other);
//    ConstIterator(ConstIterator&& other);
//    const NamedVariable operator*() const;
//    pseudo_ptr::PseudoPointer<const NamedVariable> operator->() const;
//    static ConstIterator nullit() noexcept {return  VariableMap::const_iterator(nullptr);}
//  };

//  class ConstReverseIterator : public VariableMap::const_reverse_iterator {
//  public:
//    ConstReverseIterator(VariableMap::const_reverse_iterator map_rit);
//    ConstReverseIterator(const ConstReverseIterator& other);
//    ConstReverseIterator(ConstReverseIterator&& other);
//    const NamedVariable operator*() const;
//    pseudo_ptr::PseudoPointer<const NamedVariable> operator->() const;
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

//  err::ProgressReport<NamedVariable> insert(KeyValue key, Variable variable);

//  err::Error remove(KeyValue key);

//  err::ProgressReport<NamedVariable> rename(KeyValue old_key, KeyValue new_key);

//  NamedVariable getOrInsertNamedVariable(KeyValue key);

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
