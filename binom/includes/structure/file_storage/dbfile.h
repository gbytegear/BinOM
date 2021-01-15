#ifndef DBFILE_H
#define DBFILE_H

#include "vfmem.h"
#include "../variables/variable.h"

/*

Array:
[ArrayDescriptor]
[AOElementDescriptor 1]
[AOElementDescriptor 2]
...
[AOElementDescriptor n]

Object:
[ObjectDescriptor]
[...NameLengthBlock...]
[...NameBlock...]
[AOElementDescriptor 1]
[AOElementDescriptor 2]
...
[AOElementDescriptor n]

*/



namespace binom {

class DBFile {
  VFMemory vmemeory;

  ByteArray convert(Primitive primitive);
  ByteArray convert(BufferArray buffer);
  ByteArray convert(Array array);
  ByteArray convert(Object object);
public:
  DBFile(std::string filename) : vmemeory(std::move(filename)) {}
};

}

#endif // DBFILE_H
