#define TEST_FULL_INFO
#include "test/all_test.hxx"

#include "libbinom/include/variables/number.hxx"

#include <vector>
#include <initializer_list>
#include <array>

int main() {
  testTypesConversions();
  std::cout.flush();
  testGenericValue();
  std::cout.flush();
  testNumber();
  std::cout.flush();
  testRecursiveSharedMutex();

//  std::clog << "Number a = 12, c = Number::createRefVariable(a);\n";
//  std::clog << "a = " << int(a) << '\n';
//  std::clog << "c = " << int(a) << '\n';
//  c++;
//  std::clog << "c++;\n";
//  std::clog << "a = " << int(a) << '\n';
//  std::clog << "c = " << int(a) << '\n';

//  GenericValue b = a;
//  a += b;
//  a += c;
}
