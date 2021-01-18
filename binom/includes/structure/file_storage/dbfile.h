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
  VFMemoryController vmemeory;

  ByteArray convert(Primitive primitive);
  ByteArray convert(BufferArray buffer);
  ByteArray convert(Array array);
  ByteArray convert(Object object);
public:
  DBFile(std::string filename) : vmemeory(std::move(filename)) {}

  inline ui64 getFileSize() {return vmemeory.getFileSize();}
  inline ui64 getDataSegmentsSize() {return vmemeory.getDataSegmentsSize();}
  inline ui64 getNodeSegmentsSize() {return vmemeory.getNodeSegmentsSize();}
  inline ui64 getPrimitiveSegmentsSize() {return vmemeory.getPrimitiveSegmentsSize();}

  inline ui64 getDataSegmentsCount() {return vmemeory.getDataSegmentsCount();}
  inline ui64 getNodeSegmentsCount() {return vmemeory.getNodeSegmentsCount();}
  inline ui64 getPrimitiveSegmentsCount() {return vmemeory.getPrimitiveSegmentsCount();}
};

}

#endif // DBFILE_H
