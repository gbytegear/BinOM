#ifndef MULTI_MAP_TEST_HXX
#define MULTI_MAP_TEST_HXX

#include "tester.hxx"

#include "libbinom/include/variables/multi_map.hxx"
#include "print_variable.hxx"

void testMultiMap() {
  using namespace binom::literals;
  using namespace binom;

  RAIIPerfomanceTest test_perf("MultiMap test: ");
  SEPARATOR
  TEST_ANNOUNCE(MultiMap test)
  GRP_PUSH

  LOG("(bug in [r]): binom::Map _map = map{{1, 2},{3, 4}};");
  binom::MultiMap _map = multimap{{2, 1},{2, 2},{2, 3},{1, 1},{1, 2},{1, 3}};

  PRINT_RUN(_map.insert(0, 1);)
  PRINT_RUN(_map.insert(0, 2);)
  PRINT_RUN(_map.insert(0, 3);)

  utils::printVariable(_map.move());

  PRINT_RUN(_map.removeAll(1);)

  utils::printVariable(_map.move());

  LOG("Key values iterating:");
  for(auto named_variable : _map.find(2)) {
    // BUG: After iterating over all elements of the key, a signal SIGSEGV(Invalid access to storage) is recived
    LOG("Key:");
    utils::printVariable(named_variable.getKey());
    LOG("Value:")
    utils::printVariable(named_variable.getVariable());
  }

  PRINT_RUN(_map.remove(++_map.find(2));)

  utils::printVariable(_map.move());

  PRINT_RUN(_map.clear();)

  utils::printVariable(_map.move());

  GRP_POP
}

#endif // MULTI_MAP_TEST_HXX
