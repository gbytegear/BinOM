#ifndef UTILS_H
#define UTILS_H

#include "../binom/includes/binom.h"

enum class NumberType : binom::ui8 {
  not_a_number,
  float_number,
  unsigned_integer_number,
  signed_integer_number
};

enum class FileType : binom::ui8 {
  data_base,
  serialized_data,
  invalid
};

NumberType getNumberType(const char* str);
binom::Variable parseArg(int argc, char* argv[], const char* default_arg = "default");
void openFile(std::string file_name,
              std::function<void(binom::BinOMDataBase)> db_callback,
              std::function<void(binom::BinOMFile)> file_callback,
              std::function<void(binom::Exception&)> err_callback
              = [](binom::Exception& except){throw except;});

FileType toFileType(std::string str);
binom::VarType toType(std::string str);

binom::Variable inputVariable(bool optional = false, binom::ValType constraint = binom::ValType::invalid_type);

#endif // UTILS_H
