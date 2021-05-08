#ifndef RWGUARD_H
#define RWGUARD_H

#include <mutex>
#include <condition_variable>
#include "types.h"

namespace binom {


class RWGuard {

  uint32_t reader_count = 0;
  bool writer = false;
  uint32_t reader_wait_count = 0;
  uint32_t writer_wait_count = 0;

  std::mutex mtx;
  std::condition_variable read_condition;
  std::condition_variable write_condition;

public:

  RWGuard();

  void readLock();
  void readUnlock();
  void writeLock();
  void writeUnlock();
  void readToWrite();
  void writeToRead();
};




typedef ui64 f_virtual_index;


class RWSyncMap {

  struct RWSyncMapNode {
    f_virtual_index node_index;
    RWGuard* guard;
    RWSyncMapNode(f_virtual_index node_index);
    ~RWSyncMapNode();
  };

  std::mutex map_mtx;
  ui64 length = 0;
  RWSyncMapNode* map = nullptr;

public:
  RWSyncMap() = default;
  RWGuard* get(f_virtual_index node_index);
};

}


#endif // RWGUARD_H
