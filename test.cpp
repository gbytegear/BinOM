#include "binom/includes/structure/variables/variable.h"

int main() {
  using namespace binom;
  try {
    Variable barr = L"А";
    std::cout << barr.toBufferArray();
  } catch(binom::SException except) {
    std::cout << binom::SException::ectos(except.code()) << except.what();
  }

  return 0;
}
