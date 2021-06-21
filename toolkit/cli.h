#ifndef CLI_H
#define CLI_H

#include "../binom/includes/binom.h"

namespace binom {


class CLI {

  static const binom::Object arg_tmpl;

  binom::Object args;
  static binom::Object paresArgs(int argc, char* argv[]);

  void formatOutput(BufferArray format);
  [[noreturn]] void printHelp();
  [[noreturn]] void printLicense();

public:
  CLI(int argc, char* argv[]);
};

// Type input operators

std::istream& operator>>(std::istream& is, VarType& type);
std::istream& operator>>(std::istream& is, VarTypeClass& type_class);
std::istream& operator>>(std::istream& is, ValType& val_type);

}


#endif // CLI_H
