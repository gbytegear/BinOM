#ifndef ARRAY_HXX
#define ARRAY_HXX

#include "variable.hxx"

namespace binom {

class Array : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator BufferArray& () = delete;
  operator Array& () = delete;
  operator List& () = delete;
  operator Map& () = delete;

  inline Number& toNumber() = delete;
  inline BitArray& toBitArray() = delete;
  inline BufferArray& toBufferArray() = delete;
  inline Array& toArray() = delete;
  inline List& toList() = delete;
  inline Map& toMap() = delete;

  inline priv::ArrayHeader*& getData() const noexcept {return resource_link->data.array_header;}

  friend class Variable;
  Array(priv::Link&& link) : Variable(std::move(link)) {}
public:
  typedef Variable* Iterator;
  typedef std::reverse_iterator<Variable*> ReverseIterator;

  Array()
    : Variable(ResourceData{VarType::array, {.array_header = priv::ArrayHeader::create(literals::arr{})}}) {}
  Array(const literals::arr array)
    : Variable(ResourceData{VarType::array, {.array_header = priv::ArrayHeader::create(array)}}) {}

  Array getReference() noexcept {return Link(resource_link);}

  size_t getElementCount() const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return 0;
    return getData()->getElementCount();
  }

  size_t getCapacity() const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return 0;
    return getData()->getCapacity();
  }

  size_t getSize() const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return 0;
    return getData()->getSize();
  }

  Variable pushBack(Variable variable) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return nullptr;

    auto allocated_memory = priv::ArrayHeader::increaseSize(getData(), 1);
    new(allocated_memory) Variable(std::move(variable));

    return allocated_memory->getReference();
  }

  Iterator pushBack(literals::arr variable_list) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return nullptr;

    auto allocated_memory = priv::ArrayHeader::increaseSize(getData(), variable_list.getSize());
    auto it = allocated_memory;
    for(const auto& variable : variable_list)
      new(it++) Variable(std::move(variable));

    return allocated_memory;
  }

  Variable pushFront(Variable variable) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return nullptr;

    auto allocated_memory = priv::ArrayHeader::insert(getData(), 0, 1);
    new(allocated_memory) Variable(std::move(variable));

    return allocated_memory->getReference();
  }

  Iterator pushFront(literals::arr variable_list) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return nullptr;

    auto allocated_memory = priv::ArrayHeader::insert(getData(), 0, variable_list.getSize());
    auto it = allocated_memory;
    for(const auto& variable : variable_list)
      new(it++) Variable(std::move(variable));

    return allocated_memory;
  }

  Variable insert(size_t at, Variable variable) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return nullptr;

    auto allocated_memory = priv::ArrayHeader::insert(getData(), at, 1);
    new(allocated_memory) Variable(std::move(variable));

    return allocated_memory->getReference();
  }

  Iterator insert(size_t at, literals::arr variable_list) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return nullptr;

    auto allocated_memory = priv::ArrayHeader::insert(getData(), at, variable_list.getSize());
    auto it = allocated_memory;
    for(const auto& variable : variable_list)
      new(it++) Variable(std::move(variable));

    return allocated_memory;
  }

  void popBack(size_t count = 1) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return;

    priv::ArrayHeader::popBack(getData(), count);
  }

  void popFront(size_t count = 1) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return;

    priv::ArrayHeader::remove(getData(), 0, count);
  }

  void remove(size_t at, size_t count = 1) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return;

    priv::ArrayHeader::remove(getData(), at, count);
  }

  void clear() {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return;

    priv::ArrayHeader::popBack(getData(), getElementCount());
  }


  // TODO: Some usable things...

  Variable operator[](size_t index) noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return nullptr;
    return (*getData())[index];
  }

  Variable operator+=(Variable variable) {return pushBack(std::move(variable));}
  Variable operator+=(literals::arr variable_list) {return pushBack(std::move(variable_list));}

  Iterator begin() const {return getData()->begin();}
  Iterator end() const {return getData()->end();}

  ReverseIterator rbegin() const {return getData()->rbegin();}
  ReverseIterator rend() const {return getData()->rend();}


};

}

#endif // ARRAY_HXX
