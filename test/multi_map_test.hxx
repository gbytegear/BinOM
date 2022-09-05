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

  utils::printVariable(_map.move());

  PRINT_RUN(_map.insert(0, 1);)
  PRINT_RUN(_map.insert(0, 2);)
  PRINT_RUN(_map.insert(0, 3);)

  utils::printVariable(_map.move());

  PRINT_RUN(_map.removeAll(1);)

  utils::printVariable(_map.move());

  LOG("2 Key values iterating:");
  for(auto named_variable : _map[2]) {
    LOG("Key:" << i32(named_variable.getKey().toNumber()) << "; Value:" << i32(named_variable.getVariable().toNumber()));
  }

  LOG("0 Key values iterating:");
  for(auto named_variable : _map[0]) {
    LOG("Key:" << i32(named_variable.getKey().toNumber()) << "; Value:" << i32(named_variable.getVariable().toNumber()));
  }

  LOG("1 Key values iterating:");
  for(auto named_variable : _map[1]) {
    LOG("Key:" << i32(named_variable.getKey().toNumber()) << "; Value:" << i32(named_variable.getVariable().toNumber()));
  }

  TEST(_map[1] == _map.cend())

  PRINT_RUN(_map.remove(++_map[2]);)

  utils::printVariable(_map.move());

  PRINT_RUN(_map.clear();)

  utils::printVariable(_map.move());

  GRP_POP
}

#endif // MULTI_MAP_TEST_HXX
