#ifndef FILE_IO_H
#define FILE_IO_H

#include "../types.h"
#include <cstdio>
#include <string>

#ifdef __linux__

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#elif _WIN32

#include <filesystem>

namespace fs = std::filesystem;

#endif

namespace binom {

class FileIO {
  fs::path file_path;
  std::FILE* file = nullptr;

  ui64 pos() {return ftello64(file);}
  bool seek(ui64 pos) {return !fseeko64(file, pos, SEEK_SET);}
  bool rseek(ui64 rpos) {return !fseeko64(file, rpos, SEEK_END);}

public:
  FileIO() = default;
  FileIO(const char* filename) : file_path(filename), file(fs::exists(file_path)? fopen64(filename, "r+") : fopen64(filename, "w+")) {}
  FileIO(std::string filename) : FileIO(filename.c_str()) {}

  void close() {
    if(file) fclose(file);
    file_path.clear();
  }

  bool open(const char* filename) {
    close();
    file = fs::exists(filename)? fopen64(filename, "r+") : fopen64(filename, "w+");
    file_path = filename;
    return !!file;
  }

  inline bool open(std::string filename) {return open(filename.c_str());}

  ui64 size() {fseeko64(file, 0, SEEK_END); return ftello64(file);}
  bool isExist() {return fs::exists(file_path);}
  fs::path path() const {return file_path;}

  ui64 resize(ui64 new_size) {
    fs::resize_file(file_path, new_size);
    return size();
  }

  ui64 addSize(ui64 add) {ui64 pos = size();resize(pos + add);return pos;}
  ui64 sebSize(ui64 sub) {return resize(size() - sub);}


  bool isEmpty() {return size() == 0;}

  bool write(ui64 index, const void* buffer, ui64 size) {
    seek(index);
    return fwrite(buffer, size, 1, file) == size;
  }

  template<typename Type>
  bool write(ui64 index, const Type& data) {return write (index, &data, sizeof(data));}

  bool write(ui64 index, ByteArray& byte_array) {return write(index, byte_array.begin(), byte_array.length());}

  ByteArray::iterator write(ui64 index, ByteArray& byte_array, ui64 size) {
    return write(index, byte_array.begin(), size);
  }

  ByteArray::iterator write(ui64 index, ByteArray::iterator it, ui64 size) {
    write(index, (void*)it, size);
    it += size;
    return it;
  }

  bool read(ui64 index, void* buffer, ui64 size) {
    seek(index);
    return fread (buffer, size, 1, file) == size;
  }

  template<typename Type>
  bool read(ui64 index, Type& data) {return read(index, &data, sizeof(data));}

  bool read(ui64 index, ByteArray& byte_array) {return read(index, byte_array.begin(), byte_array.length());}

  ByteArray::iterator read(ui64 index, ByteArray& byte_array, ui64 size) {
    return read(index, byte_array.begin(), size);
  }

  ByteArray::iterator read(ui64 index, ByteArray::iterator it, ui64 size) {
    read(index, (void*)it, size);
    it += size;
    return it;
  }

};

}

#endif
