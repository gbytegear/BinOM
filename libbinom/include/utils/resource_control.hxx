#ifndef RESOURCE_CONTROL_HXX
#define RESOURCE_CONTROL_HXX

#include "variable_headers.hxx"

#include "shared_recursive_mutex_wrapper.hxx"
#include <atomic>


namespace binom::priv {
using namespace extended_type_traits;


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

    BitArrayHeader* bit_array_header;
    BufferArrayHeader* buffer_array_header;
    ArrayHeader* array_header;
    SinglyLinkedListHeader* single_linked_list_header;
    DoublyLinkedListHeader* doubly_linked_list_header;

    template<typename T> T* asPointerAt() const noexcept { return reinterpret_cast<T*>(pointer);}
  };

  VarType type = VarType::null;
  Data data;
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

public:
  Link(ResourceData resource_data) noexcept;
  Link(Link&& other) noexcept;
  Link(const Link& other) noexcept;
  ~Link();

  void overwriteWithResourceCopy(ResourceData& resource_data);
  static Link cloneResource(priv::Link resource_link) noexcept;

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept;
  ResourceData* operator*() const noexcept;
  ResourceData* operator->() const noexcept;
  VarType getType() const noexcept;
};

}

#endif // RESOURCE_CONTROL_HXX
