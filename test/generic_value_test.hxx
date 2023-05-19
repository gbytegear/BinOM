#ifndef GENERIC_VALUE_TEST_HXX
#define GENERIC_VALUE_TEST_HXX

#include "libbinom/include/variables/generic_value.hxx"
#include "test/tester.hxx"

using namespace binom;

void testGenericValue() {
  RAIIPerfomanceTest test_perf("Generic value test: ");
  SEPARATOR
  TEST_ANNOUNCE(GenericValue test)
  GRP_PUSH
  {
  LOG("Types & min max value ranges")
  GRP_PUSH

      PRINT_RUN(GenericValue _ui8_max = 255_ui8; GenericValue _ui8_min = 0_ui8;)
      PRINT_RUN(GenericValue _i8_max = 127_i8; GenericValue _i8_min = i8(-128_i8);)
      PRINT_RUN(GenericValue _ui16_max = 65535_ui16; GenericValue _ui16_min = 0_ui16;)
      PRINT_RUN(GenericValue _i16_max = 32767_i16; GenericValue _i16_min = i16(-32768_i16);)
      PRINT_RUN(GenericValue _ui32_max = 4294967295_ui32; GenericValue _ui32_min = 0_ui32;)
      PRINT_RUN(GenericValue _i32_max = 2147483647_i32; GenericValue _i32_min = 2147483648_ni32;)
      PRINT_RUN(GenericValue _ui64_max = 18446744073709551615_ui64; GenericValue _ui64_min = 0_ui64;)
      PRINT_RUN(GenericValue _i64_max = 9223372036854775807_i64; GenericValue _i64_min = 9223372036854775808_ni64;)

    GRP_PUSH
      LOG("_ui8_min: " << int(_ui8_min) << " _ui8_max: " << int(_ui8_max))
      LOG("_i8_min: " << int(_i8_min) << " _i8_max: " << int(_i8_max))
      TEST(_ui8_min.getValType() == _ui8_max.getValType() && _ui8_max.getValType() == ValType::ui8)
      TEST(_i8_min.getValType() == _i8_max.getValType() && _i8_max.getValType() == ValType::si8)
    GRP_POP

    GRP_PUSH
      LOG("_ui16_min: " << ui16(_ui16_min) << " _ui16_max: " << ui16(_ui16_max))
      LOG("_i16_min: " << i16(_i16_min) << " _i16_max: " << i16(_i16_max))
      TEST(_ui16_min.getValType() == _ui16_max.getValType() && _ui16_max.getValType() == ValType::ui16)
      TEST(_i16_min.getValType() == _i16_max.getValType() && _i16_max.getValType() == ValType::si16)
    GRP_POP

    GRP_PUSH
      LOG("_ui32_min: " << ui32(_ui32_min) << " _ui32_min: " << ui32(_ui32_max))
      LOG("_i32_min: " << i32(_i32_min) << " _i32_min: " << i32(_i32_min))
      TEST(_ui32_min.getValType() == _ui32_max.getValType() && _ui32_max.getValType() == ValType::ui32)
      TEST(_i32_min.getValType() == _i32_max.getValType() && _i32_max.getValType() == ValType::si32)
    GRP_POP

    GRP_PUSH
      LOG("_ui64_min: " << ui64(_ui64_min) << " _ui64_max: " << ui64(_ui64_max))
      LOG("_i64_min: " << i64(_i64_min) << " _i64_max: " << i64(_i64_max))
      TEST(_ui64_min.getValType() == _ui64_max.getValType() && _ui64_max.getValType() == ValType::ui64)
      TEST(_i64_min.getValType() == _i64_max.getValType() && _i64_max.getValType() == ValType::si64)
    GRP_POP

  GRP_POP
  }
  GRP_POP

  GRP_PUSH
  {
    TEST_ANNOUNCE(GenericValue Operators)
        PRINT_RUN(GenericValue a = 20_ui64;);
        PRINT_RUN(GenericValue b = 40_ui64;);
        LOG("a = " << ui64(a))
        LOG("b = " << ui64(b))
        PRINT_RUN(a += b;)
        LOG("a = " << ui64(a))
        PRINT_RUN(a -= b;)
        LOG("a = " << ui64(a))
        PRINT_RUN(a *= b;)
        LOG("a = " << ui64(a))
        PRINT_RUN(a /= b;)
        LOG("a = " << ui64(a))
        PRINT_RUN(b %= a;)
        LOG("b = " << ui64(b))
        PRINT_RUN(b = 12;)
        PRINT_RUN(GenericValue c = a + b;)
        LOG("c = " << ui64(c))
        PRINT_RUN(a.castTo(ValType::f32) = 0.5);
        LOG("a = " << f32(a));
        LOG("b = " << ui64(b))
        PRINT_RUN(a += b;)
        LOG("a = " << f32(a))
        PRINT_RUN(a -= b;)
        LOG("a = " << f32(a))
        PRINT_RUN(a *= b;)
        LOG("a = " << f32(a))
        PRINT_RUN(a /= b;)
        LOG("a = " << f32(a))
        PRINT_RUN(b %= a;)
        LOG("b = " << ui64(b))
        PRINT_RUN(b = 12;)
        PRINT_RUN(c.castTo(ValType::f64) = a + b;)
        LOG("c = " << f64(c))
  }
  GRP_POP
}

#endif
