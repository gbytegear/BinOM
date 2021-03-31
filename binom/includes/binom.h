#ifndef BINOM_H
#define BINOM_H

#include "structure/variables/node_visitor.h"
#include "structure/file_storage/data_base_container.h"

namespace binom {

class BinOMFile {
  FileIO file;
public:
  BinOMFile(std::string filename);
  BinOMFile(const BinOMFile& other);
  BinOMFile(BinOMFile&& other);

  BinOMFile& write(Variable data);
  BinOMFile& load(Variable& data);
  BinOMFile& backup(DataBaseContainer& db);

  Variable load();

};

}

#endif // BINOM_H
