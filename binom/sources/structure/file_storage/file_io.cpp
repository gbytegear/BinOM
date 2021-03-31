#include "binom/includes/structure/file_storage/file_io.h"

using namespace binom;

ui64 FileIO::pos() {return ftello64(file);}

bool FileIO::seek(ui64 pos) {return !fseeko64(file, pos, SEEK_SET);}

bool FileIO::rseek(ui64 rpos) {return !fseeko64(file, rpos, SEEK_END);}

FileIO::FileIO(const char* filename) : file_path(filename), file(fs::exists(file_path)? fopen64(filename, "r+") : fopen64(filename, "w+")) {
  if(!file) throw Exception(ErrCode::file_open_error);
}

FileIO::FileIO(std::string filename) : FileIO(filename.c_str()) {}

FileIO::FileIO(const FileIO& other)
  : FileIO(other.file_path.string()) {}

FileIO::FileIO(FileIO&& other)
  : file_path(other.file_path), file(other.file) {
  other.file = nullptr;
  other.file_path.clear();
}

FileIO::~FileIO() {close();}

void FileIO::close() {
  if(file) fclose(file);
  file_path.clear();
}

bool FileIO::open(const char* filename) {
  close();
  file = fs::exists(filename)? fopen64(filename, "rb+") : fopen64(filename, "wb+");
  file_path = filename;
  return !!file;
}

bool FileIO::open(std::string filename) {return open(filename.c_str());}

ui64 FileIO::size() {fseeko64(file, 0, SEEK_END); return ftello64(file);}

bool FileIO::isExist() {return fs::exists(file_path);}

std::filesystem::__cxx11::path FileIO::path() const {return file_path;}

ui64 FileIO::resize(ui64 new_size) {
  fs::resize_file(file_path, new_size);
  return size();
}

ui64 FileIO::addSize(ui64 add) {ui64 pos = size();resize(pos + add);return pos;}

ui64 FileIO::subSize(ui64 sub) {return resize(size() - sub);}

bool FileIO::isEmpty() {return size() == 0;}

bool FileIO::write(ui64 index, const void* buffer, ui64 size) {
  seek(index);
  return fwrite(buffer, size, 1, file) == size;
}

bool FileIO::write(ui64 index, ByteArray& byte_array) {return write(index, byte_array.begin(), byte_array.length());}

ByteArray::iterator FileIO::write(ui64 index, ByteArray& byte_array, ui64 size) {
  return write(index, byte_array.begin(), size);
}

ByteArray::iterator FileIO::write(ui64 index, ByteArray::iterator it, ui64 size) {
  write(index, (void*)it, size);
  it += size;
  return it;
}

bool FileIO::read(ui64 index, void* buffer, ui64 size) {
  seek(index);
  size_t r_size = fread (buffer, 1, size, file);
  return r_size == size;
}

bool FileIO::read(ui64 index, ByteArray& byte_array) {return read(index, byte_array.begin(), byte_array.length());}

ByteArray::iterator FileIO::read(ui64 index, ByteArray& byte_array, ui64 size) {
  return read(index, byte_array.begin(), size);
}

ByteArray::iterator FileIO::read(ui64 index, ByteArray::iterator it, ui64 size) {
  read(index, (void*)it, size);
  it += size;
  return it;
}
