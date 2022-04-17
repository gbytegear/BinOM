#ifndef SHARED_RECURSIVE_MUTEX_WRAPPER_HXX
#define SHARED_RECURSIVE_MUTEX_WRAPPER_HXX

#include "types.hxx"
#include <shared_mutex>
#include <map>

namespace binom::priv {

enum class MtxLockType : ui8 {
  unlocked =      0x00,
  shared_locked = 0x01,
  unique_locked = 0x02
};

/**
 * @brief SMRecursiveWrapper - std::shared_mutex wrapper for recusive locking
 *
 * Mutex state map:
 * lock() unlock/shared_lock => unique_lock;
 * lockShared() unlock => shared_lock;
 * unlock() unique_lock => unlock/shared_lock;
 * unlockShared() shared_lock => unlock;
 */
class SharedRecursiveMutexWrapper {

  struct Counters {
    ui64 wrapper_count = 1;
    ui64 shared_lock_counter = 0;
    ui64 unique_lock_counter = 0;
  };

  static inline thread_local std::map<std::shared_mutex*, Counters> counter_storage;
  std::map<std::shared_mutex*, Counters>::iterator mtx_data;

  static std::map<std::shared_mutex*, Counters>::iterator createCountersRef(std::shared_mutex* mtx) noexcept {
    if(!mtx) return counter_storage.end();
    auto it = counter_storage.find(mtx);
    if(it == counter_storage.cend())
      return counter_storage.emplace(mtx, Counters{}).first;
    ++it->second.wrapper_count;
    return it;
  }

  static void deleteCounterRef(std::map<std::shared_mutex*, Counters>::iterator it) noexcept {counter_storage.erase(it);}

  friend class RSMLocker;

public:

  SharedRecursiveMutexWrapper(
      std::shared_mutex* mtx,
      MtxLockType lock_type = MtxLockType::unlocked) noexcept
    : mtx_data(createCountersRef(mtx)) {
    if(!mtx) std::terminate();
    switch (lock_type) {
    case binom::priv::MtxLockType::unlocked: return;
    case binom::priv::MtxLockType::shared_locked:
     mtx->lock_shared();
     ++mtx_data->second.shared_lock_counter;
    return;
    case binom::priv::MtxLockType::unique_locked:
      mtx->lock();
      ++mtx_data->second.unique_lock_counter;
    return;
    }
  }

  SharedRecursiveMutexWrapper(const SharedRecursiveMutexWrapper& other) noexcept
    : SharedRecursiveMutexWrapper(other.mtx_data->first) {}

  SharedRecursiveMutexWrapper(SharedRecursiveMutexWrapper&& other) noexcept
    : mtx_data(other.mtx_data) { other.mtx_data = counter_storage.end(); }

  ~SharedRecursiveMutexWrapper() { deleteCounterRef(mtx_data); }

  static ui64 getWrappedMutexCount() noexcept {return counter_storage.size();}

  ui64 getUniqueLockCount() const noexcept {return mtx_data->second.unique_lock_counter;}

  ui64 getSheredLockCount() const noexcept {return mtx_data->second.shared_lock_counter;}

  ui64 getThisMutexWrapperCount() const noexcept {return mtx_data->second.wrapper_count;}

  MtxLockType getLockType() const noexcept {
    if(mtx_data->second.unique_lock_counter) return MtxLockType::unique_locked;
    if(mtx_data->second.shared_lock_counter) return MtxLockType::shared_locked;
    return MtxLockType::unlocked;
  }

  void lock() noexcept {
    switch (getLockType()) {
    case binom::priv::MtxLockType::unlocked:
      mtx_data->first->lock();
      ++mtx_data->second.unique_lock_counter;
    return;
    case binom::priv::MtxLockType::shared_locked:
      mtx_data->first->unlock_shared();
      mtx_data->first->lock();
      ++mtx_data->second.unique_lock_counter;
    return;
    case binom::priv::MtxLockType::unique_locked:
      ++mtx_data->second.unique_lock_counter;
    return;
    }
  }

  void lockShared() noexcept {
    switch (getLockType()) {
    case binom::priv::MtxLockType::unlocked:
      mtx_data->first->lock_shared();
      ++mtx_data->second.shared_lock_counter;
    return;
    case binom::priv::MtxLockType::shared_locked:
      ++mtx_data->second.shared_lock_counter;
    return;
    case binom::priv::MtxLockType::unique_locked:
      ++mtx_data->second.shared_lock_counter;
    return;
    }
  }

  void unlock() noexcept {
    switch (getLockType()) {
    case binom::priv::MtxLockType::unlocked:
    case binom::priv::MtxLockType::shared_locked: return;
    case binom::priv::MtxLockType::unique_locked:
      if(!--mtx_data->second.unique_lock_counter) {
        if(mtx_data->second.shared_lock_counter) {

          // "A prior unlock() operation on the same mutex synchronizes-with (as defined in std::memory_order) this operation."
          // - (C) [https://en.cppreference.com/w/cpp/thread/shared_mutex/lock_shared] 3.04.2022
          mtx_data->first->unlock(); mtx_data->first->lock_shared();

        } else mtx_data->first->unlock();
      }
    return;
    }
  }

  void unlockShared() noexcept {
    switch (getLockType()) {
    case binom::priv::MtxLockType::unlocked: return;
    case binom::priv::MtxLockType::shared_locked:
      if(!--mtx_data->second.shared_lock_counter) mtx_data->first->unlock_shared();
    return;
    case binom::priv::MtxLockType::unique_locked:
      if(mtx_data->second.shared_lock_counter) --mtx_data->second.shared_lock_counter;
    return;
    }
  }

};

}

#endif // SHARED_RECURSIVE_MUTEX_WRAPPER_HXX
