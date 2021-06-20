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
bool isUInt(const char* str);
bool isInt(const char* str);
binom::ui64 inputUInt(const char* msg, binom::ui64 left_limit = 0, binom::ui64 right_limit = 0xffffffffffffffff);
binom::i64 inputInt(const char* msg, binom::i64 left_limit = 0x8000000000000000, binom::i64 right_limit = 0x7fffffffffffffff);
std::string inputString(const char* msg, std::function<bool (std::string&)> condition);
binom::Variable parseArg(int argc, char* argv[], const char* default_arg = "default");
void openFile(std::string file_name,
              std::function<void(binom::BinOMDataBase)> db_callback,
              std::function<void(binom::BinOMFile)> file_callback,
              std::function<void(binom::Exception&)> err_callback
              = [](binom::Exception& except){throw except;});

FileType toFileType(std::string str);
binom::VarType toType(std::string str);

binom::Primitive inputPrimitive(binom::ValType type);
binom::BufferArray inputBufferArray(binom::ValType type = binom::ValType::invalid_type);
binom::Variable inputVariable(bool optional = false, binom::ValType constraint = binom::ValType::invalid_type);

void editContainer(binom::UnionNodeVisitor root);

#endif // UTILS_H
