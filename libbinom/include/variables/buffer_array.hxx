#ifndef BUFFER_ARRAY_HXX
#define BUFFER_ARRAY_HXX

#include "variable.hxx"

namespace binom {

class BufferArray : public Variable {
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

  inline priv::BufferArrayHeader*& getData() const noexcept {return resource_link->data.buffer_array_header;}

  friend class Variable;
  BufferArray(priv::Link&& link) : Variable(std::move(link)) {}

public:
  typedef GenericValueIterator Iterator;
  typedef ReverseGenericValueIterator ReverseIterator;
  typedef GenericValueRef ValueRef;

  template<typename T>
  BufferArray(const std::initializer_list<T> init_list) : Variable(init_list) {static_assert (std::is_arithmetic_v<T>);}
  BufferArray(const BufferArray& other) noexcept : Variable(dynamic_cast<const Variable&>(other)) {}
  BufferArray(BufferArray&& other) noexcept : Variable(dynamic_cast<Variable&&>(other)) {}

  size_t getCount() const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return 0;
    return getData()->getCount(getBitWidth());
  }

  size_t getSize() const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return 0;
    return getData()->getSize();
  }

  size_t getCapacity() const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return 0;
    return getData()->getCapacity();
  }

  ValueRef operator[](size_t index) noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return ValueRef(ValType::invalid_type, nullptr, resource_link);
    return ValueRef(getValType(), getData()->get(getBitWidth(), index), resource_link);
  }

  template<typename T>
  ValueRef pushBack(T value) noexcept {
    static_assert (extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>);
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return ValueRef(ValType::invalid_type, nullptr, resource_link);
    ValueRef value_ref(getValType(), priv::BufferArrayHeader::increaseSize(getData(), getBitWidth(), 1), resource_link);
    value_ref = value;
    return value_ref;
  }

  template<typename T>
  Iterator pushBack(std::initializer_list<T> value_list) {
    static_assert (extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>);
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return Iterator(ValType::invalid_type, nullptr, resource_link);
    Iterator value_it(getValType(), priv::BufferArrayHeader::increaseSize(getData(), getBitWidth(), value_list.size()), resource_link);
    { Iterator this_it = value_it;
      for(const auto& value : value_list)
        *(this_it++) = value;
    }
    return value_it;
  }

  Iterator pushBack(BufferArray& value_list) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return Iterator(ValType::invalid_type, nullptr, resource_link);
    Iterator value_it(getValType(), priv::BufferArrayHeader::increaseSize(getData(), getBitWidth(), value_list.getSize()), resource_link);
    { Iterator this_it = value_it;
      for(auto value : value_list)
        *(this_it++) = value;
    }
    return value_it;
  }

  template<typename T>
  ValueRef pushFront(T value) noexcept {return insert(0, value);}

  template<typename T>
  Iterator pushFront(std::initializer_list<T> value_list) {return insert(0, value_list);}

  Iterator pushFront(BufferArray& value_list) {return insert(0, value_list);}

  template<typename T>
  ValueRef insert(size_t at, T value) noexcept {
    static_assert (extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>);
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return GenericValueRef(ValType::invalid_type, nullptr, resource_link);
    ValueRef value_ref(getValType(), priv::BufferArrayHeader::insert(getData(), getBitWidth(), at, 1), resource_link);
    value_ref = value;
    return value_ref;
  }

  template<typename T>
  Iterator insert(size_t at, std::initializer_list<T> value_list) {
    static_assert (extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>);
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return Iterator(ValType::invalid_type, nullptr, resource_link);
    ValueRef value_it(getValType(), priv::BufferArrayHeader::insert(getData(), getBitWidth(), at, value_list.size()), resource_link);
    { Iterator this_it = value_it;
      for(const auto& value : value_list)
        *(this_it++) = value;
    }
    return value_it;
  }

  Iterator insert(size_t at, BufferArray& value_list) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return Iterator(ValType::invalid_type, nullptr, resource_link);
    Iterator value_it(getValType(), priv::BufferArrayHeader::insert(getData(), getBitWidth(), at, value_list.getSize()), resource_link);
    { Iterator this_it = value_it;
      for(auto value : value_list)
        *(this_it++) = value;
    }
    return value_it;
  }

  Iterator begin() const {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return Iterator(ValType::invalid_type, nullptr, resource_link);
    return Iterator(getValType(), getData()->getBeginPtr(), resource_link);
  }

  Iterator end() const {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return Iterator(ValType::invalid_type, nullptr, resource_link);
    return Iterator(getValType(), getData()->getEndPtr(getBitWidth()), resource_link);
  }

  ReverseIterator rbegin() const {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return ReverseIterator(ValType::invalid_type, nullptr, resource_link);
    return ReverseIterator(getValType(), getData()->getReverseBeginPtr(getBitWidth()), resource_link);
  }

  ReverseIterator rend() const {
    auto lk = getLock(MtxLockType::shared_locked);
    if(!lk) return ReverseIterator(ValType::invalid_type, nullptr, resource_link);
    return ReverseIterator(getValType(), getData()->getReverseEndPtr(getBitWidth()), resource_link);
  }

};

}

#endif // BUFFER_ARRAY_HXX
