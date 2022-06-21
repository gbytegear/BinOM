#include "libbinom/include/variables/key_value.hxx"

using namespace binom;
using namespace binom::priv;

KeyValue::KeyValue(bool value) noexcept : type(VarKeyType::boolean), data{.bool_val = value} {}
KeyValue::KeyValue(ui8 value) noexcept  : type(VarKeyType::ui8), data{.ui8_val = value} {}
KeyValue::KeyValue(i8 value) noexcept   : type(VarKeyType::si8), data{.i8_val = value} {}
KeyValue::KeyValue(ui16 value) noexcept : type(VarKeyType::ui16), data{.ui16_val = value} {}
KeyValue::KeyValue(i16 value) noexcept  : type(VarKeyType::si16), data{.i16_val = value} {}
KeyValue::KeyValue(ui32 value) noexcept : type(VarKeyType::ui32), data{.ui32_val = value} {}
KeyValue::KeyValue(i32 value) noexcept  : type(VarKeyType::si32), data{.i32_val = value} {}
KeyValue::KeyValue(f32 value) noexcept  : type(VarKeyType::f32), data{.f32_val = value} {}
KeyValue::KeyValue(ui64 value) noexcept : type(VarKeyType::ui64), data{.ui64_val = value} {}
KeyValue::KeyValue(i64 value) noexcept  : type(VarKeyType::si64), data{.i64_val = value} {}
KeyValue::KeyValue(f64 value) noexcept  : type(VarKeyType::f64), data{.f64_val = value} {}
KeyValue::KeyValue(const GenericValue& value) noexcept
  : type(toKeyType(value.getValType())), data{.ui64_val = value.getArithmeticDataImpl().ui64_val} {}
KeyValue::KeyValue(GenericValue&& value) noexcept
  : type(toKeyType(value.getValType())), data{.ui64_val = value.getArithmeticDataImpl().ui64_val} {}
KeyValue::KeyValue(const Number& value) noexcept
  : type(toKeyType(value.getValType())), data{.ui64_val = value.getArithmeticDataImpl().ui64_val} {}
KeyValue::KeyValue(Number&& value) noexcept
  : type(toKeyType(value.getValType())), data{.ui64_val = value.getArithmeticDataImpl().ui64_val} {}

KeyValue::KeyValue(const literals::bitarr bit_array)
  : type(VarKeyType::bit_array), data{.bit_array_implementation = priv::BitArrayImplementation::create(bit_array)} {}
KeyValue::KeyValue(const BitArray& value) noexcept
  : type(VarKeyType::bit_array), data{.bit_array_implementation = priv::BitArrayImplementation::copy(value.getData())} {}
KeyValue::KeyValue(BitArray&& value) noexcept
  : type(VarKeyType::bit_array), data{.bit_array_implementation = priv::BitArrayImplementation::copy(value.getData())} {}

KeyValue::KeyValue(const literals::ui8arr ui8_array)
  : type(VarKeyType::ui8_array), data{.buffer_array_implementation = priv::BufferArrayImplementation::create(ui8_array)} {}
KeyValue::KeyValue(const literals::i8arr i8_array)
  : type(VarKeyType::si8_array), data{.buffer_array_implementation = priv::BufferArrayImplementation::create(i8_array)} {}
KeyValue::KeyValue(const literals::ui16arr ui16_array)
  : type(VarKeyType::ui16_array), data{.buffer_array_implementation = priv::BufferArrayImplementation::create(ui16_array)} {}
KeyValue::KeyValue(const literals::i16arr i16_array)
  : type(VarKeyType::si16_array), data{.buffer_array_implementation = priv::BufferArrayImplementation::create(i16_array)} {}
KeyValue::KeyValue(const literals::ui32arr ui32_array)
  : type(VarKeyType::ui32_array), data{.buffer_array_implementation = priv::BufferArrayImplementation::create(ui32_array)} {}
KeyValue::KeyValue(const literals::i32arr i32_array)
  : type(VarKeyType::si32_array), data{.buffer_array_implementation = priv::BufferArrayImplementation::create(i32_array)} {}
KeyValue::KeyValue(const literals::f32arr f32_array)
  : type(VarKeyType::f32_array), data{.buffer_array_implementation = priv::BufferArrayImplementation::create(f32_array)} {}
KeyValue::KeyValue(const literals::ui64arr ui64_array)
  : type(VarKeyType::ui64_array), data{.buffer_array_implementation = priv::BufferArrayImplementation::create(ui64_array)} {}
KeyValue::KeyValue(const literals::i64arr i64_array)
  : type(VarKeyType::si64_array), data{.buffer_array_implementation = priv::BufferArrayImplementation::create(i64_array)} {}
KeyValue::KeyValue(const literals::f64arr f64_array)
  : type(VarKeyType::f64_array), data{.buffer_array_implementation = priv::BufferArrayImplementation::create(f64_array)} {}
KeyValue::KeyValue(const BufferArray& value) noexcept
  : type(toKeyType(value.getValType())), data{.buffer_array_implementation = priv::BufferArrayImplementation::copy(value.getData())} {}
KeyValue::KeyValue(BufferArray&& value) noexcept
  : type(toKeyType(value.getValType())), data{.buffer_array_implementation = priv::BufferArrayImplementation::copy(value.getData())} {}

KeyValue::KeyValue(const KeyValue& value) noexcept : type(value.type) {
  switch (toTypeClass(type)) {
  case binom::VarTypeClass::null: return;
  case binom::VarTypeClass::number:
    data.ui64_val = value.data.ui64_val;
  return;
  case binom::VarTypeClass::bit_array:
    data.bit_array_implementation = priv::BitArrayImplementation::copy(value.data.bit_array_implementation);
  return;
  case binom::VarTypeClass::buffer_array:
    data.buffer_array_implementation = priv::BufferArrayImplementation::copy(value.data.buffer_array_implementation);
  return;
  case binom::VarTypeClass::invalid_type: default:  return;
  }
}

KeyValue::KeyValue(const KeyValue&& value) noexcept : type(value.type) {
  switch (toTypeClass(type)) {
  case binom::VarTypeClass::null: return;
  case binom::VarTypeClass::number:
    data.ui64_val = value.data.ui64_val;
  return;
  case binom::VarTypeClass::bit_array:
    data.bit_array_implementation = value.data.bit_array_implementation;
    const_cast<KeyValue&>(value).data.bit_array_implementation = nullptr;
  return;
  case binom::VarTypeClass::buffer_array:
    data.buffer_array_implementation = value.data.buffer_array_implementation;
    const_cast<KeyValue&>(value).data.buffer_array_implementation = nullptr;
  return;
  case binom::VarTypeClass::invalid_type: default:  return;
  }
}

VarKeyType KeyValue::getType() const noexcept {return type;}
VarTypeClass KeyValue::getTypeClass() const noexcept {return toTypeClass(type);}
VarType KeyValue::getVarType() const noexcept {return toVarType(type);}
ValType KeyValue::getValType() const noexcept {return toValueType(type);}
VarBitWidth KeyValue::getBitWidth() const noexcept {return toBitWidth(toValueType(type));}
VarNumberType KeyValue::getNumberType() const noexcept {return toNumberType(toValueType(type));}

size_t KeyValue::getElementCount() const noexcept {
  switch (getTypeClass()) {
  default:
  case binom::VarTypeClass::null: return 0;
  case binom::VarTypeClass::number: return 1;
  case binom::VarTypeClass::bit_array: return data.bit_array_implementation->getBitSize();
  case binom::VarTypeClass::buffer_array: return data.buffer_array_implementation->getSize();
  }
}

size_t KeyValue::getElementSize() const noexcept {
  switch (getTypeClass()) {
  default:
  case binom::VarTypeClass::null: return 0;
  case binom::VarTypeClass::number:
  case binom::VarTypeClass::bit_array:
  case binom::VarTypeClass::buffer_array: return size_t(getBitWidth());
  }
}

KeyValue::CompareResult KeyValue::getCompare(KeyValue& other) const {
  if(type > other.type) return CompareResult::highter;
  elif(type < other.type) return CompareResult::lower;

  switch (getTypeClass()) {

  case binom::VarTypeClass::number: {
    GenericValue this_value(getValType(), arithmetic::ArithmeticData{.ui64_val = data.ui64_val}),
                 other_value(other.getValType(), arithmetic::ArithmeticData{.ui64_val = other.data.ui64_val});
    if(this_value > other_value) return CompareResult::highter;
    elif(this_value < other_value) return CompareResult::lower;
    else return CompareResult::equal;
  }

  case binom::VarTypeClass::bit_array: {
    auto this_it = data.bit_array_implementation->begin(),
         this_end = data.bit_array_implementation->end(),
         other_it = other.data.bit_array_implementation->begin(),
         other_end = other.data.bit_array_implementation->end();
    forever {
      if(this_it == this_end && other_it == other_end) return CompareResult::equal;
      elif(other_it == other_end) return CompareResult::highter;
      elif(this_it == other_end) return CompareResult::lower;

      if(bool(*this_it) > bool(*other_it)) return CompareResult::highter;
      elif(bool(*this_it) < bool(*other_it)) return CompareResult::lower;

      ++this_it;
      ++other_it;
    }
  }

  case binom::VarTypeClass::buffer_array:{
    auto this_it = data.buffer_array_implementation->begin(getValType()),
         this_end = data.buffer_array_implementation->end(getValType()),
         other_it = other.data.buffer_array_implementation->begin(other.getValType()),
         other_end = other.data.buffer_array_implementation->end(other.getValType());
    forever {
      if(this_it == this_end && other_it == other_end) return CompareResult::equal;
      elif(other_it == other_end) return CompareResult::highter;
      elif(this_it == other_end) return CompareResult::lower;

      if(*this_it > *other_it) return CompareResult::highter;
      elif(*this_it < *other_it) return CompareResult::lower;

      ++this_it;
      ++other_it;
    }
  }

  case binom::VarTypeClass::null:
  case binom::VarTypeClass::invalid_type: default:
  return CompareResult::equal;

  }
}

Number KeyValue::toNumber() const {
  if(getTypeClass() == VarTypeClass::number)
    return Number(priv::Link(ResourceData{getVarType(), {.ui64_val = data.ui64_val}}));
  else return Number();
}

BitArray KeyValue::toBitArray() const {
  if(getTypeClass() == VarTypeClass::bit_array)
    return BitArray(priv::Link(ResourceData{getVarType(), {.bit_array_implementation = priv::BitArrayImplementation::copy(data.bit_array_implementation)}}));
  else return BitArray();
}

BufferArray KeyValue::toBufferArray() const {
  if(getTypeClass() == VarTypeClass::buffer_array)
    return BufferArray(priv::Link(ResourceData{getVarType(), {.buffer_array_implementation = priv::BufferArrayImplementation::copy(data.buffer_array_implementation)}}));
  else return BufferArray();
}
