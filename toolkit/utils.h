#ifndef UTILS_H
#define UTILS_H

#include "../binom/includes/binom.h"

enum class NumberType : binom::ui8 {
  not_a_number,
  float_number,
  unsigned_integer_number,
  signed_integer_number
};

NumberType getNumberType(const char* str);
binom::Variable parseArg(int argc, char* argv[], const char* default_arg = "default");

#endif // UTILS_H
