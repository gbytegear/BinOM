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
  Array()
    : Variable(ResourceData{VarType::array, {.array_header = priv::ArrayHeader::create(literals::arr{})}}) {}
  Array(const literals::arr array)
    : Variable(ResourceData{VarType::array, {.array_header = priv::ArrayHeader::create(array)}}) {}

  Array getReference() noexcept {return Link(resource_link);}

  size_t getCount() const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return 0;
    return getData()->getCount();
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

  // TODO: Some usable things...

  Variable operator[](size_t index) noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return nullptr;
    if(index < getData()->getCount())
      return getData()->getData()[index].getReference();
    else return nullptr;
  }

};

}

#endif // ARRAY_HXX
