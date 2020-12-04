#include "binom/includes/structure/variables/variable.h"

int main() {
  using namespace binom;
  try {

    Variable arr = ui16arr{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

    arr.toBufferArray().pushBack(arr.toBufferArray());

    std::cout << arr.toBufferArray() << '\n';

  } catch(binom::SException except) {
    std::cout << binom::SException::ectos(except.code()) << except.what();
  }

  return 0;
}
