#ifndef NODE_MANAGER_HPP
#define NODE_MANAGER_HPP

#include "types.hpp"
#include "page_vector.hpp"
#include <shared_mutex>
#include <atomic>
#include <set>

#define elif else if

namespace binom::priv {

typedef ui64 node_index;
typedef std::vector<node_index> LinkList;

class NodeDescriptor;
class NodeManager;

class NodeDescriptorIndex {
  node_index index;

  friend class NodeManager;
  NodeDescriptorIndex(node_index index) : index(index) {}
public:
  inline operator ui64 () const noexcept {return index;}
  inline NodeDescriptor& operator*() const noexcept;
  inline NodeDescriptor* operator->() const noexcept;
};

struct Data {
  union DataUnion {
    byte* data_ptr = nullptr;
    bool bool_val;
    ui8 ui8_val;
    ui16 ui16_val;
    ui32 ui32_val;
    ui64 ui64_val;
    i8 i8_val;
    i16 i16_val;
    i32 i32_val;
    i64 i64_val;
    f32 f32_val;
    f64 f64_val;
  };

  VarType type = VarType::null;
  DataUnion data_union;
};

class SharedResource {
  friend class NodeDescriptor;
  static constexpr ui16 MAX_HARD_LINK_COUNT = 0xFFFF;
  Data data;
  std::shared_ptr<std::shared_mutex> data_mtx = std::make_shared<std::shared_mutex>();
  std::set<NodeDescriptor*> soft_link_set;
  std::atomic<ui16> hard_link_counter = 1;
public:

  void addLink(NodeDescriptor* node_ptr) noexcept;

  void subLink(NodeDescriptor* node_ptr) noexcept;

  SharedResource() noexcept = default;

  SharedResource(bool value)  noexcept : data{ .type = VarType::boolean,  .data_union{ .bool_val  = value} } {}
  SharedResource(ui8 value)   noexcept : data{ .type = VarType::ui8,      .data_union{ .ui8_val   = value} } {}
  SharedResource(i8 value)    noexcept : data{ .type = VarType::si8,      .data_union{ .i8_val    = value} } {}
  SharedResource(ui16 value)  noexcept : data{ .type = VarType::ui16,     .data_union{ .ui16_val  = value} } {}
  SharedResource(i16 value)   noexcept : data{ .type = VarType::si16,     .data_union{ .i16_val   = value} } {}
  SharedResource(ui32 value)  noexcept : data{ .type = VarType::ui32,     .data_union{ .ui32_val  = value} } {}
  SharedResource(i32 value)   noexcept : data{ .type = VarType::si32,     .data_union{ .i32_val   = value} } {}
  SharedResource(f32 value)   noexcept : data{ .type = VarType::f32,      .data_union{ .f32_val   = value} } {}
  SharedResource(ui64 value)  noexcept : data{ .type = VarType::ui64,     .data_union{ .ui64_val  = value} } {}
  SharedResource(i64 value)   noexcept : data{ .type = VarType::si64,     .data_union{ .i64_val   = value} } {}
  SharedResource(f64 value)   noexcept : data{ .type = VarType::f64,      .data_union{ .f64_val   = value} } {}

  ~SharedResource() noexcept;

};

class NodeDescriptor {
  friend class SharedResource;
  LinkType link_type = LinkType::hard_link;
  SharedResource* data_descriptor = nullptr;
  NodeDescriptor* parent = nullptr;

  SharedResource* getSharedResource() noexcept {
    if(!data_descriptor) return data_descriptor = new SharedResource;
    return data_descriptor;
  }

  void setNull() {data_descriptor = nullptr;}

public:

  // Create hard/soft link node
  NodeDescriptor(NodeDescriptor* parent, NodeDescriptor* link_to, LinkType link_type) noexcept
    : link_type(link_type),
      data_descriptor(link_to->getSharedResource()),
      parent(parent) {data_descriptor->addLink(this);}

  // Create number node
  NodeDescriptor(NodeDescriptor* parent, bool value)  noexcept : data_descriptor(new SharedResource(value)), parent(parent) {}
  NodeDescriptor(NodeDescriptor* parent, ui8 value)   noexcept : data_descriptor(new SharedResource(value)), parent(parent) {}
  NodeDescriptor(NodeDescriptor* parent, i8 value)    noexcept : data_descriptor(new SharedResource(value)), parent(parent) {}
  NodeDescriptor(NodeDescriptor* parent, ui16 value)  noexcept : data_descriptor(new SharedResource(value)), parent(parent) {}
  NodeDescriptor(NodeDescriptor* parent, i16 value)   noexcept : data_descriptor(new SharedResource(value)), parent(parent) {}
  NodeDescriptor(NodeDescriptor* parent, ui32 value)  noexcept : data_descriptor(new SharedResource(value)), parent(parent) {}
  NodeDescriptor(NodeDescriptor* parent, i32 value)   noexcept : data_descriptor(new SharedResource(value)), parent(parent) {}
  NodeDescriptor(NodeDescriptor* parent, f32 value)   noexcept : data_descriptor(new SharedResource(value)), parent(parent) {}
  NodeDescriptor(NodeDescriptor* parent, ui64 value)  noexcept : data_descriptor(new SharedResource(value)), parent(parent) {}
  NodeDescriptor(NodeDescriptor* parent, i64 value)   noexcept : data_descriptor(new SharedResource(value)), parent(parent) {}
  NodeDescriptor(NodeDescriptor* parent, f64 value)   noexcept : data_descriptor(new SharedResource(value)), parent(parent) {}

  ~NodeDescriptor() {data_descriptor->subLink(this);}

  NodeDescriptor* getParent() const noexcept { return parent; }

  LinkType getLinkType() const noexcept { return link_type; }

  VarType getType() const noexcept {
    if(!data_descriptor) return VarType::null;
    return data_descriptor->data.type;
  }

  Data getData() const noexcept {
    if(!data_descriptor) return Data{};
    return data_descriptor->data;
  }

};

inline void SharedResource::addLink(NodeDescriptor* node_ptr) noexcept {
  switch (node_ptr->getLinkType()) {
    case binom::LinkType::hard_link: ++hard_link_counter; break;
    case binom::LinkType::soft_link: {
      std::unique_lock lk(data_mtx);
      soft_link_set.emplace(node_ptr);
    } break;
  }
}

inline void SharedResource::subLink(NodeDescriptor* node_ptr) noexcept {
  switch (node_ptr->getLinkType()) {
    case binom::LinkType::hard_link:
      --hard_link_counter;
      if(!hard_link_counter) {
        data_mtx->lock();
        std::shared_ptr mtx = data_mtx;
        data_mtx.reset();
        delete this;
        mtx->unlock();
      }
    break;
    case binom::LinkType::soft_link: {
      std::unique_lock lk(*data_mtx);
      soft_link_set.erase(node_ptr);
    } break;
  }
}

SharedResource::~SharedResource() noexcept {
  for(NodeDescriptor* soft_link : soft_link_set) soft_link->setNull();
}



class NodeManager {
  static inline PageVector<NodeDescriptor> node_page_vector;
  static inline PageVector<SharedResource> data_descriptor_vector;
public:

  template<class... Args>
  static NodeDescriptorIndex allocNode(Args&&... args) noexcept {
    return node_page_vector.emplace(std::forward<Args>(args)...).key;
  }

  static NodeDescriptor& getDescriptor(ui64 index) noexcept {return node_page_vector[index];}

};

inline NodeDescriptor& NodeDescriptorIndex::operator*() const noexcept {return NodeManager::getDescriptor(index);}
inline NodeDescriptor* NodeDescriptorIndex::operator->() const noexcept {return &NodeManager::getDescriptor(index);}

}

#undef elif

#endif // NODE_MANAGER_HPP
