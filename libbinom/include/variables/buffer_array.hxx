#ifndef BUFFER_ARRAY_HXX
#define BUFFER_ARRAY_HXX

#include "variable.hxx"

namespace binom {

class BufferArray : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator Array& () = delete;
  operator SinglyLinkedList& () = delete;
  operator DoublyLinkedList& () = delete;
  operator Map& () = delete;

  Number& toNumber() = delete;
  BitArray& toBitArray() = delete;
  BufferArray& toBufferArray() = delete;
  Array& toArray() = delete;
  SinglyLinkedList& toSinglyLinkedList() = delete;
  DoublyLinkedList& toDoublyLinkedList() = delete;
  Map& toMap() = delete;

  operator const Number& () const = delete;
  operator const BitArray& () const = delete;
  operator const Array& () const = delete;
  operator const SinglyLinkedList& () const = delete;
  operator const DoublyLinkedList& () const = delete;
  operator const Map& () const = delete;

  const Number& toNumber() const = delete;
  const BitArray& toBitArray() const = delete;
  const BufferArray& toBufferArray() const = delete;
  const Array& toArray() const = delete;
  const SinglyLinkedList& toSinglyLinkedList() const = delete;
  const DoublyLinkedList& toDoublyLinkedList() const = delete;
  const Map& toMap() const = delete;

  priv::BufferArrayImplementation*& getData() const noexcept;

  BufferArray(priv::Link&& link);

  friend class binom::Variable;
  friend class binom::KeyValue;
public:
  typedef GenericValueIterator Iterator;
  typedef ReverseGenericValueIterator ReverseIterator;
  typedef const Iterator ConstIterator;
  typedef const ReverseIterator ConstReverseIterator;
  typedef GenericValueRef ValueRef;

  BufferArray() noexcept;
  template<typename T>
  BufferArray(const std::initializer_list<T> init_list) noexcept : Variable(init_list) {static_assert (std::is_arithmetic_v<T>);}
  BufferArray(const BufferArray& other) noexcept;
  BufferArray(const BufferArray&& other) noexcept;

  BufferArray move() noexcept;
  const BufferArray move() const noexcept;

  size_t getElementCount() const noexcept;
  size_t getSize() const noexcept;
  size_t getCapacity() const noexcept;

  ValueRef operator[](size_t index) noexcept;
  const ValueRef operator[](size_t index) const noexcept;

  template<typename T>
  ValueRef pushBack(T value) noexcept {
    if(auto lk = getLock(MtxLockType::unique_locked); lk)
      return priv::BufferArrayImplementation::pushBack(getData(), getValType(), value);
    else return ValueRef(ValType::invalid_type, nullptr);
  }

  template<typename T>
  Iterator pushBack(const std::initializer_list<T> value_list) {
    if(auto lk = getLock(MtxLockType::unique_locked); lk)
      return priv::BufferArrayImplementation::pushBack(getData(), getValType(), value_list);
    else return Iterator(ValType::invalid_type, nullptr);
  }

  template<typename T>
  ValueRef pushFront(T value) noexcept {return insert<T>(0, value);}

  template<typename T>
  Iterator pushFront(std::initializer_list<T> value_list) {return insert<T>(0, value_list);}

  template<typename T>
  ValueRef insert(size_t at, T value) noexcept {
    if(auto lk = getLock(MtxLockType::unique_locked); lk)
      return priv::BufferArrayImplementation::insert(getData(), getBitWidth(), at, 1, value);
    else return ValueRef(ValType::invalid_type, nullptr);
  }

  template<typename T>
  Iterator insert(size_t at, std::initializer_list<T> value_list) {
    if(auto lk = getLock(MtxLockType::unique_locked); lk)
      return priv::BufferArrayImplementation::insert(getData(), getBitWidth(), at, 1, value_list);
    else return Iterator(ValType::invalid_type, nullptr);
  }

  void remove(size_t at, size_t count = 1);
  void popBack(size_t count = 1);
  void popFront(size_t count = 1);
  void clear();


  Iterator begin();
  Iterator end();

  const Iterator cbegin() const;
  const Iterator cend() const;

  ReverseIterator rbegin();
  ReverseIterator rend();

  inline ConstIterator begin() const noexcept {return cbegin();}
  inline ConstIterator end() const noexcept {return cend();}

  inline ConstReverseIterator rbegin() const noexcept {return crbegin();}
  inline ConstReverseIterator rend() const noexcept {return crend();}

  const ReverseIterator crbegin() const;
  const ReverseIterator crend() const;

  BufferArray& operator=(const BufferArray& other);
  BufferArray& operator=(BufferArray&& other);

  BufferArray& changeLink(const BufferArray& other);
  BufferArray& changeLink(BufferArray&& other);

};

}

#endif // BUFFER_ARRAY_HXX
