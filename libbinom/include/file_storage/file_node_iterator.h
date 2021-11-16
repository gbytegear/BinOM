#ifndef FILE_NODE_ITERATOR_H
#define FILE_NODE_ITERATOR_H

#include "file_node_visitor.h"

namespace binom {

class FileNodeVisitor::NodeIterator {
  ui64 index = 0;

  ObjectDescriptor obj_descriptor = {};
  real_index name_block_index = 0;
  real_index name_shift = 0;
  real_index name_index = 0;
  ObjectNameLength name_block_descriptor = {};

  VarType container_type;
  FileNodeVisitor& node_visitor;

  friend class FileNodeVisitor;
  NodeIterator(FileNodeVisitor& node_visitor, bool is_end = false);
  NodeIterator(FileNodeVisitor& node_visitor, ObjectElementPosition pos);
  NodeIterator(FileNodeVisitor& node_visitor, ui64 index);

public:
  NodeIterator(const NodeIterator& other);
  NodeIterator(const NodeIterator&& other);

  inline bool isBegin() { return !index; }
  bool isEnd();

  NodeIterator& operator--();
  NodeIterator& operator++();

  inline NodeIterator& operator++(int) {return ++*this;}
  std::optional<BufferArray> getName();
  bool operator==(NodeIterator iterator) const;
  bool operator!=(NodeIterator iterator) const;
  inline FileNodeVisitor getParent() {return node_visitor;}
  FileNodeVisitor operator*();


};

}

#endif // FILE_NODE_ITERATOR_H
