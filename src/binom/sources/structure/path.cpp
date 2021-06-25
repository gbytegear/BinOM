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


bool Path::isValid() {
  iterator it_end = end();
  for(iterator it = begin(); it != it_end; ++it) {
    if(it > it_end) return false;
    PathNode& path_node = *it;
    PathNodeType type = path_node.type();
    if( type != PathNodeType::index && type != PathNodeType::name ) return false;
  }
  return true;
}

Path::Path(ByteArray data) : data(std::move(data)) {if(!isValid())throw Exception(ErrCode::invalid_data, "Invalid path buffer!");}

Path::Path(Path&& other) : data(std::move(other.data)) {}
Path::Path(const Path& other) : data(other.data) {}

Path::Path() {}

bool Path::isEmpty() const {return data.isEmpty();}

bool Path::operator==(const Path& other) const {
  iterator
      this_it = begin(),
      other_it = other.begin(),
      this_end = end(),
      other_end = other.end();
  for(;this_it != this_end && other_it != other_end;(++this_it, ++other_it)) {
    if(this_it->type() != other_it->type()) return false;
    switch (this_it->type()) {
      case binom::PathNodeType::index:
        if(this_it->index() != other_it->index()) return false;
      break;
      case binom::PathNodeType::name:
        if(this_it->name() != other_it->name()) return false;
      break;
    }
  }
  return this_it == this_end && other_it == other_end;
}

//Path& Path::pushBack(Path::PathLiteral path_literal) {
//  switch (path_literal.type) {
//    case binom::PathNodeType::index:
//      data.pushBack<PathNodeType>(PathNodeType::index);
//      data.pushBack<ui64>(path_literal.value.index);
//    break;
//    case binom::PathNodeType::name:
//      data.pushBack<PathNodeType>(PathNodeType::name);
//      data.pushBack<ui64>(path_literal.value.string.getMemberCount());
//      data.pushBack(path_literal.value.string.toByteArray());
//    break;
//  }
//  return *this;
//}

Path& Path::pushBack(BufferArray name) {
  data.pushBack<PathNodeType>(PathNodeType::name);
  data.pushBack<ui64>(name.getMemberCount());
  data.pushBack(name.toByteArray());
  return *this;
}

Path& Path::pushBack(ui64 index) {
  data.pushBack<PathNodeType>(PathNodeType::index);
  data.pushBack<ui64>(index);
  return *this;
}

Path::iterator Path::begin() const {return data.begin<void*>();}

Path::iterator Path::end() const {return reinterpret_cast<void*>(data.end());}

ByteArray Path::toByteArray() const {return data;}

Path Path::fromByteArray(ByteArray path) {return path;}

// name_1.name_2[10].name_3
Path Path::fromString(std::string str) {
  ByteArray data;
  size_t start = 0;
  for(size_t i = 0; i < str.length(); ++i) {
    switch (str[i]) {
      default: continue;
      case '.': // name.
        if(start == i) continue; // name_1...name_2 (many dots)
        data.pushBack<PathNodeType>(PathNodeType::name);
        data.pushBack<ui64>(i - start);
        data.pushBack(ByteArray(str.substr(start, data.last<ui64>()).c_str(), data.last<ui64>()));
        start = i + 1;
      continue;
      case '[': // [5]
        if(start < i) { // name[5]
          data.pushBack<PathNodeType>(PathNodeType::name);
          data.pushBack<ui64>(i - start);
          data.pushBack(ByteArray(str.substr(start, data.last<ui64>()).c_str(), data.last<ui64>()));
        }
        start = i + 1;
        for(;i < str.length() && str[i] != ']'; ++i)
          if(str[i] < '0' && str[i] > '9') throw Exception(ErrCode::invalid_data);
        data.pushBack<PathNodeType>(PathNodeType::index);
        data.pushBack<ui64>(std::stoull(str.substr(start, i - start)));
        start = i + 1;
      continue;
    }
  }


  if(start < str.length() - 1) {
    data.pushBack<PathNodeType>(PathNodeType::name);
    data.pushBack<ui64>(str.length() - start);
    data.pushBack(ByteArray(str.substr(start, data.last<ui64>()).c_str(), data.last<ui64>()));
  }

//  std::clog << "Parsed path:\n";
//  for(PathNode node : Path::fromByteArray(data)) {
//    std::clog << "Path node type: "<< ((node.type() == PathNodeType::index) ? "index" : "name")
//              << "\nPath node value: " << ((node.type() == PathNodeType::index) ? std::to_string(node.index()) : node.name().toString()) << "\n\n";
//  }

  return Path::fromByteArray(data);
}

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

bool Path::iterator::operator>(Path::iterator other) {return ptr > other.ptr;}

bool Path::iterator::operator>=(Path::iterator other) {return ptr >= other.ptr;}

bool Path::iterator::operator<(Path::iterator other) {return ptr < other.ptr;}

bool Path::iterator::operator<=(Path::iterator other) {return ptr <= other.ptr;}

Path binom::operator""_pth(const char* str, unsigned long long) {return Path::fromString(str);}
