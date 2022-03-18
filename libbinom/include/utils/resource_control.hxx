#ifndef RESOURCE_CONTROL_H
#define RESOURCE_CONTROL_H

#include "types.hxx"
#include <shared_mutex>
#include <atomic>

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

  ui64 shared_lock_counter = 0;
  ui64 unique_lock_counter = 0;
  std::shared_mutex* mtx;

public:

  SMRecursiveWrapper(std::shared_mutex* mtx) noexcept : mtx(mtx) {}
  SMRecursiveWrapper(std::shared_mutex* mtx, MtxLockType lock_type) noexcept : mtx(mtx) {
    switch (lock_type) {
    case binom::priv::MtxLockType::unlocked: return;
    case binom::priv::MtxLockType::shared_locked:
     mtx->lock_shared();
     ++shared_lock_counter;
    return;
    case binom::priv::MtxLockType::unique_locked:
      mtx->lock();
      ++unique_lock_counter;
    return;
    }
  }

  MtxLockType getLockType() const noexcept {
    if(unique_lock_counter) return MtxLockType::unique_locked;
    if(shared_lock_counter) return MtxLockType::shared_locked;
    return MtxLockType::unlocked;
  }

  void lock() noexcept {
    switch (getLockType()) {
    case binom::priv::MtxLockType::unlocked:
      mtx->lock();
      ++unique_lock_counter;
    return;
    case binom::priv::MtxLockType::shared_locked:
      mtx->unlock_shared();
      mtx->lock();
      ++unique_lock_counter;
    return;
    case binom::priv::MtxLockType::unique_locked:
      ++unique_lock_counter;
    return;
    }
  }

  void lockShared() noexcept {
    switch (getLockType()) {
    case binom::priv::MtxLockType::unlocked:
      mtx->lock_shared();
      ++shared_lock_counter;
    return;
    case binom::priv::MtxLockType::shared_locked:
      ++shared_lock_counter;
    return;
    case binom::priv::MtxLockType::unique_locked:
      ++shared_lock_counter;
    return;
    }
  }

  void unlock() noexcept {
    switch (getLockType()) {
    case binom::priv::MtxLockType::unlocked:
    case binom::priv::MtxLockType::shared_locked: return;
    case binom::priv::MtxLockType::unique_locked:
      if(!--unique_lock_counter) mtx->unlock();
    return;
    }
  }

  void unlockShared() noexcept {
    switch (getLockType()) {
    case binom::priv::MtxLockType::unlocked: return;
    case binom::priv::MtxLockType::shared_locked:
      if(!--shared_lock_counter) mtx->unlock_shared();
    return;
    case binom::priv::MtxLockType::unique_locked:
      if(shared_lock_counter) --shared_lock_counter;
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
  std::atomic_uint64_t hard_link_counter = 1;
  std::atomic_uint64_t soft_link_counter = 0;
  ResourceData resource_date;
};

}

#endif // RESOURCE_CONTROL_H
