#ifndef FILE_NODE_VISITOR_H
#define FILE_NODE_VISITOR_H

#include "file_memory_manager.h"
#include "../utils/node_visitor_base.h"

namespace binom {

class FileNodeVisitor : public NodeVisitorBase {
  FileMemoryManager& fmm;
  virtual_index node_index = 0;
  real_index index = 0;
  bool is_element_ref = false;

public:

  FileNodeVisitor(FileMemoryManager& fmm,
           virtual_index node_index = 0,
           real_index index = 0xFFFFFFFFFFFFFFFF)
    : fmm(fmm),
      node_index(node_index),
      index(index),
      is_element_ref(index != 0xFFFFFFFFFFFFFFFF)
  {}

  FileNodeVisitor(FileNodeVisitor& other)
    : fmm(other.fmm),
      node_index(other.node_index),
      index(other.index),
      is_element_ref(other.is_element_ref)
  {}

  FileNodeVisitor& operator=(FileNodeVisitor& other);
  FileNodeVisitor& operator=(virtual_index node_index);

  VarType getType() const override;
  VisitorType getVisitorType() const override;
  inline virtual_index getNodeIndex() const {return node_index;}

  bool isNull() const override;
  bool isValueRef() const override;
  ui64 getElementCount() const override;

  FileNodeVisitor& stepInside(ui64 index) override;
  FileNodeVisitor& stepInside(BufferArray name) override;
  FileNodeVisitor& stepInside(Path path) override;

  Variable getVariable() const;
  Variable getVariable(ui64 index) const;
  Variable getVariable(BufferArray name) const;
  Variable getVariable(Path path) const;

  void setVariable(Variable var) override;
  void pushBack(Variable var) override;
  void pushFront(Variable var) override;
  void insert(ui64 index, Variable var) override;
  void insert(BufferArray name, Variable var) override;
  void remove(ui64 index, ui64 count) override;
  void remove(BufferArray name) override;
  void remove(Path path) override;

  FileNodeVisitor& operator()(ui64 index) override;
  FileNodeVisitor& operator()(BufferArray name) override;
  FileNodeVisitor& operator()(Path path) override;

  inline FileNodeVisitor operator[](ui64 index) {return FileNodeVisitor(*this)(index);}
  inline FileNodeVisitor operator[](BufferArray name) {return FileNodeVisitor(*this)(name);}
  inline FileNodeVisitor operator[](Path path) {return FileNodeVisitor(*this)(path);}

  NodeVisitor& toRAMVisitor() = delete;
  FileNodeVisitor& toFileVisitor() = delete;
};

}


#endif // FILE_NODE_VISITOR_H
