#ifndef TEST_AREA_HXX
#define TEST_AREA_HXX

#include "libbinom/include/binom.hxx"
#include "libbinom/include/variables/stdoutput.hxx"

//struct ComparatorTest {
//  using is_transparent = true;

//};

void runInDevTests() {
  using namespace binom;
  using namespace binom::priv;
  using namespace binom::index;
  using namespace binom::literals;

  Variable var = map{
    {"first", 8_ui8},
    {"second", bitarr{true, false, true, false}},
    {"third", i16arr{1,2,3,4,5,6,7,8}},
    {"forth", arr{1_ui8, 2_i16, 3_i32, 4_ui64}},
    {"fivth", list{1_ui8, 2_i16, 3_i32, 4_ui64, arr{1_ui8, 2_i16, 3_i32, 4_ui64}}},
    {"sixth", map{{1_ui8, 2_i16}, {3_i32, 4_ui64}}},
    {"seventh", multimap{{1_ui8, 2_i16}, {1_ui8, 3_i32}, {2_i16, 1_ui8}, {2_i16, 4_ui64}}}
  };

  std::cout << var << '\n';
}

#endif // TEST_AREA_HXX
