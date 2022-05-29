#ifndef VARIABLE_TEST_HXX
#define VARIABLE_TEST_HXX

#include "tester.hxx"
#include "libbinom/include/utils/print_variable.hxx"

using namespace binom;

void testVariable() {
  RAIIPerfomanceTest test_perf("Variable test: ");
  TEST_ANNOUNCE(Variable-reference test); GRP_PUSH;
  {
    PRINT_RUN(Variable a = 256_ui64);

    LOG("NOTE: Number b = a.getReference()");
    LOG("getReference() -> rvalue-ref");
    LOG("operator T&() -> lvalue-ref");
    LOG("Number(const Number&) *MAKES COPY*")
    PRINT_RUN(Number b = a.toNumber().getReference());
    PRINT_RUN(Variable c = a.getReference());
    TEST(ui64(a.toNumber()) == 256);
    TEST(ui64(b) == 256);
    PRINT_RUN(b = 512;)
    TEST(ui64(a.toNumber()) == 512);
    TEST(ui64(b) == 512);
  }
  GRP_POP;

  TEST_ANNOUNCE(Variable-reference in array test); GRP_PUSH;
  {
    using namespace literals;

    LOG("(bug in [r]): Variable array = arr{ true, false, i8(-8), 8_ui8, i16(-16), 16_ui16, i32(-32), 32_ui32, .32_f32, i64(-64), 64_ui64, .64_f64};")
    Variable array = arr{ true, false, i8(-8), 8_ui8, i16(-16), 16_ui16, i32(-32), 32_ui32, .32_f32, i64(-64), 64_ui64, .64_f64};

    LOG("(bug in [r]): array.toArray() += arr{false, true, array.getReference()};")
    array.toArray() += arr{false, true, array.getReference()};

    PRINT_RUN(utils::printVariable(array));

    LOG("FIXME:")
    LOG("If you don't delete the variable-reference to the parent element,")
    LOG("then the memory from under the resource will not be freed!")
    PRINT_RUN(array.toArray().popBack());
  }
  GRP_POP;
}

#endif // VARIABLE_TEST_HXX