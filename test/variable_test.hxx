#ifndef VARIABLE_TEST_HXX
#define VARIABLE_TEST_HXX

#include "tester.hxx"
#include "libbinom/include/binom_impl/print_variable.hxx"

using namespace binom;

void testVariable() {
  RAIIPerfomanceTest test_perf("Variable test: ");
  SEPARATOR
  using namespace literals;
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
    LOG("(bug in [r]): Variable array = arr{ true, false, i8(-8), 8_ui8, i16(-16), 16_ui16, i32(-32), 32_ui32, .32_f32, i64(-64), 64_ui64, .64_f64};")
    Variable array = arr{ true, false, i8(-8), 8_ui8, i16(-16), 16_ui16, i32(-32), 32_ui32, .32_f32, i64(-64), 64_ui64, .64_f64};

    LOG("(bug in [r]): array.toArray() += arr{false, true, array.getReference()};")
    array.toArray() += arr{false, true, array.getReference()};

#ifdef FULL_TEST
    PRINT_RUN(utils::printVariable(array));
#endif

    LOG("FIXME:")
    LOG("If you don't delete the variable-reference to the parent element,")
    LOG("then the memory from under the resource will not be freed!")
    PRINT_RUN(array.toArray().popBack());
  }
  GRP_POP;

  TEST_ANNOUNCE(SinglyLinkedList test); GRP_PUSH;
  {
    LOG("(bug in [r]): Variable sl_list_var = sllist{true, false, i8(-8), 8_ui8, i16(-16), 16_ui16, i32(-32), 32_ui32, .32_f32, i64(-64), 64_ui64, .64_f64};")
    Variable sl_list_var = sllist{true, false, i8(-8), 8_ui8, i16(-16), 16_ui16, i32(-32), 32_ui32, .32_f32, i64(-64), 64_ui64, .64_f64};
    PRINT_RUN(SinglyLinkedList sl_list = sl_list_var.toSinglyLinkedList().getReference());
    PRINT_RUN(utils::printVariable(sl_list));
  }

  TEST_ANNOUNCE(DoublyLinkedList test); GRP_PUSH;
  {
    LOG("(bug in [r]): Variable dl_list_var = dllist{true, false, i8(-8), 8_ui8, i16(-16), 16_ui16, i32(-32), 32_ui32, .32_f32, i64(-64), 64_ui64, .64_f64};")
    Variable dl_list_var = dllist{true, false, i8(-8), 8_ui8, i16(-16), 16_ui16, i32(-32), 32_ui32, .32_f32, i64(-64), 64_ui64, .64_f64};
    PRINT_RUN(DoublyLinkedList dl_list = dl_list_var.toDoublyLinkedList().getReference());
    PRINT_RUN(utils::printVariable(dl_list));


    for(const auto& element : dl_list) {
      utils::printVariable(element);
    }

    for(const auto& element : reverse_iterator::ReverseRange(dl_list)) {
      utils::printVariable(element);
    }


  }
}

#endif // VARIABLE_TEST_HXX
