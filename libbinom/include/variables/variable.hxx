#ifndef VARIABLE_H
#define VARIABLE_H

#include "../utils/resource_control.hxx"
#include "generic_value.hxx"

namespace binom {

class Variable {
protected:
  using Link = priv::Link;
  using ResourceData = priv::ResourceData;

  Link resource_link;

  Variable(ResourceData data);
  Variable(Link&& link);

public:
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

  Variable(const literals::sllist singly_linked_list);

  // Move & Copy
  Variable(const Variable&& other) noexcept;
  Variable(const Variable& other) noexcept;

  Variable getReference() const noexcept;

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept;

  // Properties
  bool isResourceExist() const noexcept;
  VarType getType() const noexcept;
  VarTypeClass getTypeClass() const noexcept;
  ValType getValType() const noexcept;
  VarBitWidth getBitWidth() const noexcept;
  VarNumberType getNumberType() const noexcept;
  size_t getElementCount() const noexcept;
  size_t getElementSize() const noexcept;


  // Downcast operators
  operator Number& () const;
  operator BitArray& () const;
  operator BufferArray& () const;
  operator Array& () const;
  operator SinglyLinkedList& () const;
  operator DoublyLinkedList& () const;
  operator Map& () const;

  // Downcast methods
  Number& toNumber() const;
  BitArray& toBitArray() const;
  BufferArray& toBufferArray() const;
  Array& toArray() const;
  SinglyLinkedList& toSinglyLinkedList() const;
  DoublyLinkedList& toDoublyLinkedList() const;
  Map& toMap() const;

  Variable& operator=(const Variable& other);
  Variable& operator=(Variable&& other);

  Variable& changeLink(const Variable& other);
  Variable& changeLink(Variable&& other);
};

}

#endif // VARIABLE_H
