//#define TEST_FULL_INFO
#include "test/all_test.hxx"
#include "libbinom/include/variables/buffer_array.hxx"

int main() {
  testTypesConversions();
  testGenericValue();
  testNumber();
  testBits();
  testRecursiveSharedMutex();

  Variable a = 256_ui64;
  // NOTE: Number b = a.getReference();
  // getReference() -> rvalue-ref
  // operator T&() -> lvalue-ref
  // Number(const Number&&) *MAKES COPY*
  Number b = a.toNumber().getReference();
  Variable c = a.getReference();

  std::clog << ui64(a.toNumber()) << '\n';
  std::clog << ui64(b) << '\n';

  b = 512;

  std::clog << ui64(a.toNumber()) << '\n';
  std::clog << ui64(b) << '\n';


}
