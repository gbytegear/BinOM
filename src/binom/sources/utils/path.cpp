#include "binom/includes/utils/path.h"

using namespace binom;

//// PathLiteral

Path::PathLiteral::PathLiteral(ui64 index) : type(PathNodeType::index), value(index) {}

Path::PathLiteral::PathLiteral(int index) : type(PathNodeType::index), value(index) {}

Path::PathLiteral::PathLiteral(ui8arr byte_array) : type(PathNodeType::name), value(BufferArray(byte_array)) {}
Path::PathLiteral::PathLiteral(i8arr byte_array) : type(PathNodeType::name), value(BufferArray(byte_array)) {}
Path::PathLiteral::PathLiteral(ui16arr word_array) : type(PathNodeType::name), value(BufferArray(word_array)) {}
Path::PathLiteral::PathLiteral(i16arr word_array) : type(PathNodeType::name), value(BufferArray(word_array)) {}
Path::PathLiteral::PathLiteral(ui32arr dword_array) : type(PathNodeType::name), value(BufferArray(dword_array)) {}
Path::PathLiteral::PathLiteral(i32arr dword_array) : type(PathNodeType::name), value(BufferArray(dword_array)) {}
Path::PathLiteral::PathLiteral(ui64arr qword_array) : type(PathNodeType::name), value(BufferArray(qword_array)) {}
Path::PathLiteral::PathLiteral(i64arr qword_array) : type(PathNodeType::name), value(BufferArray(qword_array)) {}


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
        data.pushBack<ValType>(path_node.value.string.getValType());
        data.pushBack<ui64>(path_node.value.string.getMemberCount()*path_node.value.string.getMemberSize());
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

Path& Path::pushBack(BufferArray name) {
  data.pushBack<PathNodeType>(PathNodeType::name);
  data.pushBack<ValType>(name.getValType());
  data.pushBack<ui64>(name.getMemberCount()*name.getMemberSize());
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

std::string Path::toString() const {
  std::string str;
  bool last_is_name = false;
  for(PathNode node : *this) {
    switch (node.type()) {
      case binom::PathNodeType::index:
        str += "[" + std::to_string(node.index()) + "]";
        last_is_name = false;
      break;
      case binom::PathNodeType::name: {
        BufferArray data = node.name();
        str += std::string("{") + toTypeString(data.getValType()) + ":";
        for(auto start = data.begin(), it = start, end = data.end(); it != end ;++it) {
          if(it != start) str += ',';
          str += std::to_string(it->asUi64());
        }
        str += '}';
      } break;
      default: throw Exception(ErrCode::invalid_data, "Invalid path data!");
    }
  }
  return str;
}

Path Path::fromByteArray(ByteArray path) {return path;}

// name_1.name_2[10].name_3
Path Path::fromString(std::string str) {
  ByteArray data;
  size_t start = 0;
  bool shielding = false;
  for(size_t i = 0; i < str.length(); ++i) {
    switch (str[i]) {
      default: continue;
      case '"':
        if(shielding) {
          shielding = false;
          data.pushBack<PathNodeType>(PathNodeType::name);
          data.pushBack<ValType>(ValType::byte);
          data.pushBack<ui64>(i - start);
          data.pushBack(ByteArray(str.substr(start, data.last<ui64>()).c_str(), data.last<ui64>()));
          start = i + 1;
          continue;
        }
        if(start < i) { // name"shielded.name[5]"
          data.pushBack<PathNodeType>(PathNodeType::name);
          data.pushBack<ValType>(ValType::byte);
          data.pushBack<ui64>(i - start);
          data.pushBack(ByteArray(str.substr(start, data.last<ui64>()).c_str(), data.last<ui64>()));
        }
        start = i + 1;
        shielding = true;
      continue;

      case '.': // name.
        if(shielding) continue;
        if(start == i) continue; // name_1...name_2 (many dots)
        data.pushBack<PathNodeType>(PathNodeType::name);
        data.pushBack<ValType>(ValType::byte);
        data.pushBack<ui64>(i - start);
        data.pushBack(ByteArray(str.substr(start, data.last<ui64>()).c_str(), data.last<ui64>()));
        start = i + 1;
      continue;

      case '{': { // {byte: 1, 2, 3} or {word: 1, 2, 3}
        if(shielding) continue;
        if(start < i) { // name{5}
          data.pushBack<PathNodeType>(PathNodeType::name);
          data.pushBack<ValType>(ValType::byte);
          data.pushBack<ui64>(i - start);
          data.pushBack(ByteArray(str.substr(start, data.last<ui64>()).c_str(), data.last<ui64>()));
        }

        start = i + 1;
        for(;i < str.length() && str[i] != ':'; ++i);
        if(i >= str.length()) throw Exception(ErrCode::invalid_data);
        ValType type = toValueType(str.substr(start, i - start).c_str());
        if(type == ValType::invalid_type) throw Exception(ErrCode::invalid_data);
        ++i;


        ByteArray data_value;
        {
          BufferArray data(type);
          std::string value;
          for(;i < str.length() && str[i] != '}'; ++i)
            switch (str[i]) {
              default:
                if(str[i] < '0' && str[i] > '9') throw Exception(ErrCode::invalid_data);
                else value += str[i];
              continue;
              case ' ': case '\t': case '\n': case '\r': case '\x0B': case '\f':
              continue;
              case ',':
                if(value.empty()) continue;
                data.pushBack(std::stoull(value));
                value.clear();
              continue;
            }
          if(!value.empty()) data.pushBack(std::stoull(value));
          if(i >= str.length()) throw Exception(ErrCode::invalid_data);
          data_value = data.toByteArray();
        }

        data.pushBack<PathNodeType>(PathNodeType::name);
        data.pushBack<ValType>(type);
        data.pushBack<ui64>(data_value.length());
        data.pushBack(data_value);
        start = i + 1;

      }continue;

      case '[': // [5]
        if(shielding) continue;
        if(start < i) { // name[5]
          data.pushBack<PathNodeType>(PathNodeType::name);
          data.pushBack<ValType>(ValType::byte);
          data.pushBack<ui64>(i - start);
          data.pushBack(ByteArray(str.substr(start, data.last<ui64>()).c_str(), data.last<ui64>()));
        }
        start = i + 1;
        for(;i < str.length() && str[i] != ']'; ++i)
          if(str[i] < '0' && str[i] > '9') throw Exception(ErrCode::invalid_data);
        if(i >= str.length()) throw Exception(ErrCode::invalid_data);
        data.pushBack<PathNodeType>(PathNodeType::index);
        data.pushBack<ui64>(std::stoull(str.substr(start, i - start)));
        start = i + 1;
      continue;
    }
  }


  if(start < str.length() - 1) {
    data.pushBack<PathNodeType>(PathNodeType::name);
    data.pushBack<ValType>(ValType::byte);
    data.pushBack<ui64>(str.length() - start);
    data.pushBack(ByteArray(str.substr(start, data.last<ui64>()).c_str(), data.last<ui64>()));
  }

  return Path::fromByteArray(data);
}

Path::PathNode::PathNode(void* ptr) : ptr(reinterpret_cast<PathNodeType*>(ptr)) {}

PathNodeType Path::PathNode::type() const {return *ptr;}

ui64 Path::PathNode::index() const {return *reinterpret_cast<ui64*>(ptr + 1);}

BufferArray Path::PathNode::name() const {
//  return BufferArray(
//        reinterpret_cast<ui8*>(ptr + 1 + sizeof (ui64)),
//        *reinterpret_cast<ui64*>(ptr + 1)
//        );
  return BufferArray(*reinterpret_cast<ValType*>(ptr + sizeof (PathNodeType)),
                     reinterpret_cast<const void*>(ptr + sizeof (PathNodeType) + sizeof (ValType) + sizeof (ui64)),
                     *reinterpret_cast<ui64*>(ptr + sizeof (PathNodeType) + sizeof (ValType))/toSize(*reinterpret_cast<ValType*>(ptr + sizeof (PathNodeType))));
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
      ptr += sizeof (PathNodeType)
             + sizeof(ValType)
             + sizeof(ui64)
             + *reinterpret_cast<ui64*>(ptr + sizeof(PathNodeType) + sizeof(ValType));// * toSize(*reinterpret_cast<ValType*>(ptr + 1 + sizeof(ui64)));
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

//Path binom::operator""_pth(const char* str, size_t) {return Path::fromString(str);}
