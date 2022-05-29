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


  arithmetic::ArithmeticData& getArithmeticDataImpl() const;
  ValType getValTypeImpl() const;

  static bool checkLock(const OptionalSharedRecursiveLock& lock) noexcept;

  void setTypeImpl(ValType new_type) noexcept;

  friend class Variable;
  Number(priv::Link&& link);

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

  Number getReference() noexcept;

  using arithmetic::ArithmeticTypeBase<Number>::operator=;
  Number& changeLink(const Number& other);
  Number& changeLink(Number&& other);
};

}

#endif // NUMBER_HXX
