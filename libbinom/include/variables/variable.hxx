#ifndef VARIABLE_H
#define VARIABLE_H

#include "../utils/resource_control.hxx"
#include "generic_value.hxx"
#include "mutex"

namespace binom {

class Variable {
protected:
  using Link = priv::Link;
  using ResourceData = priv::ResourceData;

  Link resource_link;

  std::shared_mutex& getMutex() {return resource_link.getMutex();}

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

  // SinglyLinkedList
  Variable(const literals::sllist singly_linked_list);

  // DoublyLinkedList
  Variable(const literals::dllist doubly_linked_list);

  // Move & Copy
  Variable(const Variable&& other) noexcept;
  Variable(const Variable& other) noexcept;

  Variable getReference() noexcept;
  const Variable getReference() const noexcept;

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept;

  template<class... Var>
  static void transaction(Var&... vars) {
    static_assert ((std::is_same_v<Variable, std::remove_reference_t<Var>> && ...) ||
                   (std::is_base_of_v<Variable, std::remove_reference_t<Var>> && ...));
    std::lock((vars.getMutex())...);
  }

//  template<class... Var>
//  class Transaction {
//    std::tuple<Var...> variables;
//    bool locked = true;
//  public:
//    Transaction(Var&... vars) : variables(vars...) {
//      static_assert ((std::is_same_v<Variable, std::remove_reference_t<Var>> && ...) ||
//                     (std::is_base_of_v<Variable, std::remove_reference_t<Var>> && ...));
//      std::lock((vars.getMutex())...);
//    }

//    void unlock() {
//      if(locked)std::apply([](auto&... vars) {((vars.getMutex().unlock()),...);}, variables);
//      locked = false;
//    }

//    ~Transaction() {unlock();}
//  };


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
  operator SinglyLinkedList& ();
  operator DoublyLinkedList& ();
  operator Map& ();

  operator const Number& () const;
  operator const BitArray& () const;
  operator const BufferArray& () const;
  operator const Array& () const;
  operator const SinglyLinkedList& () const;
  operator const DoublyLinkedList& () const;
  operator const Map& () const;

  // Downcast methods
  Number& toNumber();
  BitArray& toBitArray();
  BufferArray& toBufferArray();
  Array& toArray();
  SinglyLinkedList& toSinglyLinkedList();
  DoublyLinkedList& toDoublyLinkedList();
  Map& toMap();

  const Number& toNumber() const;
  const BitArray& toBitArray() const;
  const BufferArray& toBufferArray() const;
  const Array& toArray() const;
  const SinglyLinkedList& toSinglyLinkedList() const;
  const DoublyLinkedList& toDoublyLinkedList() const;
  const Map& toMap() const;

  Variable& operator=(const Variable& other);
  Variable& operator=(Variable&& other);

  Variable& changeLink(const Variable& other);
  Variable& changeLink(Variable&& other);
};

}

#endif // VARIABLE_H
