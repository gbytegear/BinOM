#ifndef KEY_VALUE_HXX
#define KEY_VALUE_HXX

#include "../binom_impl/types.hxx"
#include "generic_value.hxx"

namespace binom {

class KeyValue {
public:

  enum CompareResult : i8 {
    highter = 1,
    lower = -1,
    equal = 0
  };

private:


  union Data {
    void* pointer = nullptr;

    bool  bool_val;
    ui8   ui8_val;
    ui16  ui16_val;
    ui32  ui32_val;
    ui64  ui64_val;
    i8    i8_val;
    i16   i16_val;
    i32   i32_val;
    i64   i64_val;
    f32   f32_val;
    f64   f64_val;

    priv::BitArrayImplementation* bit_array_implementation;
    priv::BufferArrayImplementation* buffer_array_implementation;
  };

  VarKeyType type = VarKeyType::null;
  Data data;
public:

  KeyValue() = default;

  KeyValue(bool value) noexcept;
  KeyValue(ui8 value) noexcept;
  KeyValue(i8 value) noexcept;
  KeyValue(ui16 value) noexcept;
  KeyValue(i16 value) noexcept;
  KeyValue(ui32 value) noexcept;
  KeyValue(i32 value) noexcept;
  KeyValue(f32 value) noexcept;
  KeyValue(ui64 value) noexcept;
  KeyValue(i64 value) noexcept;
  KeyValue(f64 value) noexcept;
  KeyValue(const GenericValue& value) noexcept;
  KeyValue(GenericValue&& value) noexcept;
  KeyValue(const Number& value) noexcept;
  KeyValue(Number&& value) noexcept;

  KeyValue(const literals::bitarr bit_array);
  KeyValue(const BitArray& value) noexcept;
  KeyValue(BitArray&& value) noexcept;

  KeyValue(const literals::ui8arr ui8_array);
  KeyValue(const literals::i8arr i8_array);
  KeyValue(const literals::ui16arr ui16_array);
  KeyValue(const literals::i16arr i16_array);
  KeyValue(const literals::ui32arr ui32_array);
  KeyValue(const literals::i32arr i32_array);
  KeyValue(const literals::f32arr f32_array);
  KeyValue(const literals::ui64arr ui64_array);
  KeyValue(const literals::i64arr i64_array);
  KeyValue(const literals::f64arr f64_array);
  KeyValue(const BufferArray& value) noexcept;
  KeyValue(BufferArray&& value) noexcept;

  KeyValue(Variable variable) noexcept;

  KeyValue(const KeyValue& value) noexcept;
  KeyValue(KeyValue&& value) noexcept;

  ~KeyValue();

  VarKeyType getType() const noexcept;
  VarTypeClass getTypeClass() const noexcept;
  VarType getVarType() const noexcept;
  ValType getValType() const noexcept;
  VarBitWidth getBitWidth() const noexcept;
  VarNumberType getNumberType() const noexcept;
  size_t getElementCount() const noexcept;
  size_t getElementSize() const noexcept;

  CompareResult getCompare(KeyValue& other) const;

  Variable toVariable() const;
  Number toNumber() const;
  BitArray toBitArray() const;
  BufferArray toBufferArray() const;

  operator Variable () const;
  operator Number () const;
  operator BitArray () const;
  operator BufferArray () const;

  KeyValue& operator=(KeyValue key);
  KeyValue& operator=(Number number);
  KeyValue& operator=(BitArray bit_array);
  KeyValue& operator=(BufferArray buffer_array);
  KeyValue& operator=(Variable variable);
};

}


#endif // KEY_VALUE_HXX
