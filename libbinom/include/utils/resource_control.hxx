#ifndef RESOURCE_CONTROL_H
#define RESOURCE_CONTROL_H

#include "types.hxx"
#include <shared_mutex>
#include <atomic>
#include <map>

namespace binom::priv {

enum class MtxLockType : ui8 {
  unlocked = 0x00,
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
class SMRecursiveWrapper {

  struct Counters {
    ui64 wrapper_count = 1;
    ui64 shared_lock_counter = 0;
    ui64 unique_lock_counter = 0;
  };

  static inline thread_local std::map<std::shared_mutex*, Counters> counter_storage;

  Counters& counters;
  std::shared_mutex* mtx;

  static Counters& createCountersRef(std::shared_mutex* mtx) {
    auto it = counter_storage.find(mtx);
    if(it == counter_storage.cend())
      return counter_storage.emplace(mtx, Counters{}).first->second;
    ++it->second.wrapper_count;
    return it->second;
  }

  static void deleteCounterRef(std::shared_mutex* mtx) {
    auto it = counter_storage.find(mtx);
    if(it != counter_storage.cend())
      if(!--it->second.wrapper_count)
        counter_storage.erase(it);
  }

public:

  SMRecursiveWrapper(std::shared_mutex* mtx, MtxLockType lock_type = MtxLockType::unlocked) noexcept : counters(createCountersRef(mtx)), mtx(mtx) {
    if(!mtx) std::terminate();
    switch (lock_type) {
    case binom::priv::MtxLockType::unlocked: return;
    case binom::priv::MtxLockType::shared_locked:
     mtx->lock_shared();
     ++counters.shared_lock_counter;
    return;
    case binom::priv::MtxLockType::unique_locked:
      mtx->lock();
      ++counters.unique_lock_counter;
    return;
    }
  }

  ~SMRecursiveWrapper() { deleteCounterRef(mtx); }

  static ui64 getWrappedMutexCount() noexcept {return counter_storage.size();}

  ui64 getUniqueLockCount() const noexcept {return counters.unique_lock_counter;}

  ui64 getSheredLockCount() const noexcept {return counters.shared_lock_counter;}

  ui64 getThisMutexWrapperCount() const noexcept {return counters.wrapper_count;}

  MtxLockType getLockType() const noexcept {
    if(counters.unique_lock_counter) return MtxLockType::unique_locked;
    if(counters.shared_lock_counter) return MtxLockType::shared_locked;
    return MtxLockType::unlocked;
  }

  void lock() noexcept {
    switch (getLockType()) {
    case binom::priv::MtxLockType::unlocked:
      mtx->lock();
      ++counters.unique_lock_counter;
    return;
    case binom::priv::MtxLockType::shared_locked:
      mtx->unlock_shared();
      mtx->lock();
      ++counters.unique_lock_counter;
    return;
    case binom::priv::MtxLockType::unique_locked:
      ++counters.unique_lock_counter;
    return;
    }
  }

  void lockShared() noexcept {
    switch (getLockType()) {
    case binom::priv::MtxLockType::unlocked:
      mtx->lock_shared();
      ++counters.shared_lock_counter;
    return;
    case binom::priv::MtxLockType::shared_locked:
      ++counters.shared_lock_counter;
    return;
    case binom::priv::MtxLockType::unique_locked:
      ++counters.shared_lock_counter;
    return;
    }
  }

  void unlock() noexcept {
    switch (getLockType()) {
    case binom::priv::MtxLockType::unlocked:
    case binom::priv::MtxLockType::shared_locked: return;
    case binom::priv::MtxLockType::unique_locked:
      if(!--counters.unique_lock_counter) {
        if(counters.shared_lock_counter) {

          // "A prior unlock() operation on the same mutex synchronizes-with (as defined in std::memory_order) this operation."
          // - (C) [https://en.cppreference.com/w/cpp/thread/shared_mutex/lock_shared] 3.04.2022

          mtx->unlock(); mtx->lock_shared();
        } else mtx->unlock();
      }
    return;
    }
  }

  void unlockShared() noexcept {
    switch (getLockType()) {
    case binom::priv::MtxLockType::unlocked: return;
    case binom::priv::MtxLockType::shared_locked:
      if(!--counters.shared_lock_counter) mtx->unlock_shared();
    return;
    case binom::priv::MtxLockType::unique_locked:
      if(counters.shared_lock_counter) --counters.shared_lock_counter;
    return;
    }
  }
};

struct ResourceData {
  union Data {
    void* pointer = nullptr;
    bool  bool_val;
    ui8   ui8_val;
    ui16  ui16_val;
    ui32  ui32_val;
    ui64  ui64_val;
    i8    i8_val;
    i16   i16_val;
    i32   i32_val;
    i64   i64_val;
    f32   f32_val;
    f64   f64_val;

    template<typename T> T* as() const noexcept { return reinterpret_cast<T*>(pointer);}

    Data() = default;
    Data(void* pointer) : pointer(pointer)    {}
    Data(bool bool_val) : bool_val(bool_val)  {}
    Data(ui8 ui8_val)   : ui8_val(ui8_val)    {}
    Data(ui16 ui16_val) : ui16_val(ui16_val)  {}
    Data(ui32 ui32_val) : ui32_val(ui32_val)  {}
    Data(ui64 ui64_val) : ui64_val(ui64_val)  {}
    Data(i8 i8_val)     : i8_val(i8_val)      {}
    Data(i16 i16_val)   : i16_val(i16_val)    {}
    Data(i32 i32_val)   : i32_val(i32_val)    {}
    Data(i64 i64_val)   : i64_val(i64_val)    {}
    Data(f32 f32_val)   : f32_val(f32_val)    {}
    Data(f64 f64_val)   : f64_val(f64_val)    {}
  };
  VarType type = VarType::null;
  Data data;
};

enum class ResourceLinkType : char {
  hard_link = 'h',
  weak_link = 'w'
};

struct SharedResource {
  const ResourceLinkType type_info[2] = {ResourceLinkType::hard_link, ResourceLinkType::weak_link};
  ResourceData resource_date;
  std::atomic_uint64_t hard_link_counter = 1;
  std::atomic_uint64_t soft_link_counter = 0;
  std::shared_mutex mtx;
};

class SharedResourceLinkTraget {
  const ResourceLinkType link_type;

  SharedResource& getSharedResource() const noexcept {
    switch (link_type) {
    case binom::priv::ResourceLinkType::hard_link:
    return *reinterpret_cast<SharedResource*>(const_cast<SharedResourceLinkTraget*>(this));
    case binom::priv::ResourceLinkType::weak_link:
    return *reinterpret_cast<SharedResource*>(const_cast<SharedResourceLinkTraget*>(this) - 1);
    }
  }

public:
  ResourceLinkType getLinkType() const noexcept {return link_type;}


};


class Link {
  SharedResourceLinkTraget* shared_resource;
};

}

#endif // RESOURCE_CONTROL_H
