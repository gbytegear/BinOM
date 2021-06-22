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

  inline bool isFree() {return !reader_count && !reader_wait_count && !writer && !writer_wait_count;}
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

  RWSyncMapNode* getIfExist(f_virtual_index node_index);

public:
  RWSyncMap() = default;
  RWGuard* get(f_virtual_index node_index);
  void tryRemove(f_virtual_index node_index);
};

enum class LockerType : ui8 {
  free, read, write
};

class RWScopedLock {
  RWSyncMap& map;
  f_virtual_index index = 0;
  RWGuard* guard = nullptr;
  LockerType type = LockerType::free;
public:
  RWScopedLock(RWSyncMap& map, f_virtual_index index, LockerType type = LockerType::free)
    : map(map), index(index), guard(map.get(index)), type(type) {
    switch (type) {
    case binom::LockerType::free: break;
    case binom::LockerType::read: guard->readLock(); break;
    case binom::LockerType::write: guard->writeLock(); break;
    }
  }

  ~RWScopedLock() {
    unlock();
    map.tryRemove(index);
  }

  void readLock() {
    unlock();
    guard->readLock();
    type = LockerType::read;
  }

  void writeLock() {
    unlock();
    guard->writeLock();
    type = LockerType::write;
  }

  void unlock() {
    switch (type) {
    case binom::LockerType::free: break;
    case binom::LockerType::read: guard->readUnlock(); break;
    case binom::LockerType::write: guard->writeUnlock(); break;
    }
    type = LockerType::free;
  }

  void readToWrite() {
    switch (type) {
    case binom::LockerType::free: guard->writeLock(); break;
    case binom::LockerType::read: guard->readToWrite(); break;
    case binom::LockerType::write: break;
    }
    type = LockerType::write;
  }

  void writeToRead() {
    switch (type) {
    case binom::LockerType::free: guard->readLock(); break;
    case binom::LockerType::read: break;
    case binom::LockerType::write: guard->writeToRead(); break;
    }
    type = LockerType::read;
  }

};

}


#endif // RWGUARD_H
