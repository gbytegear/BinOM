#ifndef MEMORY_MANAGER_TEST_H
#define MEMORY_MANAGER_TEST_H

#define DEBUG
#include "binom/includes/file_storage/file_memory_manager.h"
#include "binom/includes/binom.h"

using namespace binom;

void memory_manager_test() {
  std::clog << "\n\nmemory_manager_test()\n\n\n";

  FMemoryManager fmm("memtest.fmm");

//  ByteArray data;
//  data.pushBack<ui16>(65535);
//  fmm.createNode(VarType::word, data);
//  fmm.removeNode(1);
//  ByteArray data("Hello, World", sizeof ("Hello, World"));
//  fmm.updateNode(1, data);
//  fmm.createNode(VarType::byte_array, data);
//  binom::BufferArray data(VarType::word_array);
  binom::BufferArray data(ui16arr{1,2,3,4,5});
  fmm.updateNode(0, data.getType(), data.toByteArray());
  fmm.check();

}

#endif // MEMORY_MANAGER_TEST_H
