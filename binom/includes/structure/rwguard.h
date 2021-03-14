#ifndef RWGUARD_H
#define RWGUARD_H

#include <mutex>
#include <condition_variable>
#include <functional>

class RWGuard {
  std::condition_variable reader_condition;
  std::condition_variable writer_condition;
  std::mutex mtx;
  uint32_t active_writers;
  uint32_t waiting_writers;
  uint32_t active_readers;

public:
  RWGuard()
    : reader_condition(),
      writer_condition(),
      mtx(),
      active_writers(0),
      waiting_writers(0),
      active_readers(0)
  {}

  void lockR() {
    std::unique_lock<std::mutex> lk(mtx);
    while ( waiting_writers )
      reader_condition.wait(lk);
    ++active_readers;
    lk.unlock();
  }

  void unlockR() {
    std::unique_lock<std::mutex> lk(mtx);
    --active_readers;
    lk.unlock();
    writer_condition.notify_one();
  }

  void lockW() {
    std::unique_lock<std::mutex> lk(mtx);
    ++waiting_writers;
    while ( active_readers || active_writers )
      writer_condition.wait(lk);
    ++active_writers;
    lk.unlock();
  }

  void unlockW() {
    std::unique_lock<std::mutex> lk(mtx);
    --waiting_writers;
    --active_writers;
    if(waiting_writers)
      writer_condition.notify_one();
    else
      reader_condition.notify_all();
    lk.unlock();
  }



  inline void writeSync(std::function<void()> callback) {lockW();callback();unlockW();}
  inline void readSync(std::function<void()> callback) {lockR();callback();unlockR();}

};


#endif // RWGUARD_H
