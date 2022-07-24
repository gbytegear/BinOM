#ifndef MAP_HXX
#define MAP_HXX

#include "variable.hxx"

namespace binom {

class Map : public Variable {

  priv::MapImplementation* getData() const { return resource_link->data.map_implementation; }

public:
  typedef priv::MapImplementation::Iterator             Iterator;
  typedef priv::MapImplementation::ReverseIterator      ReverseIterator;
  typedef priv::MapImplementation::ConstIterator        ConstIterator;
  typedef priv::MapImplementation::ConstReverseIterator ConstReverseIterator;


  Map() : Variable(literals::map{}) {}
  Map(literals::map element_list)
    : Variable(std::move(element_list)) {}
  Map(const Map& other); // TODO
  Map(Map&& other); // TODO

  bool isEmpty() const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return 0;
    return getData()->isEmpty();
  }

  size_t getSize() const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return 0;
    return getData()->getSize();
  }

  bool contains(KeyValue value) const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return false;
    return getData()->contains(std::move(value));
  }

  err::ProgressReport<NamedVariable> insert(KeyValue key, Variable variable) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return err::ErrorType::binom_resource_not_available;
    return getData()->insert(std::move(key), variable.move());
  }

  err::Error remove(KeyValue key) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return err::ErrorType::binom_resource_not_available;
    return getData()->remove(std::move(key));
  }

  err::ProgressReport<NamedVariable> rename(KeyValue old_key, KeyValue new_key) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return err::ErrorType::binom_resource_not_available;
    return getData()->rename(std::move(old_key), std::move(new_key));
  }

  Variable getVariable(KeyValue key) {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return nullptr;
    return getData()->getVariable(std::move(key));
  }

  Variable operator[] (KeyValue key) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return nullptr;
    return getData()->getOrInsertNamedVariable(std::move(key)).getVariable();
  }

  const Variable getVariable(KeyValue key) const {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return nullptr;
    return getData()->getVariable(std::move(key));
  }

  const Variable operator[] (KeyValue key) const {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return nullptr;
    return getData()->getOrInsertNamedVariable(std::move(key)).getVariable();
  }

  Iterator begin() {return getData()->begin();}
  Iterator end() {return getData()->end();}

  ReverseIterator rbegin() {return getData()->rbegin();}
  ReverseIterator rend() {return getData()->rend();}

  inline ConstIterator begin() const noexcept {return cbegin();}
  inline ConstIterator end() const noexcept {return cend();}

  inline ConstReverseIterator rbegin() const noexcept {return crbegin();}
  inline ConstReverseIterator rend() const noexcept {return crend();}

  ConstIterator cbegin() const {return getData()->cbegin();}
  ConstIterator cend() const {return getData()->cend();}

  ConstReverseIterator crbegin() const {return getData()->crbegin();}
  ConstReverseIterator crend() const {return getData()->crend();}

};

}

#endif // MAP_HXX
