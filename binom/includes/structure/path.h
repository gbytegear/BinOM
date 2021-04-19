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

  class PathLiteral {
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

  class PathNode {
    PathNodeType* ptr;
  public:
    PathNode(void* ptr);

    PathNodeType type() const;
    ui64 index() const;
    BufferArray name() const;
  };

  class iterator {
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
  };

private:
  ByteArray data;
public:
  Path(std::initializer_list<PathLiteral> path);

  Path(const Path& other);
  Path(Path&& other);

  bool isEmpty();

  iterator begin();
  iterator end();

};

}

#endif // PATH_H
