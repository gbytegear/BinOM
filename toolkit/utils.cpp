#include "utils.h"

using namespace binom;

NumberType getNumberType(const char* str) {
  bool is_negative = false;
  bool has_point = false;
  for(const char* it = str; *it; ++it) {
    if(*it >= '0' && *it <= '9') continue;
    elif(!has_point && *it == '.') has_point = true;
    elif(it == str && *it == '-') is_negative = true;
    else return NumberType::not_a_number;
  }
  return has_point? NumberType::float_number
                  :is_negative? NumberType::signed_integer_number
                              : NumberType::unsigned_integer_number;
}

Variable parseArg(int argc, char* argv[], const char* default_arg) {
  --argc; ++argv; // Skip program name
  Object arguments = vobj{
  {default_arg, varr{}}
};

  Array* current_arg = nullptr;
  for(int i = 0; i < argc; ++i) {
    if(argv[i][0] == '-') {
      current_arg = &arguments.insert(++argv[i], varr{}).toArray();
    } elif(current_arg) {
      current_arg->pushBack(argv[i]);
    } else {
      arguments[default_arg].toArray().pushBack(argv[i]);
    }
  }

  return arguments.asVar();
}
