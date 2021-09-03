#ifndef BINOM_H
#define BINOM_H

#include "utils/node_visitor.h"
#include "file_storage/file_storage.h"
#include "utils/file.h"

namespace binom {

//class BinOMFile {
//  FileIO file;

//  void writeNode(ui64& file_carret, DBNodeVisitor& node);

//public:
//  BinOMFile() = default;
//  BinOMFile(std::string filename);
//  BinOMFile(const BinOMFile& other);
//  BinOMFile(BinOMFile&& other);

//  BinOMFile& operator=(const BinOMFile& other);
//  BinOMFile& operator=(BinOMFile&& other);

//  inline void close() {return file.close();}
//  inline bool open(std::string filename) {return file.open(std::move(filename));}

//  inline ui64 size() {return file.size();}
//  inline bool isExist() {return file.isExist();}
//  inline fs::path path() const {return file.path();}
//  inline bool isEmpty() {return file.isEmpty();}
//  inline bool isOpen() {return file.isOpen();}

//  BinOMFile& write(Variable data);
//  BinOMFile& load(Variable& data);
//  BinOMFile& write(BinOMDataBase& db);

//  Variable load();
//};

}

#endif // BINOM_H
