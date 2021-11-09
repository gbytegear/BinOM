#include "libbinom/include/binom.h"

using namespace binom;

Variable SerializedStorage::read() {
  ByteArray data = file.read(sizeof(Header), file.getSize() - sizeof(Header));
  return Variable::deserialize(data);
}

SerializedStorage& SerializedStorage::write(Variable var) {
  ByteArray data = var.serialize();
  Header header;
  data.pushFront(header);
  file.resize(data.length());
  file.write(0, data);
  return *this;
}

FileType binom::checkFileType(std::string_view file_name) {
  FileIO file(file_name);
  DBVersion version;
  file.read(version, 0);
  if(file.getSize() < sizeof (version))
    return FileType::undefined_file;
  elif(!memcmp(version.file_type, "BinOM.FS", sizeof(current.file_type)))
      return FileType::dynamic_storage;
  elif(!memcmp(version.file_type, "BinOM.SS", sizeof(current.file_type)))
      return FileType::serialized_storage;
  else return FileType::undefined_file;
}
