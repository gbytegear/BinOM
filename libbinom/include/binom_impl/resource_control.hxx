#ifndef RESOURCE_CONTROL_HXX
#define RESOURCE_CONTROL_HXX

#include "types.hxx"

#include "../utils/shared_recursive_mutex_wrapper.hxx"

#include <atomic>

namespace binom::priv {

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

    BitArrayImplementation* bit_array_implementation;
    BufferArrayImplementation* buffer_array_implementation;
    ArrayImplementation* array_implementation;
    SinglyLinkedListImplementation* single_linked_list_implementation;
    DoublyLinkedListImplementation* doubly_linked_list_implementation;
    MapImplementation* map_implementation;
    MultiMapImplementation* multi_map_implementation;

    template<typename T> T* asPointerAt() const noexcept { return reinterpret_cast<T*>(pointer);}
  };

  VarType type = VarType::null;
  Data data;
};

// File storage resource
struct FileResourceIndex {
  // Union of binom::file_storage::<TypeClass>Implementation (inherit from Base<TypeClass>Implementation)
  union Index {/*TODO*/};
  VarType type = VarType::null;
  Index index;
};

struct SharedResource {
  ResourceData resource_data;

  std::atomic_uint64_t link_counter = 1;
  std::shared_mutex mtx;

  bool isExist() noexcept;
  void destroy();

  SharedResource(ResourceData resource_data);
  ~SharedResource();
};

class Link {
  SharedResource* resource = nullptr;

  friend class binom::Variable;
  std::shared_mutex& getMutex() {return resource->mtx;}

public:
  Link(ResourceData resource_data) noexcept;
  Link(Link&& other) noexcept;
  Link(const Link& other) noexcept;
  ~Link();

  void overwriteWithResourceCopy(ResourceData& resource_data);
  static Link cloneResource(priv::Link resource_link) noexcept;
  ui64 getLinkCount() const noexcept;

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept;
  ResourceData* operator*() const noexcept;
  ResourceData* operator->() const noexcept;
  VarType getType() const noexcept;
};

}

#endif // RESOURCE_CONTROL_HXX
