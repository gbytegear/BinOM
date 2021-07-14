#ifndef FILE_H
#define FILE_H

#if __has_include(<filesystem>)
  #include <filesystem>
  namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
  #include <experimental/filesystem>
  namespace fs = std::experimental::filesystem;
#endif

#include <fstream>
#include <shared_mutex>

#include "byte_array.h"

namespace binom {

class FileIO {
public:
  typedef std::basic_ifstream<ui8> Reader;
  typedef std::basic_ofstream<ui8> Writer;
private:
  fs::path file_path;
  std::shared_mutex mtx;
  inline Reader getReader() {return Reader(file_path.string(), std::ios::binary);}
  inline Writer getWriter() {return Writer(file_path.string(), std::ios::binary);}
public:
  static inline bool isExist(std::string_view file_path) {return fs::exists(file_path);}
  static inline bool isRegularFile(std::string_view file_path) {return fs::is_regular_file(file_path);}
  FileIO(std::string_view str) : file_path(str) {}

  bool readBuffer(void* buffer, ui64 pos, ui64 size) {
    Reader reader = getReader();
    if(!reader.is_open()) return false;
    mtx.lock_shared();
    reader.seekg(pos, std::ios::beg);
    reader.read(static_cast<ui8*>(buffer), size);
    mtx.unlock_shared();
    return true;
  }

  template<typename T>
  bool read(T& to, ui64 from) { return readBuffer(&to, from, sizeof(to)); }

  bool read(ByteArray& to, ui64 from) { return readBuffer(to.begin(), from, to.length()); }

  ByteArray read(ui64 from, ui64 size) {
    ByteArray data(size);
    readBuffer(data.begin(), from, size);
    return data;
  }

  bool writeBuffer(void* buffer, ui64 pos, ui64 size) {
    Writer writer = getWriter();
    if(!writer.is_open()) return false;
    mtx.lock();
    writer.seekp(pos, std::ios::beg);
    writer.write(static_cast<ui8*>(buffer), size);
    mtx.unlock();
  }

  template<typename T>
  bool write(T& from, ui64 to) { return writeBuffer(&from, to, sizeof(from)); }

  bool write(const ByteArray from, ui64 to) { return writeBuffer(from.begin(), to, from.length()); }

};


}

#endif // FILE_H
