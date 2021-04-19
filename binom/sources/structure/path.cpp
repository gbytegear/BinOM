#include "binom/includes/structure/path.h"

using namespace binom;

//// PathLiteral

Path::PathLiteral::PathLiteral(ui64 index) : type(PathNodeType::index), value(index) {}

Path::PathLiteral::PathLiteral(int index) : type(PathNodeType::index), value(index) {}

Path::PathLiteral::PathLiteral(BufferArray string) : type(PathNodeType::name), value(std::move(string)) {}

Path::PathLiteral::PathLiteral(const char* string) : type(PathNodeType::name), value(BufferArray(string)) {}

Path::PathLiteral::~PathLiteral() {
  if(type == PathNodeType::name && value.index != 0)
    value.string.~BufferArray();
}








Path::Path(std::initializer_list<Path::PathLiteral> path) {
  for(const PathLiteral& path_node : path) {
    switch (path_node.type) {
      case binom::PathNodeType::index:
        data.pushBack<PathNodeType>(PathNodeType::index);
        data.pushBack<ui64>(path_node.value.index);
      break;
      case binom::PathNodeType::name:
        data.pushBack<PathNodeType>(PathNodeType::name);
        data.pushBack<ui64>(path_node.value.string.getMemberCount());
        data.pushBack(path_node.value.string.toByteArray());
      break;
    }
  }
}

Path::Path(const Path& other) : data(other.data) {}

Path::Path(Path&& other) : data(std::move(other.data)) {}

bool Path::isEmpty() {return data.isEmpty();}

Path::iterator Path::begin() {return data.begin<void*>();}

Path::iterator Path::end() {return reinterpret_cast<void*>(data.end());}

Path::PathNode::PathNode(void* ptr) : ptr(reinterpret_cast<PathNodeType*>(ptr)) {}

PathNodeType Path::PathNode::type() const {return *ptr;}

ui64 Path::PathNode::index() const {return *reinterpret_cast<ui64*>(ptr + 1);}

BufferArray Path::PathNode::name() const {
  return BufferArray(
        reinterpret_cast<ui8*>(ptr + 1 + sizeof (ui64)),
        *reinterpret_cast<ui64*>(ptr + 1)
        );
}





Path::iterator::iterator(void* ptr) : ptr(reinterpret_cast<PathNodeType*>(ptr)) {}

Path::iterator::iterator(Path::iterator& other) : ptr(other.ptr) {}

Path::iterator::iterator(Path::iterator&& other) : ptr(other.ptr) {}

Path::PathNode& Path::iterator::operator*() {return *reinterpret_cast<PathNode*>(this);}

Path::PathNode* Path::iterator::operator->() {return reinterpret_cast<PathNode*>(this);}

Path::iterator& Path::iterator::operator++() {
  switch (*ptr) {
    case PathNodeType::index:
      ptr += 1 + sizeof (ui64);
    break;
    case PathNodeType::name:
      ptr += 1 + sizeof(ui64) + *reinterpret_cast<ui64*>(ptr + 1);
    break;
  }
  return *this;
}

Path::iterator Path::iterator::operator++(int) {
  iterator last(*this);
  switch (*ptr) {
    case PathNodeType::index:
      ptr += 1 + sizeof (ui64);
    break;
    case PathNodeType::name:
      ptr += 1 + sizeof(ui64) + *reinterpret_cast<ui64*>(ptr + 1);
    break;
  }
  return last;
}

bool Path::iterator::operator==(Path::iterator other) {return ptr == other.ptr;}

bool Path::iterator::operator!=(Path::iterator other) {return ptr != other.ptr;}
