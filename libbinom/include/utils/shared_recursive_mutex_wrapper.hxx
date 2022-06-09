#ifndef SHARED_RECURSIVE_MUTEX_WRAPPER_HXX
#define SHARED_RECURSIVE_MUTEX_WRAPPER_HXX

#include "type_aliases.hxx"
#include <shared_mutex>
#include <map>

namespace shared_recursive_mtx {
using namespace type_alias;

class SharedRecursiveLock;

enum class MtxLockType : ui8 {
  unlocked =      0x00,
  shared_locked = 0x01,
  unique_locked = 0x02
};

struct Counters {
  ui64 wrapper_count = 1;
  ui64 shared_lock_counter = 0;
  ui64 unique_lock_counter = 0;
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
  static inline thread_local std::map<std::shared_mutex*, Counters> counter_storage;
  std::map<std::shared_mutex*, Counters>::iterator mtx_data = counter_storage.end();

  static std::map<std::shared_mutex*, Counters>::iterator createCountersRef(std::shared_mutex* mtx) noexcept {
    if(!mtx) return counter_storage.end();
    auto it = counter_storage.find(mtx);
    if(it == counter_storage.cend())
      return counter_storage.emplace(mtx, Counters{}).first;
    ++it->second.wrapper_count;
    return it;
  }

  static void deleteCounterRef(std::map<std::shared_mutex*, Counters>::iterator it) noexcept {
    if(it != counter_storage.cend()) {
      if(!--it->second.wrapper_count) {
        if(it->second.unique_lock_counter) it->first->unlock();
        if(it->second.shared_lock_counter) it->first->unlock_shared();
        counter_storage.erase(it);
      }
    }
    it = counter_storage.end();
  }

  friend class RSMLocker;

public:

  SharedRecursiveMutexWrapper(
      std::shared_mutex* mtx,
      MtxLockType lock_type = MtxLockType::unlocked) noexcept
    : mtx_data(createCountersRef(mtx)) {
    if(!mtx) std::terminate();
    switch (lock_type) {
    case MtxLockType::unlocked: return;
    case MtxLockType::shared_locked: lockShared(); return;
    case MtxLockType::unique_locked: lock(); return;
    }
  }

  SharedRecursiveMutexWrapper(const SharedRecursiveMutexWrapper& other, MtxLockType lock_type = MtxLockType::unlocked) noexcept
    : SharedRecursiveMutexWrapper(other.mtx_data->first) {
    switch (lock_type) {
    case MtxLockType::unlocked: return;
    case MtxLockType::shared_locked: lockShared(); return;
    case MtxLockType::unique_locked: lock(); return;
    }
  }

  SharedRecursiveMutexWrapper(SharedRecursiveMutexWrapper&& other, MtxLockType lock_type = MtxLockType::unlocked) noexcept
    : mtx_data(other.mtx_data) {
    other.mtx_data = counter_storage.end();
    switch (lock_type) {
    case MtxLockType::unlocked: return;
    case MtxLockType::shared_locked: lockShared(); return;
    case MtxLockType::unique_locked: lock(); return;
    }
  }

  SharedRecursiveMutexWrapper(const SharedRecursiveLock& lock, MtxLockType lock_type = MtxLockType::unlocked);

  SharedRecursiveMutexWrapper(SharedRecursiveLock&& lock, MtxLockType lock_type = MtxLockType::unlocked);

  ~SharedRecursiveMutexWrapper() { deleteCounterRef(mtx_data); mtx_data = counter_storage.end(); }

  static ui64 getWrappedMutexCount() noexcept {return counter_storage.size();}

  ui64 getUniqueLockCount() const noexcept {return mtx_data->second.unique_lock_counter;}

  ui64 getSheredLockCount() const noexcept {return mtx_data->second.shared_lock_counter;}

  ui64 getThisMutexWrapperCount() const noexcept {return mtx_data->second.wrapper_count;}

  std::shared_mutex& getSharedMutex() const noexcept {return *mtx_data->first;}

  MtxLockType getLockType() const noexcept {
    if(mtx_data->second.unique_lock_counter) return MtxLockType::unique_locked;
    if(mtx_data->second.shared_lock_counter) return MtxLockType::shared_locked;
    return MtxLockType::unlocked;
  }

  void lock() noexcept {
    switch (getLockType()) {
    case MtxLockType::unlocked:
      mtx_data->first->lock();
      ++mtx_data->second.unique_lock_counter;
    return;
    case MtxLockType::shared_locked:
      mtx_data->first->unlock_shared();
      mtx_data->first->lock();
      ++mtx_data->second.unique_lock_counter;
    return;
    case MtxLockType::unique_locked:
      ++mtx_data->second.unique_lock_counter;
    return;
    }
  }

  void lockShared() noexcept {
    switch (getLockType()) {
    case MtxLockType::unlocked:
      mtx_data->first->lock_shared();
      ++mtx_data->second.shared_lock_counter;
    return;
    case MtxLockType::shared_locked:
      ++mtx_data->second.shared_lock_counter;
    return;
    case MtxLockType::unique_locked:
      ++mtx_data->second.shared_lock_counter;
    return;
    }
  }

  void unlock() noexcept {
    switch (getLockType()) {
    case MtxLockType::unlocked:
    case MtxLockType::shared_locked: return;
    case MtxLockType::unique_locked:
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
    case MtxLockType::unlocked: return;
    case MtxLockType::shared_locked:
      if(!--mtx_data->second.shared_lock_counter) mtx_data->first->unlock_shared();
    return;
    case MtxLockType::unique_locked:
      if(mtx_data->second.shared_lock_counter) --mtx_data->second.shared_lock_counter;
    return;
    }
  }

  SharedRecursiveMutexWrapper& operator = (const SharedRecursiveMutexWrapper& other) noexcept {
    this->~SharedRecursiveMutexWrapper();
    return *new(this) SharedRecursiveMutexWrapper(other);
  }

  SharedRecursiveMutexWrapper& operator = (SharedRecursiveMutexWrapper&& other) noexcept {
    this->~SharedRecursiveMutexWrapper();
    return *new(this) SharedRecursiveMutexWrapper(std::move(other));
  }

  SharedRecursiveMutexWrapper& operator = (const SharedRecursiveLock& other) noexcept {
    this->~SharedRecursiveMutexWrapper();
    return *new(this) SharedRecursiveMutexWrapper(other);
  }

  SharedRecursiveMutexWrapper& operator = (SharedRecursiveLock&& other) noexcept {
    this->~SharedRecursiveMutexWrapper();
    return *new(this) SharedRecursiveMutexWrapper(std::move(other));
  }

};


/**
 * @brief SharedRecursiveLock - RAII std::shared_mutex wrapper for recusive locking
 */
class SharedRecursiveLock : private SharedRecursiveMutexWrapper {
  MtxLockType lock_type;
  friend class SharedRecursiveMutexWrapper;
public:
  SharedRecursiveLock(const std::shared_mutex* mtx, MtxLockType lock_type)
    : SharedRecursiveMutexWrapper(const_cast<std::shared_mutex*>(mtx), lock_type), lock_type(lock_type) {}

  SharedRecursiveLock(const SharedRecursiveMutexWrapper& other, MtxLockType lock_type = MtxLockType::unlocked)
    : SharedRecursiveMutexWrapper(other, lock_type),
      lock_type(lock_type) {}

  SharedRecursiveLock(SharedRecursiveMutexWrapper&& other, MtxLockType lock_type = MtxLockType::unlocked)
    : SharedRecursiveMutexWrapper(std::move(other), lock_type),
      lock_type(lock_type) {}

  SharedRecursiveLock(const SharedRecursiveLock& other, MtxLockType lock_type = MtxLockType::unlocked)
    : SharedRecursiveMutexWrapper(other, lock_type),
      lock_type(lock_type) {}

  SharedRecursiveLock(SharedRecursiveLock&& other)
    : SharedRecursiveMutexWrapper(std::move(other)),
      lock_type(other.lock_type) {
    other.lock_type = MtxLockType::unlocked;
  }

  ~SharedRecursiveLock() {
    switch (lock_type) {
    case MtxLockType::unlocked: return;
    case MtxLockType::shared_locked: unlockShared(); return;
    case MtxLockType::unique_locked: unlock(); return;
    }
  }
};

inline SharedRecursiveMutexWrapper::SharedRecursiveMutexWrapper(const SharedRecursiveLock& lock, MtxLockType lock_type)
  : SharedRecursiveMutexWrapper(dynamic_cast<const SharedRecursiveMutexWrapper&>(lock), lock_type) {}

inline SharedRecursiveMutexWrapper::SharedRecursiveMutexWrapper(SharedRecursiveLock&& lock, MtxLockType lock_type)
  : SharedRecursiveMutexWrapper(dynamic_cast<SharedRecursiveMutexWrapper&&>(lock), lock_type) {}


/// Use it in case we DON'T need multithreading
class OptionalLockPlaceholder {
public:
  OptionalLockPlaceholder([[maybe_unused]] std::shared_mutex* mtx, [[maybe_unused]] MtxLockType lock_type) {}
  OptionalLockPlaceholder([[maybe_unused]] const OptionalLockPlaceholder& other, [[maybe_unused]] MtxLockType lock_type = MtxLockType::unlocked) {}
  OptionalLockPlaceholder([[maybe_unused]] OptionalLockPlaceholder&& other, [[maybe_unused]] MtxLockType lock_type = MtxLockType::unlocked) {}
  constexpr operator bool() const noexcept {return true;}
  constexpr bool has_value() const noexcept {return true;}
};

}

namespace binom {
using shared_recursive_mtx::MtxLockType;
using shared_recursive_mtx::SharedRecursiveLock;
typedef std::optional<shared_recursive_mtx::SharedRecursiveLock> OptionalSharedRecursiveLock;
}

#endif // SHARED_RECURSIVE_MUTEX_WRAPPER_HXX
