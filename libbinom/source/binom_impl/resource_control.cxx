#include "libbinom/include/binom_impl/resource_control.hxx"
#include "libbinom/include/binom_impl/ram_storage_implementation.hxx"

using namespace binom;
using namespace binom::priv;

//////////////////////////////////////////////////////////// SharedResource ////////////////////////////////////////////////////////


bool SharedResource::isExist() noexcept {return link_counter;}

void SharedResource::destroy() {
  switch (toTypeClass(resource_data.type)) {
  case VarTypeClass::null:
  case VarTypeClass::number: return;

  case VarTypeClass::bit_array:
    if(resource_data.data.pointer)
      delete resource_data.data.bit_array_implementation;
    resource_data.data.pointer = nullptr;
  return;

  case VarTypeClass::buffer_array:
    if(resource_data.data.pointer)
      delete resource_data.data.buffer_array_implementation;
    resource_data.data.pointer = nullptr;
  return;

  case VarTypeClass::array:
    if(resource_data.data.pointer)
      delete resource_data.data.array_implementation;
    resource_data.data.pointer = nullptr;
  return;

  case VarTypeClass::list:
    if(resource_data.data.pointer)
      delete resource_data.data.list_implementation;
    resource_data.data.pointer = nullptr;
  return;

  case VarTypeClass::map:
    if(resource_data.data.pointer)
      delete resource_data.data.map_implementation;
    resource_data.data.pointer = nullptr;
  return;

  case VarTypeClass::multimap:
    if(resource_data.data.pointer)
      delete resource_data.data.multi_map_implementation;
    resource_data.data.pointer = nullptr;
  return;

  case VarTypeClass::table:
    if(resource_data.data.pointer)
      delete resource_data.data.table_implementation;
    resource_data.data.pointer = nullptr;
  return;
  case VarTypeClass::invalid_type: default:
  return;
  }
}

SharedResource::SharedResource() : resource_data{} {}

SharedResource::SharedResource(ResourceData resource_data) : resource_data(resource_data) {}

SharedResource::~SharedResource() {destroy();}


//////////////////////////////////////////////////////////// Link ////////////////////////////////////////////////////////


Link::Link(SharedResource& shared_resource) noexcept : resource(&shared_resource) {
  if(auto lk = getLock(MtxLockType::shared_locked); lk) {
    ++resource->link_counter;
  } else resource = nullptr;
}

Link::Link(ResourceData resource_data) noexcept : resource(new SharedResource(resource_data)) {}

Link::Link(Link&& other) noexcept : resource(other.resource) {other.resource = nullptr;}

Link::Link(const Link& other) noexcept {
  if(!other.resource) return;
  if(auto lk = other.getLock(MtxLockType::shared_locked); lk) {
    ++other.resource->link_counter;
    resource = other.resource;
  } else return;
}

Link::~Link() {
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

void Link::overwriteWithResourceCopy(ResourceData& resource_data) {
  resource->destroy();
  resource->resource_data.type = resource_data.type;
  switch (toTypeClass(resource_data.type)) {
  case VarTypeClass::null: return;
  case VarTypeClass::number:
    resource->resource_data.data.ui64_val = resource_data.data.ui64_val;
  return;

  case VarTypeClass::bit_array:
    resource->resource_data.data.bit_array_implementation = BitArrayImplementation::copy(resource_data.data.bit_array_implementation);
  return;

  case VarTypeClass::buffer_array:
    resource->resource_data.data.buffer_array_implementation = BufferArrayImplementation::copy(resource_data.data.buffer_array_implementation);
  return;

  case VarTypeClass::array:
    resource->resource_data.data.array_implementation = ArrayImplementation::copy(resource_data.data.array_implementation);
  return;

  case VarTypeClass::list:
    resource->resource_data.data.list_implementation = new ListImplementation(*resource_data.data.list_implementation);
  return;

  case VarTypeClass::map:
    resource->resource_data.data.map_implementation = new MapImplementation(self, *resource_data.data.map_implementation);
  return;

  case VarTypeClass::multimap:
    resource->resource_data.data.multi_map_implementation = new MultiMapImplementation(self, *resource_data.data.multi_map_implementation);
  return;

  case VarTypeClass::table:
    resource->resource_data.data.table_implementation = new TableImplementation(*resource_data.data.table_implementation);
  return;
  case VarTypeClass::invalid_type:
  default:
  break;
  }
  resource->resource_data.type = VarType::null;
}

Link Link::cloneResource(Link resource_link) noexcept {
  switch (toTypeClass(resource_link.getType())) {
  case VarTypeClass::null:
  case VarTypeClass::number:
  return **resource_link;

  case VarTypeClass::bit_array:
  return ResourceData{VarType::bit_array, {.bit_array_implementation = BitArrayImplementation::copy(resource_link->data.bit_array_implementation)}};

  case VarTypeClass::buffer_array:
  return ResourceData{resource_link.getType(), {.buffer_array_implementation = BufferArrayImplementation::copy(resource_link->data.buffer_array_implementation)}};

  case VarTypeClass::array:
  return ResourceData{VarType::array, {.array_implementation = ArrayImplementation::copy(resource_link->data.array_implementation)}};

  case VarTypeClass::list:
  return ResourceData{VarType::list, {.list_implementation = new ListImplementation(*resource_link->data.list_implementation)}};

  case VarTypeClass::map:{
    SharedResource* shared_resource = new SharedResource();
    shared_resource->resource_data.data.map_implementation = new MapImplementation(*shared_resource, *resource_link->data.map_implementation);
    shared_resource->resource_data.type = VarType::map;
    Link link(*shared_resource);
    shared_resource->link_counter = 1;
    return link;
  }

  case VarTypeClass::multimap: {
    SharedResource* shared_resource = new SharedResource();
    shared_resource->resource_data.data.multi_map_implementation = new MultiMapImplementation(*shared_resource, *resource_link->data.multi_map_implementation);
    shared_resource->resource_data.type = VarType::multimap;
    Link link(*shared_resource);
    shared_resource->link_counter = 1;
    return link;
  }

  case VarTypeClass::table:
    return ResourceData{VarType::table, {.table_implementation = new TableImplementation(*resource_link->data.table_implementation)}};
  case VarTypeClass::invalid_type:
  default:
  break;
  }
  return ResourceData{VarType::null, {.pointer = nullptr}};
}

ui64 Link::getLinkCount() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return resource->link_counter;
  else return 0;
}

OptionalSharedRecursiveLock Link::getLock(MtxLockType lock_type) const noexcept {
  if(!resource) return OptionalSharedRecursiveLock();
  if(!resource->isExist()) return OptionalSharedRecursiveLock();
  return OptionalSharedRecursiveLock(SharedRecursiveLock(&resource->mtx, lock_type));
}

ResourceData* Link::operator*() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return &resource->resource_data;
  else return nullptr;
}

ResourceData* Link::operator->() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return &resource->resource_data;
  else return nullptr;
}

VarType Link::getType() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return resource->resource_data.type;
  else return VarType::invalid_type;
}
