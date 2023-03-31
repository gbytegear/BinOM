#include "libbinom/include/variables/key_value.hxx"

#include "libbinom/include/variables/number.hxx"
#include "libbinom/include/variables/bit_array.hxx"
#include "libbinom/include/variables/buffer_array.hxx"

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
  : type(VarKeyType::bit_array), data{.bit_array_implementation = value.resource_link->data.bit_array_implementation} {
  value.resource_link->data.pointer = nullptr;
}

template<typename CharT>
requires extended_type_traits::is_char_v<CharT>
KeyValue::KeyValue(const std::basic_string_view<CharT> string_view)
  : type(toKeyType(to_buffer_array_type<CharT>)), data{.buffer_array_implementation = priv::BufferArrayImplementation::create(string_view)} {}

template KeyValue::KeyValue(const std::basic_string_view<char>);
template KeyValue::KeyValue(const std::basic_string_view<signed char>);
template KeyValue::KeyValue(const std::basic_string_view<unsigned char>);
template KeyValue::KeyValue(const std::basic_string_view<wchar_t>);
template KeyValue::KeyValue(const std::basic_string_view<char8_t>);
template KeyValue::KeyValue(const std::basic_string_view<char16_t>);
template KeyValue::KeyValue(const std::basic_string_view<char32_t>);

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
  : type(toKeyBufferType(value.getValType())), data{.buffer_array_implementation = value.resource_link->data.buffer_array_implementation} {
  value.resource_link->data.pointer = nullptr;
}

KeyValue::KeyValue(Variable variable) noexcept {
  switch (variable.getTypeClass()) {
  case binom::VarTypeClass::null: return;
  case binom::VarTypeClass::number:
    new(this) KeyValue(variable.toNumber().move());
  return;
  case binom::VarTypeClass::bit_array:
    new(this) KeyValue(variable.toBitArray().move());
  return;
  case binom::VarTypeClass::buffer_array:
    new(this) KeyValue(variable.toBufferArray().move());
  return;
  default: return;
  }
}

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

KeyValue::KeyValue(KeyValue&& value) noexcept : type(value.type) {
  switch (toTypeClass(type)) {
  case binom::VarTypeClass::null: return;
  case binom::VarTypeClass::number:
    data.ui64_val = value.data.ui64_val;
    value.data.pointer = nullptr;
    value.type = VarKeyType::null;
  return;
  case binom::VarTypeClass::bit_array:
    data.bit_array_implementation = value.data.bit_array_implementation;
    value.data.pointer = nullptr;
    value.type = VarKeyType::null;
  return;
  case binom::VarTypeClass::buffer_array:
    data.buffer_array_implementation = value.data.buffer_array_implementation;
    value.data.pointer = nullptr;
    value.type = VarKeyType::null;
  return;
  case binom::VarTypeClass::invalid_type: default:  return;
  }
}

KeyValue::~KeyValue() {
  switch (getTypeClass()) {
  case binom::VarTypeClass::null: return;
  case binom::VarTypeClass::number:
    data.pointer = nullptr;
    type = VarKeyType::null;
  return;
  case binom::VarTypeClass::bit_array:
    delete data.bit_array_implementation;
    data.pointer = nullptr;
    type = VarKeyType::null;
  return;
  case binom::VarTypeClass::buffer_array:
    delete data.buffer_array_implementation;
    data.pointer = nullptr;
    type = VarKeyType::null;
  return;
  default: return;
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

KeyValue::CompareResult KeyValue::getCompare(const KeyValue& other) const {
  if(toTypeClass(type) > toTypeClass(other.type)) return CompareResult::highter;
  elif(toTypeClass(type) < toTypeClass(other.type)) return CompareResult::lower;

  switch (getTypeClass()) {

  case binom::VarTypeClass::number: {
    GenericValue this_value(getValType(), arithmetic::ArithmeticData{.ui64_val = data.ui64_val}),
                 other_value(other.getValType(), arithmetic::ArithmeticData{.ui64_val = other.data.ui64_val});
    if(this_value > other_value) return CompareResult::highter;
    elif(this_value < other_value) return CompareResult::lower;
    elif(type > other.type) return CompareResult::highter;
    elif(type < other.type) return CompareResult::lower;
    else return CompareResult::equal;
  }

  case binom::VarTypeClass::bit_array: {
    auto this_it = data.bit_array_implementation->begin(),
         this_end = data.bit_array_implementation->end(),
         other_it = other.data.bit_array_implementation->begin(),
         other_end = other.data.bit_array_implementation->end();
    forever {
      if(this_it == this_end && other_it == other_end) {
        if(type > other.type) return CompareResult::highter;
        elif(type < other.type) return CompareResult::lower;
        else return CompareResult::equal;
      }
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
      if(this_it == this_end && other_it == other_end) {
        if(type > other.type) return CompareResult::highter;
        elif(type < other.type) return CompareResult::lower;
        else return CompareResult::equal;
      }
      elif(other_it == other_end) return CompareResult::highter;
      elif(this_it == other_end) return CompareResult::lower;

      if(*this_it > *other_it) return CompareResult::highter;
      elif(*this_it < *other_it) return CompareResult::lower;

      ++this_it;
      ++other_it;
    }
  }

  case binom::VarTypeClass::null:
  case binom::VarTypeClass::invalid_type:
  default: return CompareResult::equal;

  }
}

binom::KeyValue::CompareResult binom::KeyValue::getCompare(KeyValue&& other) const {
  if(toTypeClass(type) > toTypeClass(other.type)) return CompareResult::highter;
  elif(toTypeClass(type) < toTypeClass(other.type)) return CompareResult::lower;

  switch (getTypeClass()) {

  case binom::VarTypeClass::number: {
    GenericValue this_value(getValType(), arithmetic::ArithmeticData{.ui64_val = data.ui64_val}),
                 other_value(other.getValType(), arithmetic::ArithmeticData{.ui64_val = other.data.ui64_val});
    if(this_value > other_value) return CompareResult::highter;
    elif(this_value < other_value) return CompareResult::lower;
    elif(type > other.type) return CompareResult::highter;
    elif(type < other.type) return CompareResult::lower;
    else return CompareResult::equal;
  }

  case binom::VarTypeClass::bit_array: {
    auto this_it = data.bit_array_implementation->begin(),
         this_end = data.bit_array_implementation->end(),
         other_it = other.data.bit_array_implementation->begin(),
         other_end = other.data.bit_array_implementation->end();
    forever {
      if(this_it == this_end && other_it == other_end) {
        if(type > other.type) return CompareResult::highter;
        elif(type < other.type) return CompareResult::lower;
        else return CompareResult::equal;
      }
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
      if(this_it == this_end && other_it == other_end) {
        if(type > other.type) return CompareResult::highter;
        elif(type < other.type) return CompareResult::lower;
        else return CompareResult::equal;
      }
      elif(other_it == other_end) return CompareResult::highter;
      elif(this_it == other_end) return CompareResult::lower;

      if(*this_it > *other_it) return CompareResult::highter;
      elif(*this_it < *other_it) return CompareResult::lower;

      ++this_it;
      ++other_it;
    }
  }

  case binom::VarTypeClass::null:
  case binom::VarTypeClass::invalid_type:
  default: return CompareResult::equal;
  }
}


binom::KeyValue::CompareResult binom::KeyValue::getCompare(const Variable& other) const {
  if(toTypeClass(type) > other.getTypeClass()) return CompareResult::highter;
  elif(toTypeClass(type) < other.getTypeClass()) return CompareResult::lower;

  switch (getTypeClass()) {

  case binom::VarTypeClass::number: {
    GenericValue this_value(getValType(), arithmetic::ArithmeticData{.ui64_val = data.ui64_val}),
                 other_value(other.getValType(), arithmetic::ArithmeticData{.ui64_val = other.toNumber().getRawData()});
    if(this_value > other_value) return CompareResult::highter;
    elif(this_value < other_value) return CompareResult::lower;
    elif(type > toKeyType(other.getType())) return CompareResult::highter;
    elif(type < toKeyType(other.getType())) return CompareResult::lower;
    else return CompareResult::equal;
  }

  case binom::VarTypeClass::bit_array: {
    auto this_it = data.bit_array_implementation->begin(),
         this_end = data.bit_array_implementation->end(),
         other_it = other.toBitArray().begin(),
         other_end = other.toBitArray().end();
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
         other_it = other.toBufferArray().begin(),
         other_end = other.toBufferArray().end();
    forever {
      if(this_it == this_end && other_it == other_end) {
        if(type > toKeyType(other.getType())) return CompareResult::highter;
        elif(type < toKeyType(other.getType())) return CompareResult::lower;
        else return CompareResult::equal;
      }
      elif(other_it == other_end) return CompareResult::highter;
      elif(this_it == other_end) return CompareResult::lower;

      if(*this_it > *other_it) return CompareResult::highter;
      elif(*this_it < *other_it) return CompareResult::lower;

      ++this_it;
      ++other_it;
    }
  }

  case binom::VarTypeClass::null:
  case binom::VarTypeClass::invalid_type:
  default: return CompareResult::equal;
  }
}

binom::KeyValue::CompareResult binom::KeyValue::getCompare(Variable&& other) const {
  if(toTypeClass(type) > other.getTypeClass()) return CompareResult::highter;
  elif(toTypeClass(type) < other.getTypeClass()) return CompareResult::lower;

  switch (getTypeClass()) {

  case binom::VarTypeClass::number: {
    GenericValue this_value(getValType(), arithmetic::ArithmeticData{.ui64_val = data.ui64_val}),
                 other_value(other.getValType(), arithmetic::ArithmeticData{.ui64_val = other.toNumber().getRawData()});
    if(this_value > other_value) return CompareResult::highter;
    elif(this_value < other_value) return CompareResult::lower;
    elif(type > toKeyType(other.getType())) return CompareResult::highter;
    elif(type < toKeyType(other.getType())) return CompareResult::lower;
    else return CompareResult::equal;
  }

  case binom::VarTypeClass::bit_array: {
    auto this_it = data.bit_array_implementation->begin(),
         this_end = data.bit_array_implementation->end(),
         other_it = other.toBitArray().begin(),
         other_end = other.toBitArray().end();
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
         other_it = other.toBufferArray().begin(),
         other_end = other.toBufferArray().end();
    forever {
      if(this_it == this_end && other_it == other_end) {
        if(type > toKeyType(other.getType())) return CompareResult::highter;
        elif(type < toKeyType(other.getType())) return CompareResult::lower;
        else return CompareResult::equal;
      }
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


Variable KeyValue::toVariable() const {
  switch (getTypeClass()) {
  default:
  case binom::VarTypeClass::null: return nullptr;
  case binom::VarTypeClass::number: return toNumber();
  case binom::VarTypeClass::bit_array: return toBitArray();
  case binom::VarTypeClass::buffer_array: return toBufferArray();
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

KeyValue::operator Variable() const {return toVariable();}

KeyValue::operator Number() const {return toNumber();}

KeyValue::operator BitArray() const {return toBitArray();}

KeyValue::operator BufferArray() const {return toBufferArray();}


template<typename CharT>
requires extended_type_traits::is_char_v<CharT>
KeyValue::operator std::basic_string<CharT>() const {
  if(getTypeClass() != VarTypeClass::buffer_array) return std::basic_string<CharT>();

  std::basic_string<CharT> str(data.buffer_array_implementation->getSize(), '\0');

  auto str_it = str.begin(), str_end = str.end();
  auto buffer_array_it = data.buffer_array_implementation->begin(getValType()),
       buffer_array_end = data.buffer_array_implementation->end(getValType());

  for(; str_it != str_end && buffer_array_it != buffer_array_end; (++str_it, ++buffer_array_it))
    *str_it = CharT(*buffer_array_it);

  return str;
}

template KeyValue::operator std::basic_string<char>() const;
template KeyValue::operator std::basic_string<signed char>() const;
template KeyValue::operator std::basic_string<unsigned char>() const;
template KeyValue::operator std::basic_string<wchar_t>() const;
template KeyValue::operator std::basic_string<char8_t>() const;
template KeyValue::operator std::basic_string<char16_t>() const;
template KeyValue::operator std::basic_string<char32_t>() const;



KeyValue& KeyValue::operator=(KeyValue key) {this->~KeyValue(); return *new(this) KeyValue(std::move(key));}

KeyValue& KeyValue::operator=(Number number) {this->~KeyValue(); return *new(this) KeyValue(number.move());}

KeyValue& KeyValue::operator=(BitArray bit_array) {this->~KeyValue(); return *new(this) KeyValue(bit_array.move());}

KeyValue& KeyValue::operator=(BufferArray buffer_array) {this->~KeyValue(); return *new(this) KeyValue(buffer_array.move());}

KeyValue& KeyValue::operator=(Variable variable) {
  this->~KeyValue();
  switch (variable.getTypeClass()) {
  case binom::VarTypeClass::null:
    this->~KeyValue(); return *new(this) KeyValue();
  case binom::VarTypeClass::number:
    this->~KeyValue(); return *new(this) KeyValue(variable.toNumber().move());
  case binom::VarTypeClass::bit_array:
    this->~KeyValue(); return *new(this) KeyValue(variable.toBitArray().move());
  case binom::VarTypeClass::buffer_array:
    this->~KeyValue(); return *new(this) KeyValue(variable.toBufferArray().move());
  default: return *new(this) KeyValue();
  }
}
