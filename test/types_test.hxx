#ifndef TYPES_TEST_HXX
#define TYPES_TEST_HXX

#include "libbinom/include/utils/types.hxx"
#include "test/tester.hxx"

using namespace binom;

void testTypesConversions() {
  SEPARATOR
  TEST_ANOUNCE(Test types conversions)
  GRP(
    LOG("VarType to TypeClass")
    GRP(
      TEST(getTypeClass(VarType::null) == VarTypeClass::null)
      TEST(getTypeClass(VarType::ui8) == VarTypeClass::number)
      TEST(getTypeClass(VarType::si8) == VarTypeClass::number)
      TEST(getTypeClass(VarType::ui16) == VarTypeClass::number)
      TEST(getTypeClass(VarType::si16) == VarTypeClass::number)
      TEST(getTypeClass(VarType::ui32) == VarTypeClass::number)
      TEST(getTypeClass(VarType::si32) == VarTypeClass::number)
      TEST(getTypeClass(VarType::f32) == VarTypeClass::number)
      TEST(getTypeClass(VarType::ui64) == VarTypeClass::number)
      TEST(getTypeClass(VarType::si64) == VarTypeClass::number)
      TEST(getTypeClass(VarType::f64) == VarTypeClass::number)
      TEST(getTypeClass(VarType::ui8_array) == VarTypeClass::buffer_array)
      TEST(getTypeClass(VarType::si8_array) == VarTypeClass::buffer_array)
      TEST(getTypeClass(VarType::ui16_array) == VarTypeClass::buffer_array)
      TEST(getTypeClass(VarType::si16_array) == VarTypeClass::buffer_array)
      TEST(getTypeClass(VarType::ui32_array) == VarTypeClass::buffer_array)
      TEST(getTypeClass(VarType::si32_array) == VarTypeClass::buffer_array)
      TEST(getTypeClass(VarType::f32_array) == VarTypeClass::buffer_array)
      TEST(getTypeClass(VarType::ui64_array) == VarTypeClass::buffer_array)
      TEST(getTypeClass(VarType::si64_array) == VarTypeClass::buffer_array)
      TEST(getTypeClass(VarType::f64_array) == VarTypeClass::buffer_array)
      TEST(getTypeClass(VarType::array) == VarTypeClass::array)
      TEST(getTypeClass(VarType::less_map) == VarTypeClass::map)
      TEST(getTypeClass(VarType::greater_map) == VarTypeClass::map)
      TEST(getTypeClass(VarType::separator) == VarTypeClass::invalid_type)
      TEST(getTypeClass(VarType::invalid_type) == VarTypeClass::invalid_type)
    )
    LOG("VarType bit width")
    GRP(
      TEST(getBitWidth(VarType::ui8) == VarBitWidth::byte)
      TEST(getBitWidth(VarType::si8) == VarBitWidth::byte)
      TEST(getBitWidth(VarType::ui8_array) == VarBitWidth::byte)
      TEST(getBitWidth(VarType::si8_array) == VarBitWidth::byte)
      TEST(getBitWidth(VarType::ui16) == VarBitWidth::word)
      TEST(getBitWidth(VarType::si16) == VarBitWidth::word)
      TEST(getBitWidth(VarType::ui16_array) == VarBitWidth::word)
      TEST(getBitWidth(VarType::si16_array) == VarBitWidth::word)
      TEST(getBitWidth(VarType::ui32) == VarBitWidth::dword)
      TEST(getBitWidth(VarType::si32) == VarBitWidth::dword)
      TEST(getBitWidth(VarType::f32) == VarBitWidth::dword)
      TEST(getBitWidth(VarType::ui32_array) == VarBitWidth::dword)
      TEST(getBitWidth(VarType::si32_array) == VarBitWidth::dword)
      TEST(getBitWidth(VarType::f32_array) == VarBitWidth::dword)
      TEST(getBitWidth(VarType::ui64) == VarBitWidth::qword)
      TEST(getBitWidth(VarType::si64) == VarBitWidth::qword)
      TEST(getBitWidth(VarType::f64) == VarBitWidth::qword)
      TEST(getBitWidth(VarType::ui64_array) == VarBitWidth::qword)
      TEST(getBitWidth(VarType::si64_array) == VarBitWidth::qword)
      TEST(getBitWidth(VarType::f64_array) == VarBitWidth::qword)
    )
    LOG("ValType bit width")
    GRP(
      TEST(getBitWidth(ValType::ui8) == VarBitWidth::byte)
      TEST(getBitWidth(ValType::si8) == VarBitWidth::byte)
      TEST(getBitWidth(ValType::ui16) == VarBitWidth::word)
      TEST(getBitWidth(ValType::si16) == VarBitWidth::word)
      TEST(getBitWidth(ValType::ui32) == VarBitWidth::dword)
      TEST(getBitWidth(ValType::si32) == VarBitWidth::dword)
      TEST(getBitWidth(ValType::f32) == VarBitWidth::dword)
      TEST(getBitWidth(ValType::ui64) == VarBitWidth::qword)
      TEST(getBitWidth(ValType::si64) == VarBitWidth::qword)
      TEST(getBitWidth(ValType::f64) == VarBitWidth::qword)
    )
  )
}

#endif // TYPES_TEST_HXX
