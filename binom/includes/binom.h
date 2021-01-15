#ifndef BINOM_H
#define BINOM_H

#include "binom/includes/structure/variables/node_visitor.h"
#include "structure/file_storage/dbfile.h"

namespace binom {

class BinOM {
public:

  static ui64 saveToFile(const char* filename, Variable var) {
    FILE* file = fs::exists(filename)? fopen64(filename, "r+") : fopen64(filename, "w+");
    if(!file) throw SException(ErrCode::file_open_error);
    ByteArray serialized = var.serialize();
    ui64 count = fwrite (serialized.begin (), 1, serialized.length (), file);
    fclose (file);
    return count;
  }
  static inline ui64 saveToFile(std::string filename, Variable var) {return saveToFile(filename.c_str(), std::move(var));}

  static Variable loadFromFile(const char* filename) {
    FILE* file = fopen64(filename, "r+");
    if(!file) throw SException(ErrCode::file_open_error);
    fseeko64 (file, 0, SEEK_END);
    ui64 size = ftello64(file);
    fseeko64 (file, 0, SEEK_SET);
    ByteArray serialized(size);
    fread(serialized.begin (), 1, serialized.length(), file);
    return Variable::deserialize(serialized);
  }

  static inline Variable loadFromFile(std::string filename) {return loadFromFile(filename.c_str());}

};

}

#endif // BINOM_H
