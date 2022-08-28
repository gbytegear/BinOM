#ifndef BUFFER_ARRAY_IMPL_HXX
#define BUFFER_ARRAY_IMPL_HXX

#include "../types.hxx"
#include "../../utils/extended_type_traits.hxx"
#include "../../utils/reverse_iterator.hxx"
#include "../../variables/generic_value.hxx"

namespace binom::priv {

class BufferArrayImplementation {
  size_t size = 0;
  size_t capacity = 0;

  template<typename T>
  requires std::is_arithmetic_v<T>
  BufferArrayImplementation(const std::initializer_list<T>& value_list)
    : size(value_list.size() * sizeof(T)), capacity(calculateCapacity(size)) {
    std::memcpy(getData(), value_list.begin(), size);
  }

  BufferArrayImplementation(const BufferArrayImplementation& other);
  static size_t calculateCapacity(size_t size) noexcept;

  static void* increaseSize(BufferArrayImplementation*& implementation, VarBitWidth type, size_t count);
  static void reduceSize(BufferArrayImplementation*& implementation, VarBitWidth type, size_t count);
  static void shrinkToFit(BufferArrayImplementation*& implementation);
  static void* insertBlock(BufferArrayImplementation*& implementation, VarBitWidth type, size_t at, size_t count);

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

  void* get(VarBitWidth type, size_t at) const;

public:
  template<typename T>
  requires std::is_arithmetic_v<T>
  static BufferArrayImplementation* create(const std::initializer_list<T>& value_list) {
    return new(new byte[ calculateCapacity(value_list.size() * sizeof(T)) ]) BufferArrayImplementation(value_list);
  }

  static BufferArrayImplementation* copy(const BufferArrayImplementation* other);

  template<typename T>
  requires extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>
  static GenericValueRef pushBack(BufferArrayImplementation*& implementation, ValType type, T value) noexcept {
    VarBitWidth bit_width = toBitWidth(type);
    void* data = increaseSize(implementation, bit_width, 1);
    set(type, data, value);
    return GenericValueRef(type, data);
  }

  template<typename T>
  requires extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>
  static GenericValueIterator pushBack(BufferArrayImplementation*& implementation, ValType type, const std::initializer_list<T>& value_list) {
    VarBitWidth bit_width = toBitWidth(type);
    void* data = increaseSize(implementation, bit_width, value_list.size());
    setRange(type, data, value_list);
    return GenericValueIterator(type, data);
  }

  template<typename T>
  static GenericValueRef pushFront(BufferArrayImplementation*& implementation, ValType type, T value) noexcept {return insert<T>(implementation, type, 0, value);}
  template<typename T>
  static GenericValueIterator pushFront(BufferArrayImplementation*& implementation, ValType type, std::initializer_list<T> value_list) {return insert<T>(implementation, type, 0, value_list);}

  template<typename T>
  requires extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>
  static GenericValueRef insert(BufferArrayImplementation*& implementation, ValType type, size_t at, T value) noexcept {
    VarBitWidth bit_width = toBitWidth(type);
    void* data = insertBlock(implementation, bit_width, at, 1);
    set(type, data, value);
    return GenericValueRef(type, data);
  }

  template<typename T>
  requires extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> || extended_type_traits::is_arithmetic_without_cvref_v<T>
  static GenericValueIterator insert(BufferArrayImplementation*& implementation, ValType type, size_t at, const std::initializer_list<T>& value_list) {
    VarBitWidth bit_width = toBitWidth(type);
    void* data = insertBlock(implementation, bit_width, at, 1);
    setRange(type, data, value_list);
    return GenericValueIterator(type, data);
  }

  static void popBack(BufferArrayImplementation*& implementation, VarBitWidth type, size_t count);
  static void popFront(BufferArrayImplementation*& implementation, VarBitWidth type, size_t count);
  static void remove(BufferArrayImplementation*& implementation, VarBitWidth type, size_t at, size_t count);
  static void clear(BufferArrayImplementation*& implementation, VarBitWidth type);

  void* getData() const;

  template<typename T>
  inline T* getDataAs() const {return const_cast<T*>(reinterpret_cast<const T*>(this + 1));}

  size_t getSize() const noexcept;
  size_t getElementCount(VarBitWidth type) const noexcept;
  size_t getCapacity() const noexcept;

  GenericValueRef get(ValType type, size_t at);
  const GenericValueRef get(ValType type, size_t at) const;

  GenericValueIterator begin(ValType type);
  GenericValueIterator end(ValType type);
  const GenericValueIterator begin(ValType type) const;
  const GenericValueIterator end(ValType type) const;
  ReverseGenericValueIterator rbegin(ValType type);
  ReverseGenericValueIterator rend(ValType type);
  const ReverseGenericValueIterator rbegin(ValType type) const;
  const ReverseGenericValueIterator rend(ValType type) const;

  void operator delete(void* ptr);
};


}

#endif // BUFFER_ARRAY_IMPL_HXX
