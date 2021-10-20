#ifndef BINOM_H
#define BINOM_H

#include "utils/node_visitor.h"
#include "file_storage/file_storage.h"
#include "utils/file.h"

namespace binom {

enum class FileType {
  undefined_file = 0,
  dynamic_storage = 1,
  serialized_storage = 2,
};

FileType checkFileType(std::string_view file_name);

class SerializedStorage {
  FileIO file;

#pragma pack(push, 1)
  struct Header {
    enum class VersionDifference {
      identical = 0,
      file_type = 1,
      major = 2,
      minor = 3,
    };

    static constexpr DBVersion current{"BinOM.SS", 0, 1}; // Serialized_Storage
    DBVersion version = current;

    VersionDifference checkFileVersion() {
      if(memcmp(version.file_type, current.file_type, sizeof(current.file_type)))
        return VersionDifference::file_type;
      if(version.major != current.major)
        return VersionDifference::major;
      if(version.minor != current.minor)
        return VersionDifference::minor;
      return VersionDifference::identical;
    }
  };
#pragma pack(pop)

  Header file_header;

public:
  SerializedStorage(std::string_view file_name)
    : file(file_name) {
    if(isEmpty()) {
      file.write(0, file_header);
    } else {
      file.read(file_header, 0);
      switch (file_header.checkFileVersion()) {
        case Header::VersionDifference::identical: break;
        case Header::VersionDifference::file_type:
          throw Exception(ErrCode::binomdb_invalid_file, "Invalid file type");
        case Header::VersionDifference::major:
          IF_DEBUG(std::cerr << "Warning: major version difference!\n");
        break;
        case Header::VersionDifference::minor:
          IF_DEBUG(std::cerr << "Warning: minor version difference!\n");
        break;
      }
    }
  }

  inline bool isExist() const {return file.isExist();}
  inline bool isEmpty() const {return file.isEmpty();}
  inline fs::path getPath() const {return file.getPath();}
  inline ui64 getSize() const {return file.getSize();}

  Variable read();
  SerializedStorage& write(Variable var);

  inline SerializedStorage& operator=(Variable var) {return write(std::move(var));}
  inline operator Variable() {return read();}
};

}

#endif // BINOM_H
