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
  operator List& () = delete;
  operator Map& () = delete;
  operator MultiMap& () = delete;
  operator Table& () = delete;

  Number& toNumber() = delete;
  BitArray& toBitArray() = delete;
  BufferArray& toBufferArray() = delete;
  Array& toArray() = delete;
  List& toList() = delete;
  Map& toMap() = delete;
  MultiMap& toMultiMap() = delete;
  Table& toTable() = delete;

  operator const BitArray& () const = delete;
  operator const BufferArray& () const = delete;
  operator const Array& () const = delete;
  operator const List& () const = delete;
  operator const Map& () const = delete;
  operator const MultiMap& () const = delete;
  operator const Table& () const = delete;

  const Number& toNumber() const = delete;
  const BitArray& toBitArray() const = delete;
  const BufferArray& toBufferArray() const = delete;
  const Array& toArray() const = delete;
  const List& toList() const = delete;
  const Map& toMap() const = delete;
  const MultiMap& toMultiMap() const = delete;
  const Table& toTable() const = delete;

  Variable& operator=(const Variable& other) = delete;
  Variable& operator=(Variable&& other) = delete;

  Variable& changeLink(const Variable& other) = delete;
  Variable& changeLink(Variable&& other) = delete;


  arithmetic::ArithmeticData& getArithmeticDataImpl() const;
  ValType getValTypeImpl() const;

  static bool checkLock(const OptionalSharedRecursiveLock& lock) noexcept;

  void setTypeImpl(ValType new_type) noexcept;

  Number(priv::Link&& link) noexcept;

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
  using binom::Variable::operator=;

  Number& changeLink(const Number& other);
  Number& changeLink(Number&& other);
};

}

#endif // NUMBER_HXX
