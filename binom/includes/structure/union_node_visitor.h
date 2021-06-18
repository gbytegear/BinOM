#ifndef UNION_NODE_VISITOR_H
#define UNION_NODE_VISITOR_H

#include "variables/node_visitor.h"
#include "file_storage/data_base_node_visitor.h"

namespace binom {

enum class VisitorType : ui8 {
  undefied,
  RAM,
  DB
};

class UnionNodeVisitor {
  VisitorType type = VisitorType::undefied;
  union Visitor {
    NodeVisitor* ram_visitor = nullptr;
    DBNodeVisitor* db_visitor;
    Visitor() = default;
    Visitor(Variable* var);
    Visitor(NodeVisitor& visitor);
    Visitor(DBNodeVisitor& visitor);
    Visitor(Visitor& other, VisitorType type);
    Visitor(Visitor&& other);
  } visitor;
public:
  UnionNodeVisitor() = default;
  UnionNodeVisitor(const NodeVisitor& other);
  UnionNodeVisitor(NodeVisitor&& other);
  UnionNodeVisitor(const DBNodeVisitor& other);
  UnionNodeVisitor(DBNodeVisitor&& other);
  UnionNodeVisitor(Variable* var);
  UnionNodeVisitor(const UnionNodeVisitor& other);
  UnionNodeVisitor(UnionNodeVisitor&& other);
  ~UnionNodeVisitor();

  UnionNodeVisitor& operator=(const NodeVisitor& other);
  UnionNodeVisitor& operator=(NodeVisitor&& other);
  UnionNodeVisitor& operator=(const DBNodeVisitor& other);
  UnionNodeVisitor& operator=(DBNodeVisitor&& other);
  UnionNodeVisitor& operator=(Variable* var);
  UnionNodeVisitor& operator=(const UnionNodeVisitor& other);
  UnionNodeVisitor& operator=(UnionNodeVisitor&& other);
  UnionNodeVisitor& operator=(NamedVariable* named_var);
  UnionNodeVisitor& operator=(ValueRef val);

  VisitorType getVisitorType() const;

  NodeVisitor& toRAMVistor() const;
  DBNodeVisitor& toDBVisitor() const;

  VarType getType() const;
  VarTypeClass getTypeClass() const;

  bool isNull() const;
  bool isInvalid() const;
  bool isPrimitive() const;
  bool isBufferArray() const;
  bool isArray() const;
  bool isObject() const;
  bool isIterable() const;

  ui64 getElementCount() const;

  UnionNodeVisitor& stepInside(ui64 index);
  UnionNodeVisitor& stepInside(BufferArray name);
  UnionNodeVisitor& stepInside(Path path);

  Variable getVariable() const;
  Variable getVariable(ui64 index) const;
  Variable getVariable(BufferArray name) const;
  Variable getVariable(Path path) const;

  void setVariable(Variable var);
  void pushBack(Variable var);
  void pushFront(Variable var);
  void insert(ui64 index, Variable var);
  void insert(BufferArray name, Variable var);
  void remove(ui64 index, ui64 count = 1);
  void remove(BufferArray name);

  UnionNodeVisitor getChild(ui64 index) const;
  UnionNodeVisitor getChild(BufferArray name) const;
  UnionNodeVisitor getChild(Path path) const;

  UnionNodeVisitor operator[](ui64 index) const {return getChild(index);}
  UnionNodeVisitor operator[](BufferArray name) const {return getChild(std::move(name));}
  UnionNodeVisitor operator[](Path path) const {return getChild(std::move(path));}

  UnionNodeVisitor& operator()(ui64 index) {return stepInside(index);}
  UnionNodeVisitor& operator()(BufferArray name) {return stepInside(std::move(name));}
  UnionNodeVisitor& operator()(Path path) {return stepInside(std::move(path));}

  UnionNodeVisitor find(Query query);

  // Functional

  UnionNodeVisitor& ifNotNull(std::function<void (UnionNodeVisitor&)> callback);
  UnionNodeVisitor& ifNull(std::function<void()> callback);
  void foreach(std::function<void(UnionNodeVisitor)> callback);

  // Other

  Variable getInfo();
};

}

#endif // UNION_NODE_VISITOR_H
