#ifndef RAM_STORAGE_IMPLEMENTATION_H
#define RAM_STORAGE_IMPLEMENTATION_H

#include "types.hxx"
#include "bits.hxx"

#include "../utils/extended_type_traits.hxx"
#include "../utils/reverse_iterator.hxx"
#include "../variables/generic_value.hxx"
//#include "avl_tree.hxx"

namespace binom::priv {

class BitArrayImplementation {
  ui64 bit_size = 0;
  size_t capacity = 0;

  BitArrayImplementation(const literals::bitarr& bit_array_data);
  BitArrayImplementation(const BitArrayImplementation& other);

  static BitIterator increaseSize(BitArrayImplementation*& implementation, size_t bit_count);
  static void reduceSize(BitArrayImplementation*& implementation, size_t bit_count);
  static BitIterator insertBits(priv::BitArrayImplementation*& implementation, size_t at, size_t count);
public:
  static BitArrayImplementation* create(const literals::bitarr& bit_array_data);
  static BitArrayImplementation* copy(const BitArrayImplementation* other);

  static size_t calculateByteSize(size_t bit_count) noexcept;
  static size_t calculateCapacity(size_t bit_count) noexcept;

  static BitValueRef pushBack(BitArrayImplementation*& implementation, bool value);
  static BitIterator pushBack(BitArrayImplementation*& implementation, const literals::bitarr& value_list);

  static BitValueRef pushFront(BitArrayImplementation*& implementation, bool value);
  static BitIterator pushFront(BitArrayImplementation*& implementation, const literals::bitarr& value_list);

  static BitValueRef insert(BitArrayImplementation*& implementation, size_t at, bool value);
  static BitIterator insert(BitArrayImplementation*& implementation, size_t at, const literals::bitarr value_list);

  static void popBack(BitArrayImplementation*& implementation, size_t size);
  static void popFront(BitArrayImplementation*& implementation, size_t size);
  static void removeBits(priv::BitArrayImplementation*& implementation, size_t at, size_t count);

  static void shrinkToFit(BitArrayImplementation*& implementation);

  size_t getCapacity() const noexcept;
  size_t getBitSize() const noexcept;
  size_t getByteSize() const noexcept;
  Bits* getData() const noexcept;

  template<typename T>
  inline T* getDataAs() const noexcept {
    return reinterpret_cast<T*>(reinterpret_cast<ui8*>(const_cast<BitArrayImplementation*>(this)) + sizeof(BitArrayImplementation));
  }

  void operator delete(void* ptr);

  BitValueRef operator[](size_t index) const noexcept;

  BitIterator begin() const noexcept;
  BitIterator end() const noexcept;
  BitReverseIterator rbegin() const noexcept;
  BitReverseIterator rend() const noexcept;
};


class BufferArrayImplementation {
  size_t size = 0;
  size_t capacity = 0;

  template<typename T>
  BufferArrayImplementation(const std::initializer_list<T>& value_list)
    : size(value_list.size() * sizeof(T)), capacity(calculateCapacity(size)) {
    static_assert (std::is_arithmetic_v<T>, "T isn't arithmetic type:"
                                            " as argument requires const std::initializer_list<T>&"
                                            " where assertion std::is_arithmetic<T>::value is true");
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
  static BufferArrayImplementation* create(const std::initializer_list<T>& value_list) {
    static_assert (std::is_arithmetic_v<T>, "T isn't arithmetic type:"
                                            " as argument requires const std::initializer_list<T>&"
                                            " where assertion std::is_arithmetic<T>::value is true");
    return new(new byte[ calculateCapacity(value_list.size() * sizeof(T)) ]) BufferArrayImplementation(value_list);
  }

  static BufferArrayImplementation* copy(const BufferArrayImplementation* other);

  template<typename T>
  static GenericValueRef pushBack(BufferArrayImplementation*& implementation, ValType type, T value) noexcept {
    static_assert (extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> ||
                   extended_type_traits::is_arithmetic_without_cvref_v<T>);
    VarBitWidth bit_width = toBitWidth(type);
    void* data = increaseSize(implementation, bit_width, 1);
    set(type, data, value);
    return GenericValueRef(type, data);
  }

  template<typename T>
  static GenericValueIterator pushBack(BufferArrayImplementation*& implementation, ValType type, const std::initializer_list<T>& value_list) {
    static_assert (extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> ||
                   extended_type_traits::is_arithmetic_without_cvref_v<T>);
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
  static GenericValueRef insert(BufferArrayImplementation*& implementation, ValType type, size_t at, T value) noexcept {
    static_assert (extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> ||
                   extended_type_traits::is_arithmetic_without_cvref_v<T>);
    VarBitWidth bit_width = toBitWidth(type);
    void* data = insertBlock(implementation, bit_width, at, 1);
    set(type, data, value);
    return GenericValueRef(type, data);
  }

  template<typename T>
  static GenericValueIterator insert(BufferArrayImplementation*& implementation, ValType type, size_t at, const std::initializer_list<T>& value_list) {
    static_assert (extended_type_traits::is_crtp_base_of_v<arithmetic::ArithmeticTypeBase, T> ||
                   extended_type_traits::is_arithmetic_without_cvref_v<T>);
    VarBitWidth bit_width = toBitWidth(type);
    void* data = insertBlock(implementation, bit_width, at, 1);
    setRange(type, data, value_list);
    return GenericValueIterator(type, data);
  }

  static void remove(BufferArrayImplementation*& implementation, VarBitWidth type, size_t at, size_t count);

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


class ArrayImplementation {
public:
  typedef Variable* Iterator;
  typedef reverse_iterator::ReverseIterator<Variable*> ReverseIterator;
private:
  size_t count;
  size_t capacity;

  ArrayImplementation(const literals::arr& value_list);
  ArrayImplementation(const ArrayImplementation& other);

  static size_t calculateCapacity(size_t count) noexcept;

  static Iterator increaseSize(ArrayImplementation*& implementation, size_t count);
  static void reduceSize(ArrayImplementation*& implementation, size_t count);
  static Iterator insert(ArrayImplementation*& implementation, size_t at, size_t count);

public:

  static ArrayImplementation* create(const literals::arr& value_list);
  static ArrayImplementation* copy(const ArrayImplementation* other);

  size_t getElementCount() const noexcept;
  size_t getCapacity() const noexcept;
  size_t getSize() const noexcept;

  Variable* getData() const;

  static Variable pushBack(ArrayImplementation*& implementation, Variable variable);
  static Iterator pushBack(ArrayImplementation*& implementation, const literals::arr variable_list);

  static Variable pushFront(ArrayImplementation*& implementation, Variable variable);
  static Iterator pushFront(ArrayImplementation*& implementation, const literals::arr& variable_list);

  static Variable insert(ArrayImplementation*& implementation, size_t at, Variable variable);
  static Iterator insert(ArrayImplementation*& implementation, size_t at, const literals::arr& variable_list);

  static void popBack(ArrayImplementation*& implementation, size_t count = 1);
  static void popFront(ArrayImplementation*& implementation, size_t count = 1);

  static void remove(ArrayImplementation*& implementation, size_t at, size_t count);
  static void clear(ArrayImplementation*& implementation);

  Variable operator[](size_t index);

  Iterator begin() const;
  Iterator end() const;
  ReverseIterator rbegin() const;
  ReverseIterator rend() const;

  void operator delete(void* ptr);
};


class SinglyLinkedListImplementation {
  struct Node;
  size_t size = 0;
  Node* first = nullptr;
  Node* last = nullptr;
public:
  class Iterator;
  SinglyLinkedListImplementation(const literals::sllist& value_list);
  SinglyLinkedListImplementation(const SinglyLinkedListImplementation& other);
  ~SinglyLinkedListImplementation();

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


class DoublyLinkedListImplementation {
  struct Node;
  size_t size = 0;
  Node* first = nullptr;
  Node* last = nullptr;
public:
  class Iterator;
  typedef reverse_iterator::ReverseIterator<Iterator> ReverseIterator;
  DoublyLinkedListImplementation(const literals::dllist& value_list);
  DoublyLinkedListImplementation(const DoublyLinkedListImplementation& other);
  ~DoublyLinkedListImplementation();

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


/*
class MapImplementation {
public:

  class NamedVariable {
    friend class MapImplementation;
    AVLTree::Node node;
    Variable variable;
  public:
    NamedVariable(KeyValue key, Variable variable)
      : node(std::move(key)), variable(variable.move()) {}

    Variable& operator*() noexcept {return variable;}
    Variable* operator->() noexcept {return &variable;}

    const Variable& operator*() const noexcept {return variable;}
    const Variable* operator->() const noexcept {return &variable;}

    Variable& getVariable() const noexcept {return variable;}
    Variable getKey() const noexcept {return node.getKey();}
  };

private:
  size_t size;
  AVLTree avl_tree;

  static NamedVariable* convert(Node* node) {
    return reinterpret_cast<NamedVariable*>(
          reinterpret_cast<byte*>(node + offsetof(NamedVariable, node)
        );
  }

public:

  class Iterator {
    AVLTree::Iterator iterator;
    Iterator(AVLTree::Iterator iterator) : iterator(std::move(iterator)) {}
  public:
    Iterator(const Iterator& iterator) : iterator(std::move(iterator.iterator)) {}
    Iterator(Iterator&& iterator) : iterator(std::move(iterator.iterator)) {}

    Iterator& operator=(Iterator& other) noexcept {return *new(this) Iterator(other);}
    Iterator& operator=(Iterator&& other) noexcept {return *new(this) Iterator(std::move(other));}
    Iterator& operator=(Node* node) noexcept {return *new(this) Iterator(node);}
    Iterator& operator=(Node& node) noexcept {return *new(this) Iterator(&node);}

    Iterator& operator++() { ++iterator; return self; }

    Iterator& operator--() { --iterator; return self; }

    Iterator operator++(int) {Iterator tmp(self); ++self; return tmp;}
    Iterator operator--(int) {Iterator tmp(self); --self; return tmp;}

    bool operator==(Iterator other) noexcept {return iterator == other.iterator;}
    bool operator!=(Iterator other) noexcept {return iterator != other.iterator;}

    NamedVariable& operator*() {return *convert(&*iterator);}
    NamedVariable* operator->() {return convert(&*iterator);}
  };

  MapImplementation(const literals::map& map);
  MapImplementation(const MapImplementation& other);
  ~MapImplementation();

  bool isEmpty() const noexcept {return avl_tree.isEmpty();}
  size_t getSize() const noexcept {return size;}

  bool contains(KeyValue value) const;

  bool insert(KeyValue key, Variable variable) {
    NamedVariable* named_variable = new NamedVariable(std::move(key), variable.move());
    if(!avl_tree.insert(&named_variable->node)) return false;
    ++size;
    return true;
  }

  bool remove(KeyValue key) {
    NamedVariable* named_variable = convert(avl_tree.extract(std::move(old_key)));
    if(!named_variable) return false;
    delete named_variable;
    --size;
    return true;
  }

  bool rename(KeyValue old_key, KeyValue new_key) {
    NamedVariable* named_variable = convert(avl_tree.extract(std::move(old_key)));
    if(!named_variable) return false;
    named_variable->node.getKey() = std::move(new_key);
    if(!avl_tree.insert(&named_variable->node)) return false;
    return true;
  }

  NamedVariable& getNamedVariable(KeyValue key) const {
    return convert(avl_tree.get(std::move(key))));
  }

};
*/


}


#endif // RAM_STORAGE_IMPLEMENTATION_H
