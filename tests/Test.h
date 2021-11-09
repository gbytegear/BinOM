#ifndef TEST_H
#define TEST_H

#include <functional>

class Test {
  bool is_test_passed = true;
  std::string error_string;
public:
  Test(std::function<bool()> test, std::string error_str) {
    try {
      is_test_passed = test();
      if(!is_test_passed) error_string = error_str;
    } catch (std::exception& exception) {
      is_test_passed = false;
      error_string = exception.what();
    } catch(...) {
      is_test_passed = false;
      error_string = "Unknown exception!";
    }
  }

  Test(const Test& other) : is_test_passed(other.is_test_passed), error_string(other.error_string) {}
  Test(Test&& other) : is_test_passed(other.is_test_passed), error_string(std::move(other.error_string)) {}

  operator const char* () {return error_string.c_str();}
  operator bool () {return is_test_passed;}
};


#define TEST(test) Test()

#endif // TEST_H
