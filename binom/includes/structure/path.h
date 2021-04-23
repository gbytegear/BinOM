#ifndef PATH_H
#define PATH_H

#include "variables/variable.h"

namespace binom {

enum class PathNodeType : ui8 {
  index,
  name
};

class Path {
public:

  class PathLiteral;
  class PathNode;
  class iterator;

private:
  ByteArray data;
  bool isValid();
  Path(ByteArray data);
public:
  Path(std::initializer_list<PathLiteral> path);
  Path();

  Path(const Path& other);
  Path(Path&& other);

  bool isEmpty() const;

  iterator begin();
  iterator end();

  ByteArray toByteArray() const;
  static Path fromByteArray(ByteArray path);

};

class Path::PathLiteral{
  PathNodeType type;
  union PathLiteralValue {
    ui64 index;
    BufferArray string;
    PathLiteralValue(ui64 index) : index(index) {}
    PathLiteralValue(const BufferArray& string) : string(std::move(string)) {}
    PathLiteralValue(BufferArray&& string) : string(std::move(string)) {}
    ~PathLiteralValue() {}
  } value;
  friend class Path;
public:
  PathLiteral(ui64 index);
  PathLiteral(int index);
  PathLiteral(BufferArray string);
  PathLiteral(const char* string);
  PathLiteral(const PathLiteral& other) = delete;
  PathLiteral(PathLiteral&& other) = delete;
  ~PathLiteral();
};

class Path::PathNode  {
  PathNodeType* ptr;
public:
  PathNode(void* ptr);

  PathNodeType type() const;
  ui64 index() const;
  BufferArray name() const;
};

class Path::iterator {
  friend class Path;
  PathNodeType* ptr;
public:
  iterator(void* ptr);
  iterator(iterator& other);
  iterator(iterator&& other);

  PathNode& operator*();
  PathNode* operator->();

  iterator& operator++();

  iterator operator++(int);

  bool operator==(iterator other);
  bool operator!=(iterator other);
  bool operator>(iterator other);
  bool operator>=(iterator other);
  bool operator<(iterator other);
  bool operator<=(iterator other);
};

}

#endif // PATH_H
