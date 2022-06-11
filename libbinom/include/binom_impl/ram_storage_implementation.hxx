#ifndef RAM_STORAGE_IMPLEMENTATION_H
#define RAM_STORAGE_IMPLEMENTATION_H

#include "types.hxx"
#include "bits.hxx"

#include "../utils/extended_type_traits.hxx"
#include "../utils/reverse_iterator.hxx"
#include "../variables/generic_value.hxx"

namespace binom::priv {

class BitArrayHeader {
  ui64 bit_size = 0;
  size_t capacity = 0;

  BitArrayHeader(const literals::bitarr& bit_array_data);
  BitArrayHeader(const BitArrayHeader& other);

  static BitIterator increaseSize(BitArrayHeader*& header, size_t bit_count);
  static void reduceSize(BitArrayHeader*& header, size_t bit_count);
  static BitIterator insertBits(priv::BitArrayHeader*& header, size_t at, size_t count);
public:
  static BitArrayHeader* create(const literals::bitarr& bit_array_data);
  static BitArrayHeader* copy(const BitArrayHeader* other);

  static size_t calculateByteSize(size_t bit_count) noexcept;
  static size_t calculateCapacity(size_t bit_count) noexcept;

  static BitValueRef pushBack(BitArrayHeader*& header, bool value);
  static BitIterator pushBack(BitArrayHeader*& header, const literals::bitarr& value_list);

  static BitValueRef pushFront(BitArrayHeader*& header, bool value);
  static BitIterator pushFront(BitArrayHeader*& header, const literals::bitarr& value_list);

  static BitValueRef insert(BitArrayHeader*& header, size_t at, bool value);
  static BitIterator insert(BitArrayHeader*& header, size_t at, const literals::bitarr value_list);

  static void popBack(BitArrayHeader*& header, size_t size);
  static void popFront(BitArrayHeader*& header, size_t size);
  static void removeBits(priv::BitArrayHeader*& header, size_t at, size_t count);

  static void shrinkToFit(BitArrayHeader*& header);

  size_t getCapacity() const noexcept;
  size_t getBitSize() const noexcept;
  size_t getByteSize() const noexcept;
  Bits* getData() const noexcept;

  template<typename T>
  inline T* getDataAs() const noexcept {
    return reinterpret_cast<T*>(reinterpret_cast<ui8*>(const_cast<BitArrayHeader*>(this)) + sizeof(BitArrayHeader));
  }

  void operator delete(void* ptr);

  BitValueRef operator[](size_t index) const noexcept;

  BitIterator begin() const noexcept;
  BitIterator end() const noexcept;
  BitReverseIterator rbegin() const noexcept;
  BitReverseIterator rend() const noexcept;
};


class BufferArrayHeader {
  size_t size = 0;
  size_t capacity = 0;

  template<typename T>
  BufferArrayHeader(const std::initializer_list<T>& value_list)
    : size(value_list.size() * sizeof(T)), capacity(calculateCapacity(size)) {
    static_assert (std::is_arithmetic_v<T>, "T isn't arithmetic type:"
                                            " as argument requires const std::initializer_list<T>&"
                                            " where assertion std::is_arithmetic<T>::value is true");
    std::memcpy(getData(), value_list.begin(), size);
  }

  BufferArrayHeader(const BufferArrayHeader& other);
  static size_t calculateCapacity(size_t size) noexcept;

  static void* increaseSize(BufferArrayHeader*& header, VarBitWidth type, size_t count);
  static void reduceSize(BufferArrayHeader*& header, VarBitWidth type, size_t count);
  static void shrinkToFit(BufferArrayHeader*& header);
  static void* insertBlock(BufferArrayHeader*& header, VarBitWidth type, size_t at, size_t count);

  template<typename T>
  static constexpr VarNumberType determineNumberType() {
    if constexpr      (std::is_floating_point_v<T>) return VarNumberType::float_point;
    else if constexpr (std::is_unsigned_v<T>)       return VarNumberType::unsigned_integer;
    else if constexpr (std::is_signed_v<T>)         return VarNumberType::signed_integer;
    else return VarNumberType::invalid_type;
  }

  template<typename T>
  static void set(ValType type, void* to, T from) {
    switch (type) {
    case binom::ValType::ui8:
      *reinterpret_cast<ui8*>(to) = static_cast<ui8>(from);
    return;
    case binom::ValType::si8:
      *reinterpret_cast<i8*>(to) = static_cast<i8>(from);
    return;
    case binom::ValType::ui16:
      *reinterpret_cast<ui16*>(to) = static_cast<ui16>(from);
    return;
    case binom::ValType::si16:
      *reinterpret_cast<i16*>(to) = static_cast<i16>(from);
    return;
    case binom::ValType::ui32:
      *reinterpret_cast<ui32*>(to) = static_cast<ui32>(from);
    return;
    case binom::ValType::si32:
      *reinterpret_cast<i32*>(to) = static_cast<i32>(from);
    return;
    case binom::ValType::f32:
      *reinterpret_cast<f32*>(to) = static_cast<f32>(from);
    return;
    case binom::ValType::ui64:
      *reinterpret_cast<ui64*>(to) = static_cast<ui64>(from);
    return;
    case binom::ValType::si64:
      *reinterpret_cast<i64*>(to) = static_cast<i64>(from);
    return;
    case binom::ValType::f64:
      *reinterpret_cast<f64*>(to) = static_cast<f64>(from);
    return;
    default:
    case binom::ValType::invalid_type:
    return;
    }
  }

  static inline void* shiftToNextAndGetCurrent(VarBitWidth type, void*& data) {
    void* current = data;
    reinterpret_cast<ui8*&>(data) += size_t(type);
    return current;
  }

  template<typename T>
  static inline void setRange(ValType type, void* memory_block, const std::initializer_list<T>& value_list) {
    VarBitWidth bit_width = toBitWidth(type);
    for(const auto& value : value_list)
      set(type, shiftToNextAndGetCurrent(bit_width, memory_block), &value);
  }

  static inline size_t pointerToIndex(VarBitWidth bit_width, void* begin, void* pointer) {
    return size_t((reinterpret_cast<ui8*>(pointer) - reinterpret_cast<ui8*>(begin)) / size_t(bit_width));
  }

public:
  template<typename T>
  static BufferArrayHeader* create(const std::initializer_list<T>& value_list) {
    static_assert (std::is_arithmetic_v<T>, "T isn't arithmetic type:"
                                            " as argument requires const std::initializer_list<T>&"
                                            " where assertion std::is_arithmetic<T>::value is true");
    return new(new byte[ calculateCapacity(value_list.size() * sizeof(T)) ]) BufferArrayHeader(value_list);
  }

  static BufferArrayHeader* copy(const BufferArrayHeader* other);

  template<typename T>
  static size_t pushBack(BufferArrayHeader*& header, ValType type, T value) noexcept {
    static_assert (extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> ||
                   extended_type_traits::is_arithmetic_without_cvref_v<T>);
    VarBitWidth bit_width = toBitWidth(type);
    void* data = increaseSize(header, bit_width, 1);
    set(type, data, value);
    return pointerToIndex(bit_width, header->getData(), data);
  }

  template<typename T>
  static size_t pushBack(BufferArrayHeader*& header, ValType type, const std::initializer_list<T>& value_list) {
    static_assert (extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> ||
                   extended_type_traits::is_arithmetic_without_cvref_v<T>);
    VarBitWidth bit_width = toBitWidth(type);
    void* data = increaseSize(header, bit_width, value_list.size());
    setRange(type, data, value_list);
    return pointerToIndex(bit_width, header->getData(), data);
  }

  template<typename T>
  static size_t pushFront(BufferArrayHeader*& header, ValType type, T value) noexcept {return insert<T>(header, type, 0, value);}
  template<typename T>
  static size_t pushFront(BufferArrayHeader*& header, ValType type, std::initializer_list<T> value_list) {return insert<T>(header, type, 0, value_list);}

  template<typename T>
  static size_t insert(BufferArrayHeader*& header, ValType type, size_t at, T value) noexcept {
    static_assert (extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> ||
                   extended_type_traits::is_arithmetic_without_cvref_v<T>);
    VarBitWidth bit_width = toBitWidth(type);
    void* data = insertBlock(header, bit_width, at, 1);
    set(type, data, value);
    return pointerToIndex(bit_width, header->getData(), data);
  }

  template<typename T>
  static size_t insert(BufferArrayHeader*& header, ValType type, size_t at, const std::initializer_list<T>& value_list) {
    static_assert (extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> ||
                   extended_type_traits::is_arithmetic_without_cvref_v<T>);
    VarBitWidth bit_width = toBitWidth(type);
    void* data = insertBlock(header, bit_width, at, 1);
    setRange(type, data, value_list);
    return pointerToIndex(bit_width, header->getData(), data);
  }

  static void remove(BufferArrayHeader*& header, VarBitWidth type, size_t at, size_t count);

  void* getData() const;

  template<typename T>
  inline T* getDataAs() const {return const_cast<T*>(reinterpret_cast<const T*>(this + 1));}

  size_t getSize() const noexcept;
  size_t getElementCount(VarBitWidth type) const noexcept;
  size_t getCapacity() const noexcept;

  void* get(VarBitWidth type, size_t at) const;

  void* getBeginPtr() const;
  void* getEndPtr(VarBitWidth type) const;
  void* getReverseBeginPtr(VarBitWidth type) const;
  void* getReverseEndPtr(VarBitWidth type) const;

  void operator delete(void* ptr);

};


class ArrayHeader {
  size_t count;
  size_t capacity;

  ArrayHeader(const literals::arr& value_list);
  ArrayHeader(const ArrayHeader& other);

  static size_t calculateCapacity(size_t count) noexcept;
public:
  typedef Variable* Iterator;
  typedef reverse_iterator::ReverseIterator<Variable*> ReverseIterator;

  static ArrayHeader* create(const literals::arr& value_list);
  static ArrayHeader* copy(const ArrayHeader* other);

  size_t getElementCount() const noexcept;
  size_t getCapacity() const noexcept;
  size_t getSize() const noexcept;

  Variable* getData() const;
  static Iterator increaseSize(ArrayHeader*& header, size_t count);
  static void reduceSize(ArrayHeader*& header, size_t count);
  static void popBack(ArrayHeader*& header, size_t count);
  static Iterator insert(ArrayHeader*& header, size_t at, size_t count);
  static void remove(ArrayHeader*& header, size_t at, size_t count);

  Variable operator[](size_t index);

  Iterator begin() const;
  Iterator end() const;
  ReverseIterator rbegin() const;
  ReverseIterator rend() const;

  void operator delete(void* ptr);
};


class SinglyLinkedListHeader {
  struct Node;
  size_t size = 0;
  Node* first = nullptr;
  Node* last = nullptr;
public:
  class Iterator;
  SinglyLinkedListHeader(const literals::sllist& value_list);
  SinglyLinkedListHeader(const SinglyLinkedListHeader& other);
  ~SinglyLinkedListHeader();

  bool isEmpty();

  void clear();

  Variable pushBack(Variable var);
  Iterator pushBack(const literals::sllist& value_list);

  Variable pushFront(Variable var);
  Iterator pushFront(const literals::sllist& value_list);

  Iterator insert(Iterator it, Variable var);
  void remove(Iterator it);

  Iterator begin() const;
  Iterator end() const;
};


class DoublyLinkedListHeader {
  struct Node;
  size_t size = 0;
  Node* first = nullptr;
  Node* last = nullptr;
public:
  class Iterator;
  typedef reverse_iterator::ReverseIterator<Iterator> ReverseIterator;
  DoublyLinkedListHeader(const literals::dllist& value_list);
  DoublyLinkedListHeader(const DoublyLinkedListHeader& other);
  ~DoublyLinkedListHeader();

  bool isEmpty();

  void clear();

  Variable pushBack(Variable var);
  Iterator pushBack(const literals::dllist& value_list);

  Variable pushFront(Variable var);
  Iterator pushFront(const literals::dllist& value_list);

  Iterator insert(Iterator it, Variable var);

  void popBack();
  void popFront();
  void remove(Iterator it);

  Iterator begin() const;
  Iterator end() const;

  ReverseIterator rbegin() const;
  ReverseIterator rend() const;
};

}


#endif // RAM_STORAGE_IMPLEMENTATION_H
