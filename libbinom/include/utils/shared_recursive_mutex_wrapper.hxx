#ifndef SHARED_RECURSIVE_MUTEX_WRAPPER_HXX
#define SHARED_RECURSIVE_MUTEX_WRAPPER_HXX

#include "type_aliases.hxx"
#include "reverse_iterator.hxx"
#include <shared_mutex>
#include <mutex>
#include <map>
#include <set>
#include <stack>
#include <optional>

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
  thread_local static std::map<std::shared_mutex*, Counters> counter_storage;
  std::map<std::shared_mutex*, Counters>::iterator mtx_data = counter_storage.end();
  static std::map<std::shared_mutex*, Counters>::iterator createCountersRef(const std::shared_mutex* mtx) noexcept;
  static void deleteCounterRef(std::map<std::shared_mutex*, Counters>::iterator it) noexcept;

  friend class RSMLocker;

public:

  SharedRecursiveMutexWrapper(const std::shared_mutex* mtx, MtxLockType lock_type = MtxLockType::unlocked) noexcept;
  SharedRecursiveMutexWrapper(const SharedRecursiveMutexWrapper& other, MtxLockType lock_type = MtxLockType::unlocked) noexcept;
  SharedRecursiveMutexWrapper(SharedRecursiveMutexWrapper&& other, MtxLockType lock_type = MtxLockType::unlocked) noexcept;
  SharedRecursiveMutexWrapper(const SharedRecursiveLock& lock, MtxLockType lock_type = MtxLockType::unlocked);
  SharedRecursiveMutexWrapper(SharedRecursiveLock&& lock, MtxLockType lock_type = MtxLockType::unlocked);

  ~SharedRecursiveMutexWrapper();

  static ui64 getWrappedMutexCount() noexcept;
  ui64 getUniqueLockCount() const noexcept;
  ui64 getSheredLockCount() const noexcept;
  ui64 getThisMutexWrapperCount() const noexcept;
  std::shared_mutex& getSharedMutex() const noexcept;
  MtxLockType getLockType() const noexcept;
  void lock() noexcept;
  void lockShared() noexcept;
  void unlock() noexcept;
  void unlockShared() noexcept;
  bool tryLock() noexcept;
  bool tryLockShared() noexcept;


  SharedRecursiveMutexWrapper& operator = (const SharedRecursiveMutexWrapper& other) noexcept;
  SharedRecursiveMutexWrapper& operator = (SharedRecursiveMutexWrapper&& other) noexcept;
  SharedRecursiveMutexWrapper& operator = (const SharedRecursiveLock& other) noexcept;
  SharedRecursiveMutexWrapper& operator = (SharedRecursiveLock&& other) noexcept;

};


/**
 * @brief SharedRecursiveLock - RAII std::shared_mutex wrapper for recusive locking
 */
class SharedRecursiveLock : private SharedRecursiveMutexWrapper {
  MtxLockType lock_type;
  friend class SharedRecursiveMutexWrapper;
public:
  SharedRecursiveLock(const std::shared_mutex* mtx, MtxLockType lock_type);
  SharedRecursiveLock(const SharedRecursiveMutexWrapper& other, MtxLockType lock_type = MtxLockType::unlocked);
  SharedRecursiveLock(SharedRecursiveMutexWrapper&& other, MtxLockType lock_type = MtxLockType::unlocked);
  SharedRecursiveLock(const SharedRecursiveLock& other, MtxLockType lock_type = MtxLockType::unlocked);
  SharedRecursiveLock(SharedRecursiveLock&& other);

  ~SharedRecursiveLock();
};

inline SharedRecursiveMutexWrapper::SharedRecursiveMutexWrapper(const SharedRecursiveLock& lock, MtxLockType lock_type)
  : SharedRecursiveMutexWrapper(dynamic_cast<const SharedRecursiveMutexWrapper&>(lock), lock_type) {}

inline SharedRecursiveMutexWrapper::SharedRecursiveMutexWrapper(SharedRecursiveLock&& lock, MtxLockType lock_type)
  : SharedRecursiveMutexWrapper(dynamic_cast<SharedRecursiveMutexWrapper&&>(lock), lock_type) {}

class TransactionLock {
  std::stack<SharedRecursiveMutexWrapper> mtx_stack;
  MtxLockType lock_type = MtxLockType::unique_locked;

public:
  TransactionLock(std::set<std::shared_mutex*> mtx_set, MtxLockType lock_type = MtxLockType::unique_locked);
  TransactionLock(TransactionLock&& other);
  ~TransactionLock();
};

}

namespace binom {
using shared_recursive_mtx::MtxLockType;
using shared_recursive_mtx::SharedRecursiveLock;
using shared_recursive_mtx::TransactionLock;
typedef std::optional<shared_recursive_mtx::SharedRecursiveLock> OptionalSharedRecursiveLock;
}

#endif // SHARED_RECURSIVE_MUTEX_WRAPPER_HXX
