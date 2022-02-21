#define TEST_FULL_INFO
#include "test/all_test.hxx"

int main(int, char*[]) {
  SEPARATOR
  std::cout << "OS: " << OS_TYPE << '\n';
  testTypesConversions();
  testResourceControll();
  testGenericValue();

  std::cout << "+---------------------------------------------------------------------------+\n"
               "|                            Test ended success!                            |\n"
               "+---------------------------------------------------------------------------+\n";
  return 0;
}
