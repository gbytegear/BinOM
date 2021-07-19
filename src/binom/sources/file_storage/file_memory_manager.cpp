#include "binom/includes/file_storage/file_memory_manager.h"

using namespace binom;

FMemoryManager::FMemoryManager(std::string_view file_path) : file(file_path) {
  init();
}

void FMemoryManager::init() {
  if(file.isEmpty()) {
    file.resize(sizeof(db_header));
    file.write(db_header, 0);
    return;
  }
  file.read(db_header, 0);

  switch (db_header.checkFileVersion()) {
    case binom::DBHeader::VersionDifference::identical: break;
    case binom::DBHeader::VersionDifference::file_type:
      throw Exception(ErrCode::binomdb_invalid_file, "Invalid file type");
    case binom::DBHeader::VersionDifference::major:
      std::cerr << "Warning: major version difference!\n";
    break;
    case binom::DBHeader::VersionDifference::minor:
      std::cerr << "Warning: minor version difference!\n";
    break;
  }

  // TODO: ...

}
