#ifndef RESOURCE_CONTROL_HXX
#define RESOURCE_CONTROL_HXX

#include "shared_recursive_mutex_wrapper.hxx"
#include <atomic>

namespace binom::priv {

typedef std::optional<SharedRecursiveLock> OptionalSharedRecursiveLock;

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

    bool*  bool_ptr;
    ui8*   ui8_ptr;
    ui16*  ui16_ptr;
    ui32*  ui32_ptr;
    ui64*  ui64_ptr;
    i8*    i8_ptr;
    i16*   i16_ptr;
    i32*   i32_ptr;
    i64*   i64_ptr;
    f32*   f32_ptr;
    f64*   f64_ptr;

    template<typename T> T* asPointerAt() const noexcept { return reinterpret_cast<T*>(pointer);}
  };

  VarType type = VarType::null;
  Data data;
};



struct SharedResource {
  ResourceData resource_data;
  std::atomic_uint64_t link_counter = 1;
  std::shared_mutex mtx;

  bool isExist() noexcept {return link_counter;}
};

class Link {
  SharedResource* resource = nullptr;

public:
  Link(ResourceData resource_data) noexcept : resource(new SharedResource{.resource_data = resource_data}) {}
  Link(Link&& other) noexcept : resource(other.resource) {}
  Link(const Link& other) noexcept {
    if(!other.resource) return;
    SharedRecursiveLock lock(&other.resource->mtx, MtxLockType::unique_locked);
    if(!other.resource->isExist()) {
      ++other.resource->link_counter;
      resource = other.resource;
    } else return;
  }

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept {
    if(!resource) return OptionalSharedRecursiveLock();
    else return SharedRecursiveLock(&resource->mtx, lock_type);
  }

  ResourceData* operator*() const noexcept {
    auto lk = getLock(MtxLockType::shared_locked);
    if(lk) return &resource->resource_data;
    else return nullptr;
  }
};

}

#endif // RESOURCE_CONTROL_HXX