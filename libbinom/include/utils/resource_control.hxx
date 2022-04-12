#ifndef RESOURCE_CONTROL_H
#define RESOURCE_CONTROL_H

#include "types.hxx"
#include <shared_mutex>
#include <atomic>
#include <map>
#include <set>

namespace binom::priv {

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


class RSMLocker {
  MtxLockType lock_type;
  SharedRecursiveMutexWrapper& rsmw;
public:
  RSMLocker(SharedRecursiveMutexWrapper& rsmw, MtxLockType lock_type) noexcept
    : lock_type(lock_type), rsmw(rsmw) {
    switch (lock_type) {
    case binom::priv::MtxLockType::unlocked: return;
    case binom::priv::MtxLockType::shared_locked: rsmw.lockShared(); return;
    case binom::priv::MtxLockType::unique_locked: rsmw.lock(); return;
    }
  }

  ~RSMLocker() {
    switch (lock_type) {
    case binom::priv::MtxLockType::unlocked: return;
    case binom::priv::MtxLockType::shared_locked: rsmw.unlockShared(); return;
    case binom::priv::MtxLockType::unique_locked: rsmw.unlock(); return;
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

    template<typename T> T* asPointerAt() const noexcept { return reinterpret_cast<T*>(pointer);}

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

struct SharedResource;

class LinkTraget {
  const ResourceLinkType link_type;
  LinkTraget() = delete;
public:

  ResourceLinkType getLinkType() const noexcept { return link_type; }

  SharedResource& getSharedResource() const noexcept {
    switch (link_type) {
    case binom::priv::ResourceLinkType::hard_link:
    return *reinterpret_cast<SharedResource*>(const_cast<LinkTraget*>(this));
    case binom::priv::ResourceLinkType::weak_link:
    return *reinterpret_cast<SharedResource*>(reinterpret_cast<byte*>(const_cast<LinkTraget*>(this)) - 1);
    }
  }

  void destoryLink() noexcept;

};

struct SharedResource {
  const ResourceLinkType type_info[2] = {ResourceLinkType::hard_link, ResourceLinkType::weak_link};
  ResourceData resource_data;
  std::atomic_uint64_t hard_link_counter = 1;
  std::atomic_uint64_t soft_link_counter = 0;
  std::shared_mutex mtx;

  static LinkTraget* createNewResource(ResourceData resource_data) {
    SharedResource* new_shared_resource = new SharedResource{.resource_data = resource_data};
    return reinterpret_cast<LinkTraget*>(const_cast<ResourceLinkType*>(new_shared_resource->type_info));
  }

  LinkTraget* createHardLink() noexcept {
    ++hard_link_counter;
    return reinterpret_cast<LinkTraget*>(const_cast<ResourceLinkType*>(type_info));
  }

  LinkTraget* createSoftLink() noexcept {
    ++soft_link_counter;
    return reinterpret_cast<LinkTraget*>(const_cast<ResourceLinkType*>(type_info + 1));
  }

};

inline void LinkTraget::destoryLink() noexcept {
  switch (link_type) {
  case binom::priv::ResourceLinkType::hard_link:{
    SharedResource& res = *reinterpret_cast<SharedResource*>(const_cast<LinkTraget*>(this));
    if(!--res.hard_link_counter && !res.soft_link_counter) {
      // TODO: Destroy resource
      delete &res;
    } else {
      // TODO: Destroy resource
    }
  }
  break;
  case binom::priv::ResourceLinkType::weak_link:{
    SharedResource& res = *reinterpret_cast<SharedResource*>(reinterpret_cast<byte*>(const_cast<LinkTraget*>(this)) - 1);
    if(--res.soft_link_counter && !res.soft_link_counter) {
      // TODO: Destroy resource
      delete &res;
    } else {
      // TODO: Destroy resource
    }
  }
  break;
  }
}


class Link {
  LinkTraget* link_target;

  Link(LinkTraget* link_target) : link_target(link_target) {}

public:
  Link(ResourceData resource_data) noexcept : link_target(SharedResource::createNewResource(resource_data)) {}
  Link(Link&& other) noexcept : link_target(other.link_target) {}
  Link(const Link&) noexcept = delete;

  ResourceData& getData() const noexcept {return link_target->getSharedResource().resource_data;}

  SharedRecursiveMutexWrapper getMutex() const noexcept {return &link_target->getSharedResource().mtx;}

  LinkType getLinkType() const noexcept {
    switch (link_target->getLinkType()) {
    case binom::priv::ResourceLinkType::hard_link: return LinkType::hard_link;
    case binom::priv::ResourceLinkType::weak_link: return LinkType::soft_link;
    }
  }

  Link createHardLink() noexcept {return link_target->getSharedResource().createHardLink();}

  Link createSoftLink() noexcept {return link_target->getSharedResource().createSoftLink();}

};

}

#endif // RESOURCE_CONTROL_H
