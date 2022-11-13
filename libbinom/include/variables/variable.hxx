#ifndef VARIABLE_H
#define VARIABLE_H

#include "../binom_impl/resource_control.hxx"
#include "../binom_impl/multi_avl_tree.hxx"
#include "generic_value.hxx"

#include <list>

namespace binom {

class Variable {
protected:
  using Link = priv::Link;
  using ResourceData = priv::ResourceData;

  Link resource_link;

  std::shared_mutex& getMutex() const {return resource_link.getMutex();}

  Variable(ResourceData data);
  Variable(Link&& link);

//  static void serializeImpl(const Variable& variable, std::vector<byte>& buffer);
//  static void serializeImpl(const Number& number, std::vector<byte>& buffer);
//  static void serializeImpl(const BitArray& bit_array, std::vector<byte>& buffer);
//  static void serializeImpl(const BufferArray& buffer_array, std::vector<byte>& buffer);
//  static void serializeImpl(const Array& array, std::vector<byte>& buffer);
//  static void serializeImpl(const List& dl_list, std::vector<byte>& buffer);
//  static void serializeImpl(const Map& map, std::vector<byte>& buffer);
//  static void serializeImpl(const Table& bit_array, std::vector<byte>& buffer);

public:
  using NewNodePosition = binom::priv::MultiAVLTree::NewNodePosition;
//  using TransactionLock = shared_recursive_mtx::TransactionLock;

  // Null
  Variable() noexcept;
  Variable(decltype(nullptr)) noexcept;

  // Number
  Variable(bool value) noexcept;
  Variable(ui8 value) noexcept;
  Variable(i8 value) noexcept;
  Variable(ui16 value) noexcept;
  Variable(i16 value) noexcept;
  Variable(ui32 value) noexcept;
  Variable(i32 value) noexcept;
  Variable(f32 value) noexcept;
  Variable(ui64 value) noexcept;
  Variable(i64 value) noexcept;
  Variable(f64 value) noexcept;
  Variable(const GenericValue& value) noexcept;
  Variable(GenericValue&& value) noexcept;

  // BitArray
  Variable(const literals::bitarr bit_array);

  // BufferArray
  template<typename CharT>
  requires extended_type_traits::is_char_v<CharT>
  Variable(const std::basic_string_view<CharT> string_view);

  template<typename CharT>
  requires extended_type_traits::is_char_v<CharT>
  Variable(const CharT* c_str) : Variable(std::basic_string_view<CharT>(c_str)) {}

  Variable(const literals::ui8arr ui8_array);
  Variable(const literals::i8arr i8_array);
  Variable(const literals::ui16arr ui16_array);
  Variable(const literals::i16arr i16_array);
  Variable(const literals::ui32arr ui32_array);
  Variable(const literals::i32arr i32_array);
  Variable(const literals::f32arr f32_array);
  Variable(const literals::ui64arr ui64_array);
  Variable(const literals::i64arr i64_array);
  Variable(const literals::f64arr f64_array);

  // Array
  Variable(const literals::arr array);

  // List
  Variable(const literals::list list);

  // Map
  Variable(const literals::map map);

  // MultiMap
  Variable(const literals::multimap multimap, NewNodePosition pos = NewNodePosition::back);

  // Move & Copy
  Variable(Variable&& other) noexcept;
  Variable(const Variable& other) noexcept;

  Variable move() noexcept;
  const Variable move() const noexcept;

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept;

  static shared_recursive_mtx::TransactionLock makeTransaction(std::initializer_list<const Variable> variables, MtxLockType lock_type = MtxLockType::unique_locked);

  // Properties
  bool isResourceExist() const noexcept;
  VarType getType() const noexcept;
  VarTypeClass getTypeClass() const noexcept;
  ValType getValType() const noexcept;
  VarBitWidth getBitWidth() const noexcept;
  VarNumberType getNumberType() const noexcept;
  size_t getElementCount() const noexcept;
  size_t getElementSize() const noexcept;
  ui64 getLinkCount() const noexcept;

  // Downcast operators
  operator Number& ();
  operator BitArray& ();
  operator BufferArray& ();
  operator Array& ();
  operator List& ();
  operator Map& ();
  operator MultiMap& ();

  operator const Number& () const;
  operator const BitArray& () const;
  operator const BufferArray& () const;
  operator const Array& () const;
  operator const List& () const;
  operator const Map& () const;
  operator const MultiMap& () const;

  // Downcast methods
  Number& toNumber();
  BitArray& toBitArray();
  BufferArray& toBufferArray();
  Array& toArray();
  List& toList();
  Map& toMap();
  MultiMap& toMultiMap();

  const Number& toNumber() const;
  const BitArray& toBitArray() const;
  const BufferArray& toBufferArray() const;
  const Array& toArray() const;
  const List& toList() const;
  const Map& toMap() const;
  const MultiMap& toMultiMap() const;

  Variable& operator=(const Variable& other);
  Variable& operator=(Variable&& other);

  Variable& changeLink(const Variable& other);
  Variable& changeLink(Variable&& other);

  std::vector<byte> serialize() const;
  void serializeToStraem(std::ostream& os) const;

  inline Variable& upcast() {return self;}
  inline const Variable& upcast() const {return self;}
};

}

#endif // VARIABLE_H
