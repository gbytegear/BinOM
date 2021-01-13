#ifndef FILE_IO_H
#define FILE_IO_H

#include "../types.h"
#include <cstdio>
#include <string>

#ifdef __linux__

#include <filesystem>

#elif _WIN32

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#endif

namespace binom {

class FileIO {
  std::FILE* file = nullptr;
public:
  FileIO() = default;
  FileIO(const char* filename) : file(fs::exists(filename)? fopen64(filename, "r+") : fopen64(filename, "w+")) {}
  FileIO(std::string filename) : FileIO(filename.c_str()) {}

  void close() {if(file) fclose(file);}

  bool open(const char* filename) {
    close();
    file = fs::exists (filename)? fopen64(filename, "r+") : fopen64(filename, "w+");
    return !!file;
  }

  inline bool open(std::string filename) {return open (filename.c_str());}

  ui64 pos() {return ftello64(file);}
  bool seek(ui64 pos) {return !fseeko64(file, pos, SEEK_SET);}
  bool rseek(ui64 rpos) {return !fseeko64(file, rpos, SEEK_END);}

};

}

#endif
