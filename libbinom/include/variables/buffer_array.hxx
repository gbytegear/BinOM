#ifndef BUFFER_ARRAY_HXX
#define BUFFER_ARRAY_HXX

#include "variable.hxx"

namespace binom {

class BufferArray : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator BufferArray& () = delete;
  operator Array& () = delete;
  operator SingleLinkedList& () = delete;
  operator DoublyLinkedList& () = delete;
  operator Map& () = delete;

  Number& toNumber() const = delete;
  BitArray& toBitArray() const = delete;
  BufferArray& toBufferArray() const = delete;
  Array& toArray() const = delete;
  SingleLinkedList& toSingleLinkedList() const = delete;
  DoublyLinkedList& toDoublyLinkedList() const = delete;
  Map& toMap() const = delete;

  priv::BufferArrayHeader*& getData() const noexcept;

  friend class Variable;
  BufferArray(priv::Link&& link);

public:
  typedef GenericValueIterator Iterator;
  typedef ReverseGenericValueIterator ReverseIterator;
  typedef GenericValueRef ValueRef;

  template<typename T>
  BufferArray(const std::initializer_list<T> init_list) : Variable(init_list) {static_assert (std::is_arithmetic_v<T>);}
  BufferArray(const BufferArray& other) noexcept;
  BufferArray(const BufferArray&& other) noexcept;

  BufferArray getReference() noexcept;
  size_t getElementCount() const noexcept;
  size_t getSize() const noexcept;
  size_t getCapacity() const noexcept;

  ValueRef operator[](size_t index) noexcept;

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

  Iterator pushBack(BufferArray& value_list);

  template<typename T>
  ValueRef pushFront(T value) noexcept {return insert(0, value);}

  template<typename T>
  Iterator pushFront(std::initializer_list<T> value_list) {return insert(0, value_list);}

  Iterator pushFront(BufferArray& value_list);

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

  Iterator insert(size_t at, BufferArray& value_list);

  Iterator begin() const;
  Iterator end() const;

  ReverseIterator rbegin() const;
  ReverseIterator rend() const;

  BufferArray& operator=(const BufferArray& other);
  BufferArray& operator=(BufferArray&& other);

  BufferArray& changeLink(const BufferArray& other);
  BufferArray& changeLink(BufferArray&& other);

};

}

#endif // BUFFER_ARRAY_HXX
