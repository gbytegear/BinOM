#ifndef RESOURCE_CONTROL_HXX
#define RESOURCE_CONTROL_HXX

#include "shared_recursive_mutex_wrapper.hxx"
#include "variable_memory.hxx"
#include <atomic>

namespace binom::priv {

struct ResourceData {

  union Data {
    void* pointer = nullptr;

    // Number
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

    // BitArray
    BitArrayHeader* bit_array_header;
    // BufferArray
    BufferArrayHeader* buffer_array_header;
    // Array
    // List
    // Map

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

  SharedResource(ResourceData resource_data) : resource_data(resource_data) {}
  ~SharedResource() {
    switch (toTypeClass(resource_data.type)) {
    case binom::VarTypeClass::null:
    case binom::VarTypeClass::number: return;
    case binom::VarTypeClass::bit_array: delete resource_data.data.bit_array_header; return;
    case binom::VarTypeClass::buffer_array: delete resource_data.data.buffer_array_header; return;
    case binom::VarTypeClass::array: return; // TODO
    case binom::VarTypeClass::list: return; // TODO
    case binom::VarTypeClass::map: return; // TODO
    case binom::VarTypeClass::invalid_type: default:
    return;
    }
  }
};

class Link {
  SharedResource* resource = nullptr;

public:
  Link(ResourceData resource_data) noexcept : resource(new SharedResource(resource_data)) {}

  Link(Link&& other) noexcept : resource(other.resource) {other.resource = nullptr;}

  Link(const Link& other) noexcept {
    if(!other.resource) return;
    if(auto lk = other.getLock(MtxLockType::shared_locked); lk) {
      ++other.resource->link_counter;
      resource = other.resource;
    } else return;
  }

  ~Link() {
    SharedResource* old_resource = resource;
    resource = nullptr;
    if(old_resource) {
      if(!--old_resource->link_counter) {
        old_resource->mtx.lock();
        old_resource->mtx.unlock();
        delete old_resource;
      }
    }
  }

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept {
    if(!resource) return OptionalSharedRecursiveLock();
    if(!resource->isExist()) return OptionalSharedRecursiveLock();
    return OptionalSharedRecursiveLock(SharedRecursiveLock(&resource->mtx, lock_type));
  }

  ResourceData* operator*() const noexcept {
    if(auto lk = getLock(MtxLockType::shared_locked); lk)
      return &resource->resource_data;
    else return nullptr;
  }

  ResourceData* operator->() const noexcept {
    if(auto lk = getLock(MtxLockType::shared_locked); lk)
      return &resource->resource_data;
    else return nullptr;
  }

  VarType getType() const noexcept {
    if(auto lk = getLock(MtxLockType::shared_locked); lk)
      return resource->resource_data.type;
    else return VarType::invalid_type;
  }

  static Link cloneResource(priv::Link resource_link) noexcept {
    switch (toTypeClass(resource_link.getType())) {
    case binom::VarTypeClass::null:
    case binom::VarTypeClass::number:
    return **resource_link;

    case binom::VarTypeClass::bit_array:
    return ResourceData{VarType::bit_array, {.bit_array_header = BitArrayHeader::copy(resource_link->data.bit_array_header)}};

    case binom::VarTypeClass::buffer_array:
    return ResourceData{resource_link.getType(), {.buffer_array_header = BufferArrayHeader::copy(resource_link->data.buffer_array_header)}};

    case binom::VarTypeClass::array: // TODO
    case binom::VarTypeClass::list: // TODO
    case binom::VarTypeClass::map: // TODO
    default:
    case binom::VarTypeClass::invalid_type:
    break;
    }
    return ResourceData{VarType::null, {}};
  }

};

}

#endif // RESOURCE_CONTROL_HXX
