#ifndef RESOURCE_CONTROLL_H
#define RESOURCE_CONTROLL_H

#include "types.hxx"
#include <shared_mutex>
#include <thread>
#include <atomic>
#include <optional>

namespace binom::priv {

class shared_recursive_mutex: public std::shared_mutex {
public:
    void lock(void) {
        std::thread::id this_id = std::this_thread::get_id();
        if(owner == this_id) {
            // recursive locking
            count++;
        } else {
            // normal locking
            shared_mutex::lock();
            owner = this_id;
            count = 1;
        }
    }
    void unlock(void) {
        if(count > 1) {
            // recursive unlocking
            count--;
        } else {
            // normal unlocking
            owner = std::thread::id();
            count = 0;
            shared_mutex::unlock();
        }
    }

private:
    std::atomic<std::thread::id> owner;
    int count;
};

struct Resource {
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

class Link;

struct ResourceBase {
  virtual ~ResourceBase() noexcept {}
  virtual Link linkHard() noexcept = 0;
  virtual Link linkWeak() noexcept = 0;
  virtual void unlink() noexcept = 0;
  virtual ui64 getHardLinkCount() const noexcept = 0;
  virtual std::optional<std::shared_lock<shared_recursive_mutex>> tryLockShared() const noexcept = 0;
  virtual std::optional<std::unique_lock<shared_recursive_mutex>> tryLockUnique() const noexcept = 0;
  virtual Resource* getResource() const noexcept = 0;
  virtual LinkType getType() const noexcept = 0;
};

class SharedResource;

class WeakResourceLink : public ResourceBase {
  friend class SharedResource;
  SharedResource* resource = nullptr;
  WeakResourceLink* next = nullptr;
  WeakResourceLink* prev = nullptr;
  WeakResourceLink(SharedResource* res, WeakResourceLink* prev) noexcept;
  void toNull();
public:

  virtual ~WeakResourceLink() noexcept override;

  virtual Link linkHard() noexcept override;
  virtual void unlink() noexcept override;
  virtual Link linkWeak() noexcept override;
  virtual ui64 getHardLinkCount() const noexcept override;
  virtual std::optional<std::shared_lock<shared_recursive_mutex>> tryLockShared() const noexcept override;
  virtual std::optional<std::unique_lock<shared_recursive_mutex>> tryLockUnique() const noexcept override;
  virtual Resource* getResource() const noexcept override;
  virtual LinkType getType() const noexcept override;

  operator bool () const noexcept;
};

class SharedResource : public ResourceBase {
  std::shared_ptr<shared_recursive_mutex> resource_mutex;
  Resource resource;
  std::atomic<ui64> hard_link_counter = 1;
  WeakResourceLink* weak_first = nullptr;
  WeakResourceLink* weak_last = nullptr;
  friend class WeakResourceLink;
public:
  SharedResource() noexcept : resource_mutex(new shared_recursive_mutex()) {}
  SharedResource(Resource resource) noexcept : resource_mutex(new shared_recursive_mutex()), resource(resource) {}

  virtual ~SharedResource() noexcept override;
  virtual Link linkHard() noexcept override;
  virtual void unlink() noexcept override;
  virtual Link linkWeak() noexcept override;
  virtual ui64 getHardLinkCount() const noexcept override;
  virtual std::optional<std::shared_lock<shared_recursive_mutex>> tryLockShared() const noexcept override;
  virtual std::optional<std::unique_lock<shared_recursive_mutex>> tryLockUnique() const noexcept override;
  virtual Resource* getResource() const noexcept override;
  virtual LinkType getType() const noexcept override;

};

class Link {
  friend class SharedResource;
  friend class WeakResourceLink;
  ResourceBase* resource_pointer = nullptr;
  Link(ResourceBase* resource_pointer) noexcept : resource_pointer(resource_pointer) {}
public:
  Link() = default;
  Link(Resource resource) : resource_pointer(new SharedResource(resource)) {}
  Link(Link&& other) : resource_pointer(other.resource_pointer) {other.resource_pointer = nullptr;}
  Link(Link& other, LinkType type)
    : Link((other.resource_pointer != nullptr)
            ?(
               (type == LinkType::hard_link)
               ? other.resource_pointer->linkHard()
               : other.resource_pointer->linkWeak()
             )
           : std::move(other)) {}

  ~Link() {if(resource_pointer) resource_pointer->unlink(); resource_pointer = nullptr;}

  void changeLink(const Link& link, LinkType type = LinkType::invalid_link) noexcept {
    this->~Link();
    if(link.resource_pointer)
      switch (type) {
      case binom::LinkType::soft_link: new(this) Link(link.resource_pointer->linkWeak()); return;
      case binom::LinkType::hard_link: new(this) Link(link.resource_pointer->linkHard()); return;
      case binom::LinkType::invalid_link: return;
      }
  }

  Resource* operator*() const noexcept {
    if(!resource_pointer) return nullptr;
    return resource_pointer->getResource();
  }

  LinkType getLinkType() const noexcept {
    if(!resource_pointer) return LinkType::invalid_link;
    return resource_pointer->getType();
  }

  VarType getResourceType() const noexcept {
    auto lock = tryLockShared();
    if(!lock) return VarType::null;
    Resource* res = **this;
    if(!res) return VarType::null;
    return res->type;
  }

  bool isResourceLinked() const noexcept {
    if(!resource_pointer) return false;
    switch (resource_pointer->getType()) {
      case binom::LinkType::soft_link: return *dynamic_cast<WeakResourceLink*>(resource_pointer);
      case binom::LinkType::hard_link: return true;
      default:
      case binom::LinkType::invalid_link: return false;
    }
  }

  ui64 getHardLinkCount() const noexcept {
    if(!resource_pointer) return 0;
    return resource_pointer->getHardLinkCount();
  }

  std::optional<std::shared_lock<shared_recursive_mutex>> tryLockShared() const noexcept {
    if(!resource_pointer) return std::optional<std::shared_lock<shared_recursive_mutex>>();
    return resource_pointer->tryLockShared();
  }

  std::optional<std::unique_lock<shared_recursive_mutex>> tryLockUnique() const noexcept {
    if(!resource_pointer) return std::optional<std::unique_lock<shared_recursive_mutex>>();
    return resource_pointer->tryLockUnique();
  }

};

}

#endif // RESOURCE_CONTROLL_H
