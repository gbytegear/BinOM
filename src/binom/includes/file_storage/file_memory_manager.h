#ifndef FILE_MEMORY_MANAGER_H
#define FILE_MEMORY_MANAGER_H

#include "binom/includes/utils/file.h"
#include "fmm_containers.h"

namespace binom {


class FMemoryManager {
  FileIO file;
  NodePageVector node_page_vector;
  BytePageVector byte_page_vector;
  HeapPageVector heap_page_vector;


};

}

#endif // FILE_MEMORY_MANAGER_H
