#ifndef CLI_H
#define CLI_H

#include "../binom/includes/binom.h"
#include "utils.h"
#include <map>
#include <functional>

namespace binom {

typedef std::function<void()> command_t;

class CLI {
  typedef std::map<BufferArray, command_t> command_map_t;

  static const binom::Object arg_tmpl;
  static const std::map<BufferArray, command_t> commands;

  binom::Object args;

  static const Object& getArgs();
  static binom::Object paresArgs(int argc, char* argv[]);
  static void requredArg(BufferArray name,
                         VarType requred_type = VarType::invalid_type,
                         const char* exist_err = "Argumment isn't exist",
                         const char* type_err = "Invalid argumment type");

  void formatOutput(BufferArray format);
  [[noreturn]] void printHelp();
  [[noreturn]] void printLicense();
  void exec();

  friend command_t;
  friend command_map_t;

public:
  [[noreturn]]CLI(int argc, char* argv[]);
};

// Type input operators

std::istream& operator>>(std::istream& is, VarType& type);
std::istream& operator>>(std::istream& is, VarTypeClass& type_class);
std::istream& operator>>(std::istream& is, ValType& val_type);

}


#endif // CLI_H
