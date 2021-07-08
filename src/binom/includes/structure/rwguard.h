#ifndef RWGUARD_H
#define RWGUARD_H

#include <shared_mutex>
#include <map>
#include <condition_variable>
#include "types.h"


namespace binom {

typedef ui64 f_virtual_index;

struct RWSyncMap {
  class RWGuard;
private:
  class RWGuardAutoDelete;
  std::map<f_virtual_index, std::weak_ptr<RWGuardAutoDelete>> mtx_map;
  std::mutex general_mtx;

  class RWGuardAutoDelete {
    RWSyncMap* map;
    std::weak_ptr<RWGuardAutoDelete> weak_ptr;
    std::shared_mutex mtx;
    std::map<f_virtual_index, std::weak_ptr<RWGuardAutoDelete>>::iterator it;


    friend class RWGuard;
  public:
    RWGuardAutoDelete(RWSyncMap* map)
      : map(map), mtx() {}
    RWGuardAutoDelete(const RWGuardAutoDelete&) = delete;
    ~RWGuardAutoDelete() {
      map->general_mtx.lock();
      if(auto shr_ptr = it->second.lock(); !shr_ptr)
        map->mtx_map.erase(it);
      map->general_mtx.unlock();
    }
  };

public:

  class RWGuard {
    std::shared_ptr<RWGuardAutoDelete> shr_ptr;

    RWGuard(f_virtual_index v_index, RWSyncMap* map) : shr_ptr(std::make_shared<RWGuardAutoDelete>(map)) {
      shr_ptr->weak_ptr = shr_ptr;
      map->mtx_map.emplace(v_index, shr_ptr);
      shr_ptr->it = map->mtx_map.find(v_index);
    }

    RWGuard(std::weak_ptr<RWGuardAutoDelete>& weak, f_virtual_index v_index, RWSyncMap* map) : shr_ptr(weak.lock()) {
      if(!shr_ptr) {
        new(this) RWGuard(v_index, map);
        weak = shr_ptr;
      }
    }

    friend struct RWSyncMap;
  public:
    RWGuard(RWGuard&& other) : shr_ptr(std::move(other.shr_ptr)) {}
    RWGuard(RWGuard&) = delete;

    void lock() {shr_ptr->mtx.lock();}
    bool tryLock() {return shr_ptr->mtx.try_lock();}
    void unlock() {shr_ptr->mtx.unlock();}
    void lockShared() {shr_ptr->mtx.lock_shared();}
    bool tryLockShared() {return shr_ptr->mtx.try_lock_shared();}
    void unlockShared() {shr_ptr->mtx.unlock_shared();}
  };

  RWSyncMap() = default;

  RWGuard get(f_virtual_index node_index) {
    general_mtx.lock();
    if(auto it = mtx_map.find(node_index); it != mtx_map.cend()) {
      RWGuard guard(it->second, node_index, this);
      general_mtx.unlock();
      return guard;
    } else {
      RWGuard guard(node_index, this);
      general_mtx.unlock();
      return guard;
    }
  }
};


}


#endif // RWGUARD_H
