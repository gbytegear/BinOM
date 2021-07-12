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
  enum class LockType {
    shared_lock,
    unique_lock,
    unlocked
  };

  class RWGuard {
    std::shared_ptr<RWGuardAutoDelete> shr_ptr;
    LockType lock_type = LockType::unlocked;
    ui64 lock_count = 0;

    RWGuard(f_virtual_index v_index, RWSyncMap* map) : shr_ptr(std::make_shared<RWGuardAutoDelete>(map)) {
      map->mtx_map.emplace(v_index, shr_ptr);
      shr_ptr->it = map->mtx_map.find(v_index);
    }

    RWGuard(std::weak_ptr<RWGuardAutoDelete>& weak, f_virtual_index v_index, RWSyncMap* map) : shr_ptr(weak.lock()) {
      if(!shr_ptr) {
        new(this) RWGuard(v_index, map);
        weak = shr_ptr;
      }
    }

    void forceUnlock() {
      if(!shr_ptr) return;
      lock_count = 0;
      switch (lock_type) {
        case LockType::shared_lock:
          shr_ptr->mtx.unlock_shared();
          lock_type = LockType::unlocked;
        return;
        case LockType::unique_lock:
          shr_ptr->mtx.unlock();
          lock_type = LockType::unlocked;
        return;
        case LockType::unlocked: return;
      }
    }

    friend struct RWSyncMap;
  public:
    RWGuard() = default;
    RWGuard(RWGuard&& other) : shr_ptr(std::move(other.shr_ptr)) {}
    RWGuard(RWGuard&) = delete;
    ~RWGuard() {forceUnlock();}

    f_virtual_index getLockedIndex() {return shr_ptr->it->first;}

    void clear() {
      forceUnlock();
      shr_ptr.reset();
    }

    void operator=(RWGuard&& other) {
      this->~RWGuard();
      new(this) RWGuard(std::move(other));
    }

    LockType getLockType() {return lock_type;}

    void lock() {
      if(!shr_ptr) return;
      if(lock_type == LockType::unique_lock) {
        ++lock_count;
        return;
      }
      forceUnlock();
      shr_ptr->mtx.lock();
      lock_type = LockType::unique_lock;
    }


    void lockShared() {
      if(!shr_ptr) return;
      if(lock_type != LockType::unlocked) {
        ++lock_count;
        return;
      }
      shr_ptr->mtx.lock_shared();
      lock_type = LockType::shared_lock;
    }

    void unlock() {
      if(!shr_ptr) return;
      if(lock_count) {
        --lock_count;
        return;
      }
      forceUnlock();
    }

//    bool tryLock() {
//      if(!shr_ptr) return;
//      unlock();
//      if(shr_ptr->mtx.try_lock()) {
//        lock_type = LockType::unique_lock;
//        return true;
//      }
//      return false;
//    }

//    bool tryLockShared() {
//      if(!shr_ptr) return;
//      unlock();
//      if(shr_ptr->mtx.try_lock_shared()) {
//        lock_type = LockType::shared_lock;
//        return true;
//      }
//      return false;
//    }
  };

  class ScopedRWGuard {
    RWSyncMap::RWGuard& rwg;
  public:
    ScopedRWGuard(RWGuard& rwg,
                  LockType lock_type = LockType::unlocked)
      : rwg(rwg) {
      switch (lock_type) {
        case binom::RWSyncMap::LockType::shared_lock:
          rwg.lockShared();
        return;
        case binom::RWSyncMap::LockType::unique_lock:
          rwg.lock();
        return;
        case binom::RWSyncMap::LockType::unlocked:return;
      }
    }

    ~ScopedRWGuard() {rwg.unlock();}

    inline void lock() {rwg.lock();}
    inline void lockShared() {rwg.lockShared();}
    inline void unlock() {rwg.unlock();}
  };

  RWSyncMap() = default;
  ~RWSyncMap() {}

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
