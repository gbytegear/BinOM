#ifndef NUMBER_TEST_HXX
#define NUMBER_TEST_HXX

#include "libbinom/include/variables/number.hxx"
#include "test/tester.hxx"

void testNumber() {
  using namespace binom;
  RAIIPerfomanceTest test_perf("Generic value test: ");
  SEPARATOR
  TEST_ANNOUNCE(Number test)
  GRP_PUSH
  {
  LOG("Types & min max value ranges")
  GRP_PUSH

      PRINT_RUN(Number _ui8_max = 255_ui8; Number _ui8_min = 0_ui8;)
      PRINT_RUN(Number _i8_max = 127_i8; Number _i8_min = i8(-128_i8);)
      PRINT_RUN(Number _ui16_max = 65535_ui16; Number _ui16_min = 0_ui16;)
      PRINT_RUN(Number _i16_max = 32767_i16; Number _i16_min = i16(-32768_i16);)
      PRINT_RUN(Number _ui32_max = 4294967295_ui32; Number _ui32_min = 0_ui32;)
      PRINT_RUN(Number _i32_max = 2147483647_i32; Number _i32_min = i32(-2147483648_i32);)
      PRINT_RUN(Number _ui64_max = 18446744073709551615_ui64; Number _ui64_min = 0_ui64;)
      PRINT_RUN(Number _i64_max = 9223372036854775807_i64; Number _i64_min = i64(-9223372036854775808_i64);)

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
    TEST_ANNOUNCE(Number Operators)
        PRINT_RUN(Number a = 20_ui64;);
        PRINT_RUN(Number b = 40_ui64;);
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
        PRINT_RUN(Number c = a + b;)
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

#endif // NUMBER_TEST_HXX
