#ifndef VARIABLE_TEST_HXX
#define VARIABLE_TEST_HXX

#include "tester.hxx"
#include "print_variable.hxx"

using namespace binom;

void testVariable() {
  RAIIPerfomanceTest test_perf("Variable test: ");
  SEPARATOR
  using namespace literals;
  TEST_ANNOUNCE(Variable-reference test); GRP_PUSH;
  {
    PRINT_RUN(Variable a = 256_ui64);

    LOG("NOTE: Number b = a.move()");
    LOG("move() -> rvalue-ref");
    LOG("operator T&() -> lvalue-ref");
    LOG("Number(const Number&) *MAKES COPY*")
    PRINT_RUN(Number b = a.toNumber().move());
    PRINT_RUN(Variable c = a.move());
    TEST(ui64(a.toNumber()) == 256);
    TEST(ui64(b) == 256);
    PRINT_RUN(b = 512;)
    TEST(ui64(a.toNumber()) == 512);
    TEST(ui64(b) == 512);
  } GRP_POP;

  TEST_ANNOUNCE(Variable-reference in array test); GRP_PUSH;
  {
    LOG("(bug in [r]): Variable array = arr{ true, false, i8(-8), 8_ui8, i16(-16), 16_ui16, i32(-32), 32_ui32, .32_f32, i64(-64), 64_ui64, .64_f64};")
    Variable array = arr{ true, false, i8(-8), 8_ui8, i16(-16), 16_ui16, i32(-32), 32_ui32, .32_f32, i64(-64), 64_ui64, .64_f64};

    LOG("(bug in [r]): array.toArray() += arr{false, true, array.move()};")
    array.toArray() += arr{false, true, array.move()};

#ifdef FULL_TEST
    PRINT_RUN(utils::printVariable(array));
#endif

    LOG("FIXME:")
    LOG("If you don't delete the variable-reference to the parent element,")
    LOG("then the memory from under the resource will not be freed!")
    PRINT_RUN(array.toArray().popBack());
  } GRP_POP;

  TEST_ANNOUNCE(List test); GRP_PUSH;
  {
    LOG("(bug in [r]): Variable list_var = list{true, false, i8(-8), 8_ui8, i16(-16), 16_ui16, i32(-32), 32_ui32, .32_f32, i64(-64), 64_ui64, .64_f64};")
    Variable list_var = list{true, false, i8(-8), 8_ui8, i16(-16), 16_ui16, i32(-32), 32_ui32, .32_f32, i64(-64), 64_ui64, .64_f64};
    PRINT_RUN(List _list = list_var.toList().move());
    PRINT_RUN(utils::printVariable(_list));

    LOG("Iteration in order")
    for(const auto& element : _list) {
      utils::printVariable(element);
    }

    LOG("Reverse iteration")
    for(const auto& element : reverse_iterator::ReverseRange(_list)) {
      utils::printVariable(element);
    }

  } GRP_POP
}

#endif // VARIABLE_TEST_HXX
