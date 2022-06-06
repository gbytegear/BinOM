#include "libbinom/include/utils/resource_control.hxx"

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
      delete resource_data.data.bit_array_header;
    resource_data.data.pointer = nullptr;
  return;

  case VarTypeClass::buffer_array:
    if(resource_data.data.pointer)
      delete resource_data.data.buffer_array_header;
    resource_data.data.pointer = nullptr;
  return;

  case VarTypeClass::array:
    if(resource_data.data.pointer)
      delete resource_data.data.array_header;
    resource_data.data.pointer = nullptr;
  return;

  case VarTypeClass::singly_linked_list:
    if(resource_data.data.pointer)
      delete resource_data.data.single_linked_list_header;
    resource_data.data.pointer = nullptr;
  return;

  case VarTypeClass::doubly_linked_list:
    if(resource_data.data.pointer)
      delete resource_data.data.doubly_linked_list_header;
    resource_data.data.pointer = nullptr;
  return;

  case VarTypeClass::map: return; // TODO
  case VarTypeClass::table: return; // TODO
  case VarTypeClass::invalid_type: default:
  return;
  }
}

SharedResource::SharedResource(ResourceData resource_data) : resource_data(resource_data) {}

SharedResource::~SharedResource() {destroy();}


//////////////////////////////////////////////////////////// Link ////////////////////////////////////////////////////////



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
    resource->resource_data.data.bit_array_header = BitArrayHeader::copy(resource_data.data.bit_array_header);
  return;

  case VarTypeClass::buffer_array:
    resource->resource_data.data.buffer_array_header = BufferArrayHeader::copy(resource_data.data.buffer_array_header);
  return;

  case VarTypeClass::array:
    resource->resource_data.data.array_header = ArrayHeader::copy(resource_data.data.array_header);
  return;

  case VarTypeClass::singly_linked_list:
    resource->resource_data.data.single_linked_list_header = new SinglyLinkedListHeader(*resource_data.data.single_linked_list_header);
  return;

  case VarTypeClass::doubly_linked_list:
    resource->resource_data.data.doubly_linked_list_header = new DoublyLinkedListHeader(*resource_data.data.doubly_linked_list_header);
  return;

  case VarTypeClass::map: // TODO
  case VarTypeClass::table: // TODO
  default:
  case VarTypeClass::invalid_type:
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
  return ResourceData{VarType::bit_array, {.bit_array_header = BitArrayHeader::copy(resource_link->data.bit_array_header)}};

  case VarTypeClass::buffer_array:
  return ResourceData{resource_link.getType(), {.buffer_array_header = BufferArrayHeader::copy(resource_link->data.buffer_array_header)}};

  case VarTypeClass::array:
  return ResourceData{VarType::array, {.array_header = ArrayHeader::copy(resource_link->data.array_header)}};

  case VarTypeClass::singly_linked_list:
  return ResourceData{VarType::singly_linked_list, {.single_linked_list_header = new SinglyLinkedListHeader(*resource_link->data.single_linked_list_header)}};

  case VarTypeClass::doubly_linked_list:
  return ResourceData{VarType::doubly_linked_list, {.doubly_linked_list_header = new DoublyLinkedListHeader(*resource_link->data.doubly_linked_list_header)}};

  case VarTypeClass::map: // TODO
  case VarTypeClass::table: // TODO
  default:
  case VarTypeClass::invalid_type:
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
