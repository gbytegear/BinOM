#include "binom/includes/structure/file_storage/file_io.h"

using namespace binom;

ui64 FileIO::pos() {return ftello64(file);}

bool FileIO::seek(ui64 pos) {return !fseeko64(file, pos, SEEK_SET);}

bool FileIO::rseek(ui64 rpos) {return !fseeko64(file, rpos, SEEK_END);}

FileIO::FileIO(std::string filename) : file_path(filename), file(fs::exists(file_path)? fopen64(filename.c_str(), "rb+") : fopen64(filename.c_str(), "wb+")) {
  if(!file) throw Exception(ErrCode::file_open_error);
}

FileIO::FileIO(const FileIO& other)
  : FileIO(other.file_path.string()) {}

FileIO::FileIO(FileIO&& other)
  : file_path(other.file_path), file(other.file) {
  other.file = nullptr;
  other.file_path.clear();
}

FileIO::~FileIO() {close();}

void FileIO::close() {
  if(file) {
    fclose(file);
    file = nullptr;
  }
  file_path.clear();
}

bool FileIO::open(std::string filename) {
  close();
  file = fs::exists(filename)? fopen64(filename.c_str(), "rb+") : fopen64(filename.c_str(), "wb+");
  file_path = filename;
  return !!file;
}

ui64 FileIO::size() {fseeko64(file, 0, SEEK_END); return ftello64(file);}

bool FileIO::isExist(std::string file_path) {return fs::exists(file_path);}
bool FileIO::isExist() {return fs::exists(file_path);}
bool FileIO::isOpen() {return !!file;}

ui64 FileIO::resize(ui64 new_size) {
  fs::resize_file(file_path, new_size);
  return size();
}

fs::path FileIO::path() const {return file_path;}

ui64 FileIO::addSize(ui64 add) {ui64 pos = size();resize(pos + add);return pos;}

ui64 FileIO::subSize(ui64 sub) {return resize(size() - sub);}

bool FileIO::isEmpty() {return isOpen()? size() == 0 : true;}

bool FileIO::write(ui64 index, const void* buffer, ui64 size) {
  bool answ;
  mtx.lock();
  seek(index);
  answ = fwrite(buffer, size, 1, file) == size;
  mtx.unlock();
  return answ;
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
  mtx.lock();
  seek(index);
  size_t r_size = fread (buffer, 1, size, file);
  mtx.unlock();
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
