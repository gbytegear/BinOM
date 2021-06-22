#include "binom/includes/binom.h"

using namespace binom;

void BinOMFile::writeNode(ui64& file_carret, DBNodeVisitor& node) {

  ByteArray data;

  switch (node.getTypeClass()) {
    case binom::VarTypeClass::primitive:
      data = node.getVariable().serialize();
      file.write(file_carret, data.begin(), data.length());
      file_carret += data.length();
    return;
    case binom::VarTypeClass::buffer_array:
      data = node.getVariable().serialize();
      file.write(file_carret, data.begin(), data.length());
      file_carret += data.length();
    break;
    case binom::VarTypeClass::array:
      data += byte(VarType::array);
      file.write(file_carret, data.begin(), data.length());
      file_carret += data.length();
      for(const DBNodeVisitor& child : node)
        writeNode(file_carret, const_cast<DBNodeVisitor&>(child));
      data.get<VarType>(0,0) = VarType::end;
      file.write(file_carret, data.begin(), data.length());
      file_carret += data.length();
    return;
    case binom::VarTypeClass::object:
      data += byte(VarType::object);
      file.write(file_carret, data.begin(), data.length());
      file_carret += data.length();

      for(DBNodeIterator iterator = node.begin(); !iterator.isEnd() ; ++iterator) {
        ByteArray name(iterator.getName());
        name += byte(0);
        file.write(file_carret, name.begin(), name.length());
        file_carret += name.length();
        DBNodeVisitor child = *iterator;
        writeNode(file_carret, child);
      }

      data.get<VarType>(0,0) = VarType::end;
      file.write(file_carret, data.begin(), data.length());
      file_carret += data.length();
    break;
    case binom::VarTypeClass::invalid_type:
    return;

  }


}

BinOMFile::BinOMFile(std::string filename)
  : file(filename)
{}

BinOMFile::BinOMFile(const BinOMFile& other)
  : file(other.file) {}

BinOMFile::BinOMFile(BinOMFile&& other)
  : file(std::move(other.file)) {}

BinOMFile& BinOMFile::operator=(const BinOMFile& other) {
  file.close();
  return *new(this) BinOMFile(other);
}

BinOMFile& BinOMFile::operator=(BinOMFile&& other) {
  file.close();
  return *new(this) BinOMFile(std::move(other));
}

BinOMFile& BinOMFile::write(Variable data) {
  if(!isOpen()) return *this;
  ByteArray serialized = data.serialize();
  file.write(0, serialized);
  return *this;
}

BinOMFile& BinOMFile::load(Variable& data) {
  if(!isOpen()) return *this;
  data = load();
  return *this;
}

BinOMFile& BinOMFile::write(BinOMDataBase& db) {
  DBNodeVisitor db_root(db.getRoot());
  ui64 file_carret = 0;
  file.resize(0);
  writeNode(file_carret, db_root);
  return *this;
}

Variable BinOMFile::load() {
  if(!isOpen()) return nullptr;
  ByteArray serialized(file.size());
  file.read(0, serialized.begin(), serialized.length());
  return Variable::deserialize(serialized);
}

