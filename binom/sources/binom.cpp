#include "binom/includes/binom.h"

using namespace binom;

BinOMFile::BinOMFile(std::string filename)
  : file(filename)
{}

BinOMFile::BinOMFile(const BinOMFile& other)
  : file(other.file) {}

BinOMFile::BinOMFile(BinOMFile&& other)
  : file(std::move(other.file)) {}

BinOMFile& BinOMFile::write(Variable data) {
  ByteArray serialized = data.serialize();
  file.write(0, serialized);
  return *this;
}

BinOMFile& BinOMFile::load(Variable& data) {
  data = load();
  return *this;
}

BinOMFile& BinOMFile::backup(BinOMDataBase& db) {
  // TODO this
  return *this;
}

Variable BinOMFile::load() {
  ByteArray serialized(file.size());
  file.read(0, serialized.begin(), serialized.length());
  return Variable::deserialize(serialized);
}

