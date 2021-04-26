#ifndef BINOM_H
#define BINOM_H

#include "structure/variables/node_visitor.h"
#include "structure/file_storage/data_base.h"

namespace binom {

class BinOMFile {
  FileIO file;
public:
  BinOMFile(std::string filename);
  BinOMFile(const BinOMFile& other);
  BinOMFile(BinOMFile&& other);

  inline ui64 size() {return file.size();}
  inline bool isExist() {return file.isExist();}
  inline fs::path path() const {return file.path();}
  inline bool isEmpty() {return file.isEmpty();}

  BinOMFile& write(Variable data);
  BinOMFile& load(Variable& data);
  BinOMFile& backup(BinOMDataBase& db);

  Variable load();

};

}

#endif // BINOM_H
