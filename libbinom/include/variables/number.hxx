#ifndef NUMBER_HXX
#define NUMBER_HXX

#include "variable.hxx"

namespace binom {

class Number :
    public arithmetic::ArithmeticTypeBase<Number>,
    arithmetic::EnableCopyableArithmetic,
    public arithmetic::CastableArithmeticTypeBase<Number>,
    public Variable {
  USE_ARITHMETIC
  USE_ARITHMETIC_CAST

  operator BitArray& () = delete;
  operator BufferArray& () = delete;
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

  operator const BitArray& () const = delete;
  operator const BufferArray& () const = delete;
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


  arithmetic::ArithmeticData& getArithmeticDataImpl() const;
  ValType getValTypeImpl() const;

  static bool checkLock(const OptionalSharedRecursiveLock& lock) noexcept;

  void setTypeImpl(ValType new_type) noexcept;

  Number(priv::Link&& link);

  friend class Variable;
  friend class KeyValue;

public:
  using Variable::getLock;
  using Variable::getValType;
  using Variable::getBitWidth;
  using Variable::getNumberType;

  Number() noexcept;
  Number(bool value) noexcept;
  Number(ui8 value) noexcept;
  Number(i8 value) noexcept;
  Number(ui16 value) noexcept;
  Number(i16 value) noexcept;
  Number(ui32 value) noexcept;
  Number(i32 value) noexcept;
  Number(f32 value) noexcept;
  Number(ui64 value) noexcept;
  Number(i64 value) noexcept;
  Number(f64 value) noexcept;

  Number(const Number& other) noexcept;
  Number(const Number&& other) noexcept;

  Number(const GenericValue& value) noexcept;
  Number(const GenericValue&& value) noexcept;

  Number move() noexcept;
  const Number move() const noexcept;

  using arithmetic::ArithmeticTypeBase<Number>::operator=;
  Number& changeLink(const Number& other);
  Number& changeLink(Number&& other);
};

}

#endif // NUMBER_HXX
