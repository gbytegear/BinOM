#ifndef RESOURCE_CONTROL_HXX
#define RESOURCE_CONTROL_HXX

#include "shared_recursive_mutex_wrapper.hxx"
#include <atomic>

// NOTE: Remove soft link mechanic

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

#endif // RESOURCE_CONTROL_HXX
