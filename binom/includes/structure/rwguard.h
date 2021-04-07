#ifndef RWGUARD_H
#define RWGUARD_H

#include <mutex>
#include <condition_variable>

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


#endif // RWGUARD_H
