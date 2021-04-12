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

  RWGuard()
    : reader_count(0),
      writer(false),
      reader_wait_count(0),
      writer_wait_count(0),
      mtx(),
      read_condition(),
      write_condition() {}

  void readLock() {
    std::unique_lock lk(mtx);
    ++reader_wait_count;
    read_condition.wait(lk, [&](){return !writer_wait_count;});
    --reader_wait_count;
    ++reader_count;
    lk.unlock();
  }

  void readUnlock() {
    std::unique_lock lk(mtx);
    --reader_count;
    lk.unlock();
    write_condition.notify_one();
  }

  void writeLock() {
    std::unique_lock lk(mtx);
    ++writer_wait_count;
    write_condition.wait(lk, [&](){return (!reader_count && !writer);});
    --writer_wait_count;
    writer = true;
    lk.unlock();
  }

  void writeUnlock() {
    std::unique_lock lk(mtx);
    writer = false;
    if(writer_wait_count)
      write_condition.notify_one();
    else
      read_condition.notify_all();
    lk.unlock();
  }

  void readToWrite() {
    std::unique_lock lk(mtx);
    --reader_count;
    ++writer_wait_count;
    write_condition.wait(lk, [&](){return (!reader_count && !writer);});
    --writer_wait_count;
    writer = true;
    lk.unlock();
  }

  void writeToRead() {
    std::unique_lock lk(mtx);
    writer = false;
    if(writer_wait_count)
      write_condition.notify_one();
    else
      read_condition.notify_all();

    ++reader_wait_count;
    read_condition.wait(lk, [&](){return !writer_wait_count;});
    --reader_wait_count;
    ++reader_count;
    lk.unlock();
  }


};




typedef ui64 f_virtual_index;


class RWSyncMap {

  struct RWSyncMapNode {
    f_virtual_index node_index;
    RWGuard* guard;
    RWSyncMapNode(f_virtual_index node_index)
      : node_index(node_index), guard(new RWGuard()) {}
    ~RWSyncMapNode() {if(guard) delete guard;}
  };

  std::mutex map_mtx;
  ui64 length = 0;
  RWSyncMapNode* map = nullptr;

public:
  RWSyncMap() = default;

  RWGuard* get(f_virtual_index node_index) {
    map_mtx.lock();

    RWGuard* result;

    if(!map) {
      length = 1;
      return (map = new RWSyncMapNode(node_index))->guard;
    }

    i64 left = 0;
    i64 right = length;
    i64 middle;

    do {
      middle = (left + right) / 2;

      if(middle >= static_cast<i64>(length) ||
         left > right ||
         (middle == 0 && map[middle].node_index > node_index))
        break;

      if(map[middle].node_index > node_index) right = middle - 1;
      elif(map[middle].node_index < node_index) left = middle + 1;
      elif(map[middle].node_index == node_index) {
        result = map[middle].guard;
        map_mtx.unlock();
        return result;
      }

    } while(true);

    ++length;
    map = tryRealloc<RWSyncMapNode>(map, length);

    if(middle == 0) {
      memmove(map + 1, map, (length - 1) * sizeof(RWSyncMapNode));
      RWGuard* result = (new(map) RWSyncMapNode(node_index))->guard;
      map_mtx.unlock();
      return result;
    } elif(middle == static_cast<i64>(length)) {
      result = (new(map + length - 1) RWSyncMapNode(node_index))->guard;
      map_mtx.unlock();
      return result;
    }

    if(map[middle].node_index < node_index) ++middle;
    memmove(map + middle, map + middle + 1, (length - middle - 1) * sizeof(RWSyncMapNode));
    result = (new(map + middle) RWSyncMapNode(node_index))->guard;
    map_mtx.unlock();
    return result;

  }

};

}


#endif // RWGUARD_H
