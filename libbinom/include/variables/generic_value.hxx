#ifndef GENERIC_VALUE_HXX
#define GENERIC_VALUE_HXX

#include "../utils/generic_arithmetic.hxx"

namespace binom {

class GenericValueIterator;
class GenericValueRef;

class GenericValue :
    public arithmetic::CopyableArithmeticTypeBase<GenericValue>,
    public arithmetic::CastableArithmeticTypeBase<GenericValue> {

  ValType type;
  arithmetic::ArithmeticData data;

  // ArithmeticTypeBase & CopyableArithmeticTypeBase Implementation
  friend class ArithmeticTypeBase<GenericValue>;
  arithmetic::ArithmeticData& getArithmeticDataImpl() const {return const_cast<arithmetic::ArithmeticData&>(data);}
  ValType getTypeImpl() const {return type;}
  // CastableArithmeticTypeBase implementation
  friend class CastableArithmeticTypeBase<GenericValue>;
  void setTypeImpl(ValType new_type) noexcept {type = new_type;}

public:
  GenericValue() noexcept {}
  GenericValue(bool value) noexcept : type(ValType::boolean), data{.bool_val = value} {}
  GenericValue(ui8 value) noexcept : type(ValType::ui8), data{.ui8_val = value} {}
  GenericValue(i8 value) noexcept : type(ValType::si8), data{.i8_val = value} {}
  GenericValue(ui16 value) noexcept : type(ValType::ui16), data{.ui16_val = value} {}
  GenericValue(i16 value) noexcept : type(ValType::si16), data{.i16_val = value} {}
  GenericValue(ui32 value) noexcept : type(ValType::ui32), data{.ui32_val = value} {}
  GenericValue(i32 value) noexcept : type(ValType::si32), data{.i32_val = value} {}
  GenericValue(f32 value) noexcept : type(ValType::f32), data{.f32_val = value} {}
  GenericValue(ui64 value) noexcept : type(ValType::ui64), data{.ui64_val = value} {}
  GenericValue(i64 value) noexcept : type(ValType::si64), data{.i64_val = value} {}
  GenericValue(f64 value) noexcept : type(ValType::f64), data{.f64_val = value} {}
  GenericValue(const GenericValue& other) : type(other.type), data{.ui64_val = other.data.ui64_val} {}
  GenericValue(GenericValue&& other) : type(other.type), data{.ui64_val = other.data.ui64_val} {}

  GenericValue& operator=(bool value) noexcept {return ArithmeticTypeBase::operator=(value);}
  GenericValue& operator=(ui8 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
  GenericValue& operator=(i8 value) noexcept   {return ArithmeticTypeBase::operator=(value);}
  GenericValue& operator=(ui16 value) noexcept {return ArithmeticTypeBase::operator=(value);}
  GenericValue& operator=(i16 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
  GenericValue& operator=(ui32 value) noexcept {return ArithmeticTypeBase::operator=(value);}
  GenericValue& operator=(i32 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
  GenericValue& operator=(f32 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
  GenericValue& operator=(ui64 value) noexcept {return ArithmeticTypeBase::operator=(value);}
  GenericValue& operator=(i64 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
  GenericValue& operator=(f64 value) noexcept  {return ArithmeticTypeBase::operator=(value);}
  GenericValue& operator=(const GenericValue& value) noexcept {return ArithmeticTypeBase::operator=(value);}
  GenericValue& operator=(GenericValue&& value) noexcept      {return ArithmeticTypeBase::operator=(std::move(value));}

};

class GenericValueRef : public arithmetic::ArithmeticTypeBase<GenericValueRef> {
  friend class arithmetic::ArithmeticTypeBase<GenericValueRef>;

  ValType value_type;
  union pointer {
      void* ptr;
      bool* bool_ptr;
      ui8* ui8_ptr;
      ui16* ui16_ptr;
      ui32* ui32_ptr;
      ui64* ui64_ptr;
      i8* i8_ptr;
      i16* i16_ptr;
      i32* i32_ptr;
      i64* i64_ptr;
      f32* f32_ptr;
      f64* f64_ptr;
      arithmetic::ArithmeticData* num_data_ptr;
      pointer(void* ptr) : ptr(ptr) {}
    } ptr;


  arithmetic::ArithmeticData& getArithmeticDataImpl() const noexcept {return *ptr.num_data_ptr;}
  ValType getTypeImpl() const noexcept {return value_type;}


  friend class GenericValueIterator;
  friend class Number;
  friend class BufferArray;

  GenericValueRef(ValType value_type, void* ptr)
    : value_type(value_type), ptr(ptr) {}
  GenericValueRef(VarType variable_type, void* ptr)
    : value_type(getValueType(variable_type)), ptr(ptr) {}
public:
  GenericValueRef(const GenericValueRef& other) : value_type(other.value_type), ptr(other.ptr.ptr) {}
  GenericValueRef(GenericValueRef&& other) : value_type(other.value_type), ptr(other.ptr.ptr) {}
  GenericValueRef(const GenericValueIterator& it);
  GenericValueRef(GenericValueIterator&& it);


};

class GenericValueIterator {

};

}

#endif // GENERIC_VALUE_HXX
