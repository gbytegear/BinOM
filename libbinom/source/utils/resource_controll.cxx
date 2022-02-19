#include "libbinom/include/utils/resource_controll.hpp"

using namespace binom;
using namespace binom::priv;

// ------------------------------------------------- WeakResourceLink Implementation

WeakResourceLink::WeakResourceLink(SharedResource* res, WeakResourceLink* prev) noexcept : resource(res), prev(prev) {if(prev)prev->next = this;}

void WeakResourceLink::toNull() {
  resource = nullptr;
  next = nullptr;
  prev = nullptr;
}

WeakResourceLink::~WeakResourceLink() noexcept {
  if(prev) prev->next = next;
  if(next) next->prev = prev;
}

Link WeakResourceLink::linkHard() noexcept {
  auto lk = tryLockShared();
  if(!lk) return nullptr;
  return resource;
}

void WeakResourceLink::unlink() noexcept {
  delete this;
}

Link WeakResourceLink::linkWeak() noexcept {
  auto lk = tryLockShared();
  if(!lk) return nullptr;
  return resource->linkWeak();
}

ui64 WeakResourceLink::getHardLinkCount() const noexcept {
  if(!resource) return 0;
  return resource->hard_link_counter;
}

std::optional<std::shared_lock<std::shared_mutex> > WeakResourceLink::tryLockShared() const noexcept {
  if(resource) if(resource->resource_mutex)
    return std::shared_lock<std::shared_mutex>(*resource->resource_mutex);
  return std::optional<std::shared_lock<std::shared_mutex>>();
}

std::optional<std::unique_lock<std::shared_mutex> > WeakResourceLink::tryLockUnique() const noexcept {
  if(resource) if(resource->resource_mutex)
    return std::unique_lock<std::shared_mutex>(*resource->resource_mutex);
  return std::optional<std::unique_lock<std::shared_mutex>>();
}

Resource* WeakResourceLink::getResource() const noexcept {
  if(resource) return &resource->resource;
  return nullptr;
}

LinkType WeakResourceLink::getType() const noexcept {return LinkType::soft_link;}

binom::priv::WeakResourceLink::operator bool() const noexcept {return resource;}

// ------------------------------------------------- SharedResource Implementation

SharedResource::~SharedResource() noexcept {
  WeakResourceLink* next;
  for(WeakResourceLink* it = weak_first; it != nullptr; it = next) {
    next = it->next;
    it->toNull();
  }
}

Link SharedResource::linkHard() noexcept {++hard_link_counter; return this;}

void SharedResource::unlink() noexcept {
  if(!--hard_link_counter) {
    resource_mutex->lock();
    std::shared_ptr destoryed_resource_mutex = std::move(resource_mutex);
    delete this;
    destoryed_resource_mutex->unlock();
  }
}

Link SharedResource::linkWeak() noexcept {
  std::shared_lock lk(*resource_mutex);
  weak_last = new WeakResourceLink(this, weak_last);
  if(!weak_first) weak_first = weak_last;
  return weak_last;
}

ui64 SharedResource::getHardLinkCount() const noexcept {return hard_link_counter;}

std::optional<std::shared_lock<std::shared_mutex> > SharedResource::tryLockShared() const noexcept {
  if(resource_mutex)
    return std::shared_lock<std::shared_mutex>(*resource_mutex);
  return std::optional<std::shared_lock<std::shared_mutex>>();
}

std::optional<std::unique_lock<std::shared_mutex> > SharedResource::tryLockUnique() const noexcept {
  if(resource_mutex)
    return std::unique_lock<std::shared_mutex>(*resource_mutex);
  return std::optional<std::unique_lock<std::shared_mutex>>();
}

Resource* SharedResource::getResource() const noexcept {
  if(resource_mutex) return const_cast<Resource*>(&resource);
  return nullptr;
}

LinkType SharedResource::getType() const noexcept {return LinkType::hard_link;}
