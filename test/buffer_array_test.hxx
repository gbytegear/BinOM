#ifndef BUFFER_ARRAY_TEST_H
#define BUFFER_ARRAY_TEST_H

#include "tester.hxx"
#include "libbinom/include/variables/buffer_array.hxx"
#include "print_variable.hxx"


void testBufferArray() {
  RAIIPerfomanceTest test_perf("Bits test: ");
  SEPARATOR;
  TEST_ANNOUNCE(Test BufferArray);
  GRP_PUSH;

  using namespace binom;
  using namespace binom::literals;
  using namespace binom::utils;

  LOG("(bug in [r]): BufferArray ui8_array = ui8arr{0,1,255};");
  BufferArray ui8_array = ui8arr{0,1,255};
  PRINT_RUN(printVariable(ui8_array));
  LOG("(bug in [r]): BufferArray i8_array = i8arr{0,1,255};");
  BufferArray i8_array = i8arr{0,1,127,-128,-1};
  PRINT_RUN(printVariable(i8_array));

  LOG("(bug in [r]): BufferArray ui16_array = ui16arr{0,1,65535};");
  BufferArray ui16_array = ui16arr{0,1,65535};
  PRINT_RUN(printVariable(ui16_array));
  LOG("(bug in [r]): BufferArray i16_array = i16arr{0,1,32767,-32768,-1};");
  BufferArray i16_array = i16arr{0,1,32767,-32768,-1};
  PRINT_RUN(printVariable(i16_array));

  LOG("(bug in [r]): BufferArray ui32_array = ui32arr{0,1,4294967295};");
  BufferArray ui32_array = ui32arr{0,1,4294967295};
  PRINT_RUN(printVariable(ui32_array));
  LOG("(bug in [r]): BufferArray i32_array = i32arr{0,1,2147483647,-2147483648,-1};");
  BufferArray i32_array = i32arr{0,1,2147483647,-2147483648,-1};
  PRINT_RUN(printVariable(i32_array));
  LOG("(bug in [r]): BufferArray f32_array = f32arr{0,3.4E-38,-3.4E-38};");
  BufferArray f32_array = f32arr{0,3.4E-38,-3.4E-38};
  PRINT_RUN(printVariable(f32_array));

  LOG("(bug in [r]): BufferArray ui64_array = ui64arr{0,1,18446744073709551615ull};");
  BufferArray ui64_array = ui64arr{0,1,18446744073709551615ull};
  PRINT_RUN(printVariable(ui64_array));
  LOG("(bug in [r]): BufferArray i64_array = i64arr{0,1,9223372036854775807,-9223372036854775808_i64,-1};");
  BufferArray i64_array = i64arr{0,1,9223372036854775807,9223372036854775808_ni64,-1};
  PRINT_RUN(printVariable(i64_array));
  LOG("(bug in [r]): BufferArray f64_array = f64arr{0,1.7E-308,-1.7E-308};");
  BufferArray f64_array = f64arr{0,1.7E-308,-1.7E-308};
  PRINT_RUN(printVariable(f64_array));

  GRP_POP;
}

#endif // BUFFER_ARRAY_TEST_H
