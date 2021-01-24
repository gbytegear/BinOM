#ifndef DBFILE_H
#define DBFILE_H

#include "vfmem.h"
#include "f_node_visitor.h"
#include "../variables/variable.h"

/*

Array:
[ArrayDescriptor]
[AOElementDescriptor 1]
[AOElementDescriptor 2]
...
[AOElementDescriptor n]

Object:
[ObjectDescriptor]
[...NameLengthBlock...]
[...NameBlock...]
[AOElementDescriptor 1]
[AOElementDescriptor 2]
...
[AOElementDescriptor n]

*/



namespace binom {


class Path {
  struct PathNode {
    enum Type {
      name,
      number
    };

    union Value {
      char* name;
      ui64 number;
      Value(char* name) : name(name) {}
      Value(ui64 number) : number(number) {}
    };

    Type type;
    Value value;
    PathNode* next = nullptr;

    PathNode(char* name) : type(Type::name), value(name) {}
    PathNode(ui64 number) : type(Type::number), value(number) {}
    PathNode(const PathNode& other) : type(other.type), value(other.value), next(other.next) {}
    PathNode(const PathNode&& other) : type(other.type), value(other.value), next(other.next) {}
  };

  PathNode first;
  PathNode* last = &first;

public:

  class iterator {
    PathNode* current;
  public:
    iterator(PathNode* node) : current(node) {}
    iterator(iterator& other) : current(other.current) {}
    iterator(iterator&& other) : current(other.current){}

    iterator& operator++() {++current; return *this;}
    iterator operator++(int) {iterator prev(*this); ++current; return prev;}
    iterator& operator--() {--current; return *this;}
    iterator operator--(int) {iterator next(*this); --current; return next;}
    iterator& operator=(iterator& other) {current = other.current; return *this;}
    iterator& operator=(iterator&& other) {current = other.current; return *this;}
    iterator& operator=(PathNode* ptr) {current = ptr; return *this;}
    bool operator==(iterator& other) {return current == other.current;}
    bool operator!=(iterator& other) {return current != other.current;}
    PathNode& operator*() {return *current;}
    PathNode* operator->() {return current;}
  };

  Path(char* name) : first(name) {}
  Path(ui64 number) : first(number) {}
  Path(Path& other) : first(other.first) {}
  Path(Path&& other) : first(other.first) {other.first.next = nullptr; other.first.value.name = nullptr;}
  Path(std::initializer_list<PathNode> node_list) : first(*node_list.begin()) {
    std::initializer_list<PathNode>::iterator it = node_list.begin();
    ++it;
    for(; it != node_list.end(); ++it)
      pushBack(*it);
  }
  ~Path() {
    for(PathNode& node : *this)
      if(node.type == PathNode::name) free(node.value.name);
  }

  PathNode& pushBack(char* name) {return *(last = last->next = new PathNode(name));}
  PathNode& pushBack(ui64 number) {return *(last = last->next = new PathNode(number));}
  PathNode& pushBack(const PathNode& node) {return *(last = last->next = new PathNode(node));}
  PathNode& pushBack(const PathNode&& node) {return *(last = last->next = new PathNode(node));}

  iterator begin() {return &first;}
  iterator end() {return nullptr;}

};



class DBFile {
  VFMemoryController vmemory;

  ByteArray convert(Primitive primitive);
  ByteArray convert(BufferArray buffer);
  ByteArray convert(Array array);
  ByteArray convert(Object object);

  NodeDescriptor writePrimitive(Primitive primitive);
  NodeDescriptor writeData(ByteArray data);
  ui64 writeNode(NodeDescriptor descriptor);

public:
  DBFile(std::string filename) : vmemory(std::move(filename)) {}
  DBFile(const char* filename) : vmemory(filename) {}

  inline ui64 getFileSize() {return vmemory.getFileSize();}
  inline ui64 getDataSegmentsSize() {return vmemory.getDataSegmentsSize();}
  inline ui64 getNodeSegmentsSize() {return vmemory.getNodeSegmentsSize();}
  inline ui64 getPrimitiveSegmentsSize() {return vmemory.getPrimitiveSegmentsSize();}

  inline ui64 getDataSegmentsCount() {return vmemory.getDataSegmentsCount();}
  inline ui64 getNodeSegmentsCount() {return vmemory.getNodeSegmentsCount();}
  inline ui64 getNodeCount() {return vmemory.getNodeCount();}
  inline ui64 getPrimitiveSegmentsCount() {return vmemory.getPrimitiveSegmentsCount();}

  VFMemoryController& getMemoryCntroller() {return vmemory;}

  FileNodeVisitor getRootNode() {return FileNodeVisitor(vmemory);}

};

}

#endif // DBFILE_H
