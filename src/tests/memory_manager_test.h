#ifndef MEMORY_MANAGER_TEST_H
#define MEMORY_MANAGER_TEST_H

#define DEBUG
#include "binom/includes/file_storage/file_memory_manager.h"

using namespace binom;

void memory_manager_test() {
  std::clog << "\n\nmemory_manager_test()\n\n\n";

  FMemoryManager fmm("memtest.fmm");

//  ByteArray data;
//  data.pushBack<ui16>(65535);
//  fmm.createNode(VarType::word, data);
//  fmm.removeNode(1);
//  ByteArray data("Hello, World", sizeof ("Hello, World"));
//  fmm.createNode(VarType::byte_array, data);
  fmm.check();

}

#endif // MEMORY_MANAGER_TEST_H
