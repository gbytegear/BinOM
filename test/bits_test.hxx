#ifndef BITS_TEST_H
#define BITS_TEST_H

#include "tester.hxx"
#include "libbinom/include/variables/bit_array.hxx"
#include <assert.h>
#include <array>


void printBist(const binom::BitArray& bit_array) {
  LOG("Print bits:");
  size_t counter = 0;
  for(auto bit : bit_array)
    LOG(counter++ << " - " << bit);
}

void testBits() {
  RAIIPerfomanceTest test_perf("Bits test: ");
  SEPARATOR
  using namespace binom;
  using namespace binom::priv;
  TEST_ANNOUNCE(Test Bits & Bits value);
  GRP(
    PRINT_RUN(Bits bits;);
    PRINT_RUN(Bits::ValueRef bit_first = bits[0];);
    PRINT_RUN(Bits::ValueRef bit_last = bits[7];);

    LOG("bit_first: " << bit_first << "; bit_last: " << bit_last);
    LOG("bit_first == bit_last: " << (bit_first == bit_last));
    PRINT_RUN(bit_first = true;);
    LOG("bit_first: " << bit_first << "; bit_last: " << bit_last);
    LOG("bit_first == bit_last: " << (bit_first == bit_last));
    PRINT_RUN(bit_last = bit_first;);
    LOG("bit_first: " << bit_first << "; bit_last: " << bit_last);
    LOG("bit_first == bit_last: " << (bit_first == bit_last));
  );

  TEST_ANNOUNCE(Test BitArray);
  GRP(
    PRINT_RUN(BitArray test);
    printBist(test);
    PRINT_RUN(test.pushBack({1,0,1}));
    printBist(test);
    PRINT_RUN(test.pushFront({1,0,1}));
    printBist(test);
    PRINT_RUN(test.insert(3, {0,1,0}));
    printBist(test);
    PRINT_RUN(test.insert(3, {1,0,0,1,1,0,0,1,1,0,0,1,1,0}));
    printBist(test);
    PRINT_RUN(test.remove(3, 14));
    printBist(test);
    PRINT_RUN(test.remove(3, 3));
    printBist(test);
    PRINT_RUN(test.popFront(3));
    printBist(test);
    PRINT_RUN(test.popBack(3));
    printBist(test);
  );
}

#endif // BITS_TEST_H
