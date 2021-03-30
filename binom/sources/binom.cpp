#include "binom/includes/binom.h"

using namespace binom;

ui64 saveToFile(std::string filename, Variable var) {
  FILE* file = fs::exists(filename)
               ? fopen64(filename.c_str(), "r+")
               : fopen64(filename.c_str(), "w+");

  if(!file) throw Exception(ErrCode::file_open_error);
  ByteArray serialized = var.serialize();
  ui64 count = fwrite (serialized.begin (), 1, serialized.length (), file);
  fclose (file);
  return count;
}

Variable loadFromFile(std::string filename) {
  FILE* file = fopen64(filename.c_str(), "r+");
  if(!file) throw Exception(ErrCode::file_open_error);
  fseeko64 (file, 0, SEEK_END);
  ui64 size = ftello64(file);
  fseeko64 (file, 0, SEEK_SET);
  ByteArray serialized(size);
  fread(serialized.begin (), 1, serialized.length(), file);
  return Variable::deserialize(serialized);
}
