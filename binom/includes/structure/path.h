#ifndef PATH_H
#define PATH_H

#include "variables/variable.h"

namespace binom {

enum class PathNodeType : ui8 {
  index,
  name
};

class PathNode {
  friend class iterator;

  PathNode* next = nullptr;

  PathNodeType _type;
  union PathNodeValue {
    ui64 index;
    BufferArray name;

    ~PathNodeValue();

    PathNodeValue(ui8 index);
    PathNodeValue(BufferArray name);
    PathNodeValue(PathNodeType type, const PathNodeValue& value);

    PathNodeValue(PathNodeType type, PathNodeValue&& value);

  } value;
public:
  PathNode(ui64 index);
  PathNode(BufferArray name);

  // For literal init
  PathNode(const char* name);
  PathNode(int index);

  PathNode(const PathNode& other);

  PathNode(PathNode&& other);

  PathNode(std::initializer_list<PathNode> path);

  ~PathNode();

  PathNodeType type() const;
  ui8 index() const;
  BufferArray name() const;

  class iterator {
    friend class PathNode;
    PathNode* current;
  public:
    iterator(PathNode* current);
    iterator(iterator& other);
    iterator(iterator&& other);

    PathNode& operator*();
    PathNode* operator->();

    iterator& operator++();
    iterator operator++(int);

    bool operator==(iterator other);
    bool operator!=(iterator other);
  };

  class const_iterator {
    friend class PathNode;
    const PathNode* current;
  public:
    const_iterator(const PathNode* current);
    const_iterator(const_iterator& other);
    const_iterator(const_iterator&& other);

    const PathNode& operator*();
    const PathNode* operator->();

    const_iterator& operator++();
    const_iterator operator++(int);

    bool operator==(const_iterator other);
    bool operator!=(const_iterator other);
  };

  iterator begin();
  iterator end();

  const_iterator cbegin() const;
  const_iterator cend() const;

};

}

#endif // PATH_H
