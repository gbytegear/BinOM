#ifndef BUFFER_ARRAY_HXX
#define BUFFER_ARRAY_HXX

#include "variable.hxx"
#include "../binom_impl/ram_storage_implementation/buffer_array_impl.hxx"

namespace binom {

class BufferArray : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator Array& () = delete;
  operator List& () = delete;
  operator Map& () = delete;
  operator MultiMap& () = delete;

  Number& toNumber() = delete;
  BitArray& toBitArray() = delete;
  BufferArray& toBufferArray() = delete;
  Array& toArray() = delete;
  List& toList() = delete;
  Map& toMap() = delete;
  MultiMap& toMultiMap() = delete;

  operator const Number& () const = delete;
  operator const BitArray& () const = delete;
  operator const Array& () const = delete;
  operator const List& () const = delete;
  operator const Map& () const = delete;
  operator const MultiMap& () const = delete;

  const Number& toNumber() const = delete;
  const BitArray& toBitArray() const = delete;
  const BufferArray& toBufferArray() const = delete;
  const Array& toArray() const = delete;
  const List& toList() const = delete;
  const Map& toMap() const = delete;
  const MultiMap& toMultiMap() const = delete;

  Variable& operator=(const Variable& other) = delete;
  Variable& operator=(Variable&& other) = delete;

  Variable& changeLink(const Variable& other) = delete;
  Variable& changeLink(Variable&& other) = delete;

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

  template<typename CharT>
  requires extended_type_traits::is_char_v<CharT>
  BufferArray(const std::basic_string_view<CharT> string_view) : Variable(string_view) {}

  template<typename CharT>
  requires extended_type_traits::is_char_v<CharT>
  BufferArray(const CharT* c_str) : Variable(std::basic_string_view<CharT>(c_str)) {}

  template<typename T>
  requires std::is_arithmetic_v<T>
  BufferArray(const std::initializer_list<T> init_list) noexcept : Variable(init_list) {}

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
  requires extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>
  ValueRef pushBack(T value) noexcept {
    if(auto lk = getLock(MtxLockType::unique_locked); lk)
      return priv::BufferArrayImplementation::pushBack(getData(), getValType(), value);
    else return ValueRef(ValType::invalid_type, nullptr);
  }

  template<typename T>
  requires extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>
  Iterator pushBack(const std::initializer_list<T> value_list) {
    if(auto lk = getLock(MtxLockType::unique_locked); lk)
      return priv::BufferArrayImplementation::pushBack(getData(), getValType(), value_list);
    else return Iterator(ValType::invalid_type, nullptr);
  }

  template<typename T>
  requires extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>
  ValueRef pushFront(T value) noexcept {return insert<T>(0, value);}

  template<typename T>
  requires extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>
  Iterator pushFront(std::initializer_list<T> value_list) {return insert<T>(0, value_list);}

  template<typename T>
  requires extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>
  ValueRef insert(size_t at, T value) noexcept {
    if(auto lk = getLock(MtxLockType::unique_locked); lk)
      return priv::BufferArrayImplementation::insert(getData(), getBitWidth(), at, 1, value);
    else return ValueRef(ValType::invalid_type, nullptr);
  }

  template<typename T>
  requires extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>
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

  template<typename CharT>
  requires extended_type_traits::is_char_v<CharT>
  operator std::basic_string<CharT>() const;

  BufferArray& operator=(const BufferArray& other);
  BufferArray& operator=(BufferArray&& other);

  BufferArray& changeLink(const BufferArray& other);
  BufferArray& changeLink(BufferArray&& other);

};

}

#endif // BUFFER_ARRAY_HXX
