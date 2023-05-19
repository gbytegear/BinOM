#ifndef MULTI_MAP_TEST_HXX
#define MULTI_MAP_TEST_HXX

#include "tester.hxx"

#include "libbinom/include/variables/multi_map.hxx"
#include "print_variable.hxx"

template <class IteratorType>
struct IteratorRange : public std::pair<IteratorType, IteratorType> {
  IteratorType begin() {return self.first;}
  IteratorType end() {return self.second;}
};

void testMultiMap() {
  using namespace binom::literals;
  using namespace binom;

  RAIIPerfomanceTest test_perf("MultiMap test: ");
  SEPARATOR
  TEST_ANNOUNCE(MultiMap test)
  GRP_PUSH

  LOG("(bug in [r]): binom::Map _map = map{{2, 1},{2, 2},{2, 3},{1, 1},{1, 2},{1, 3}, {\"Hello\", \"World\"}, {\"Hello\", \"Someone\"}};");
  binom::MultiMap _map = multimap{{2, 1},{2, 2},{2, 3},{1, 1},{1, 2},{1, 3}, {"Hello", "World"}, {"Hello", "Someone"}};
  utils::printVariable(_map);
  SEPARATOR

  utils::printVariable(_map.move());

  PRINT_RUN(_map.insert(0, 1);)
  PRINT_RUN(_map.insert(0, 2);)
  PRINT_RUN(_map.insert(0, 3);)

  utils::printVariable(_map.move());

  IteratorRange<MultiMap::Iterator> range{_map.getRange(2)};

  PRINT_RUN(_map.removeAll(1);)

  utils::printVariable(_map.move());

  LOG("2 Key values iterating:");
  for(auto named_variable : IteratorRange<MultiMap::Iterator>{_map.getRange(2)}) {
    LOG("Key:" << i32(named_variable.getKey().toNumber()) << "; Value:" << i32(named_variable.getValue().toNumber()));
  }

  LOG("0 Key values iterating:");
  for(auto named_variable : IteratorRange<MultiMap::Iterator>{_map.getRange(0)}) {
    LOG("Key:" << i32(named_variable.getKey().toNumber()) << "; Value:" << i32(named_variable.getValue().toNumber()));
  }

  LOG("1 Key values iterating:");
  for(auto named_variable : IteratorRange<MultiMap::Iterator>{_map.getRange(1)}) {
    LOG("Key:" << i32(named_variable.getKey().toNumber()) << "; Value:" << i32(named_variable.getValue().toNumber()));
  }

  TEST(_map[1] == _map.cend())

  PRINT_RUN(_map.remove(++_map[2]);)

  utils::printVariable(_map.move());

  PRINT_RUN(_map.clear();)

  utils::printVariable(_map.move());

  GRP_POP
}

#endif // MULTI_MAP_TEST_HXX
