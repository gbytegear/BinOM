#ifndef BUGS_HXX
#define BUGS_HXX

#include "libbinom/include/binom.hxx"
#include "tester.hxx"

void testBugConstGenericArithmeticAssignment() {
  TEST_ANNOUNCE(Constant GenericArithmetic Assignment bug#1);
  LOG( RED_TXT "This error is related to the following symbols: binom::GenericValue, binom::GenericValueRef, binom::Number")
  LOG( RED_TXT "Solution not found")
  GRP_PUSH;
    using namespace binom;
    LOG("Context:"); GRP_PUSH;
      PRINT_RUN(GenericValue a = 5);
      PRINT_RUN(const GenericValue b = 15);
      PRINT_RUN(GenericValue c = 20);
      PRINT_RUN(a = c);
    GRP_POP;

    LOG( RED_TXT "Problem:"); GRP_PUSH;
      LOG("a = b;" RED_TXT " error: object of type 'binom::GenericValue' cannot be assigned because its copy assignment operator is implicitly deleted");
    GRP_POP;

    LOG( GREEN_TXT "Workarounds:"); GRP_PUSH;
      LOG( GREEN_TXT "Way 1: Cast a generic value to a value of a specific type:")
      PRINT_RUN(a = int(b));
      LOG("Result:");
      LOG("a = " << int(a));
      LOG( GREEN_TXT "Way 2: Remove type constancy:")
      PRINT_RUN(a = c);
      PRINT_RUN(a = const_cast<GenericValue&>(b));
      LOG("Result:");
      LOG("a = " << int(a));
    GRP_POP;
  GRP_POP;
}

void testAllBugs() {
  SEPARATOR
  TEST_ANNOUNCE(Bug list:);
  LOG("Attention: Known errors in the project and their workarounds are described here");
  GRP_PUSH;
  testBugConstGenericArithmeticAssignment();
  GRP_POP;
}

#endif // BUGS_HXX
