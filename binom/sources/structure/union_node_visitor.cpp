#include "binom/includes/structure/union_node_visitor.h"

using namespace binom;

UnionNodeVisitor::Visitor::Visitor(Variable* var) : ram_visitor(new NodeVisitor(var)) {}
UnionNodeVisitor::Visitor::Visitor(NodeVisitor& visitor) : ram_visitor(new NodeVisitor(visitor)) {}
UnionNodeVisitor::Visitor::Visitor(DBNodeVisitor& visitor) : db_visitor(new DBNodeVisitor(visitor)) {}
UnionNodeVisitor::Visitor::Visitor(UnionNodeVisitor::Visitor&& other) : ram_visitor(other.ram_visitor) {other.ram_visitor = nullptr;}

UnionNodeVisitor::Visitor::Visitor(UnionNodeVisitor::Visitor& other, VisitorType type) {
  switch (type) {
    case binom::VisitorType::undefied: return;
    case binom::VisitorType::RAM: ram_visitor = new NodeVisitor(*other.ram_visitor); return;
    case binom::VisitorType::DB: db_visitor = new DBNodeVisitor(*other.db_visitor); return;
  }
}


UnionNodeVisitor::UnionNodeVisitor(const NodeVisitor& other) : type(VisitorType::RAM), visitor(const_cast<NodeVisitor&>(other)) {}
UnionNodeVisitor::UnionNodeVisitor(NodeVisitor&& other) : type(VisitorType::RAM), visitor(other) {}
UnionNodeVisitor::UnionNodeVisitor(const DBNodeVisitor& other) : type(VisitorType::DB), visitor(const_cast<DBNodeVisitor&>(other)) {}
UnionNodeVisitor::UnionNodeVisitor(DBNodeVisitor&& other) : type(VisitorType::DB), visitor(other) {}
UnionNodeVisitor::UnionNodeVisitor(Variable* var) : type(VisitorType::RAM), visitor(var) {}
UnionNodeVisitor::UnionNodeVisitor(UnionNodeVisitor&& other) : type(other.type), visitor(std::move(other.visitor)) {other.visitor.ram_visitor = nullptr; other.type = VisitorType::undefied;}
UnionNodeVisitor::UnionNodeVisitor(const UnionNodeVisitor& other) : type(other.type), visitor(const_cast<UnionNodeVisitor&>(other).visitor, other.type) {}

UnionNodeVisitor::~UnionNodeVisitor() {
  if(visitor.ram_visitor)
    switch (type) {
      case binom::VisitorType::undefied: return;
      case binom::VisitorType::RAM: delete visitor.ram_visitor; return;
      case binom::VisitorType::DB: delete visitor.db_visitor; return;
    }
}



UnionNodeVisitor& UnionNodeVisitor::operator=(Variable* var) {
  this->~UnionNodeVisitor();
  return *new(this) UnionNodeVisitor(var);
}

UnionNodeVisitor& UnionNodeVisitor::operator=(const NodeVisitor& other) {
  this->~UnionNodeVisitor();
  return *new(this) UnionNodeVisitor(other);
}

UnionNodeVisitor& UnionNodeVisitor::operator=(NodeVisitor&& other) {
  this->~UnionNodeVisitor();
  return *new(this) UnionNodeVisitor(std::move(other));
}

UnionNodeVisitor& UnionNodeVisitor::operator=(const DBNodeVisitor& other) {
  this->~UnionNodeVisitor();
  return *new(this) UnionNodeVisitor(other);
}

UnionNodeVisitor& UnionNodeVisitor::operator=(DBNodeVisitor&& other) {
  this->~UnionNodeVisitor();
  return *new(this) UnionNodeVisitor(std::move(other));
}

UnionNodeVisitor& UnionNodeVisitor::operator=(const UnionNodeVisitor& other) {
  this->~UnionNodeVisitor();
  return *new(this) UnionNodeVisitor(other);
}

UnionNodeVisitor& UnionNodeVisitor::operator=(UnionNodeVisitor&& other) {
  this->~UnionNodeVisitor();
  return *new(this) UnionNodeVisitor(std::move(other));
}

UnionNodeVisitor& UnionNodeVisitor::operator=(ValueRef val) {
  this->~UnionNodeVisitor();
  return *new(this) UnionNodeVisitor(val);
}

UnionNodeVisitor& UnionNodeVisitor::operator=(NamedVariable* named_var) {
  this->~UnionNodeVisitor();
  return *new(this) UnionNodeVisitor(named_var);
}

VisitorType UnionNodeVisitor::getVisitorType() const {return type;}

NodeVisitor& UnionNodeVisitor::toRAMVistor() const {if(type != VisitorType::RAM) throw Exception(ErrCode::binom_invalid_visitor); return *visitor.ram_visitor;}
DBNodeVisitor& UnionNodeVisitor::toDBVisitor() const {if(type != VisitorType::DB) throw Exception(ErrCode::binom_invalid_visitor); return *visitor.db_visitor;}

VarType UnionNodeVisitor::getType() const {
  switch (type) {
  default:
  case binom::VisitorType::undefied: return VarType::invalid_type;
  case binom::VisitorType::RAM:
    return visitor.ram_visitor->getType();
    case binom::VisitorType::DB:
    return visitor.db_visitor->getType();
  }
}

VarTypeClass UnionNodeVisitor::getTypeClass() const {
  switch (type) {
    case binom::VisitorType::undefied: return VarTypeClass::invalid_type;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->getTypeClass();
    case binom::VisitorType::DB:
    return visitor.db_visitor->getTypeClass();
  }
}

bool UnionNodeVisitor::isNull() const {
  switch (type) {
    case binom::VisitorType::undefied: return true;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->isNull();
    case binom::VisitorType::DB:
    return visitor.db_visitor->isNull();
  }
}

bool UnionNodeVisitor::isInvalid() const {
  switch (type) {
    case binom::VisitorType::undefied: return true;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->isInvalid();
    case binom::VisitorType::DB:
    return visitor.db_visitor->isInvalid();
  }
}

bool UnionNodeVisitor::isPrimitive() const {
  switch (type) {
    case binom::VisitorType::undefied: return false;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->isPrimitive();
    case binom::VisitorType::DB:
    return visitor.db_visitor->isPrimitive();
  }
}

bool UnionNodeVisitor::isBufferArray() const {
  switch (type) {
    case binom::VisitorType::undefied: return false;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->isBufferArray();
    case binom::VisitorType::DB:
    return visitor.db_visitor->isBufferArray();
  }
}

bool UnionNodeVisitor::isArray() const {
  switch (type) {
    case binom::VisitorType::undefied: return false;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->isArray();
    case binom::VisitorType::DB:
    return visitor.db_visitor->isArray();
  }
}

bool UnionNodeVisitor::isObject() const {
  switch (type) {
    case binom::VisitorType::undefied: return false;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->isObject();
    case binom::VisitorType::DB:
    return visitor.db_visitor->isObject();
  }
}

bool UnionNodeVisitor::isIterable() const {
  switch (type) {
    case binom::VisitorType::undefied: return false;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->isIterable();
    case binom::VisitorType::DB:
    return visitor.db_visitor->isIterable();
  }
}

ui64 UnionNodeVisitor::getElementCount() const {
  switch (type) {
    case binom::VisitorType::undefied: return 0;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->getElementCount();
    case binom::VisitorType::DB:
    return visitor.db_visitor->getElementCount();
  }
}

UnionNodeVisitor& UnionNodeVisitor::stepInside(ui64 index) {
  switch (type) {
    case binom::VisitorType::undefied: break;
    case binom::VisitorType::RAM:
      visitor.ram_visitor->stepInside(index);
    break;
    case binom::VisitorType::DB:
      visitor.db_visitor->stepInside(index);
    break;
  }
  return *this;
}

UnionNodeVisitor& UnionNodeVisitor::stepInside(BufferArray name) {
  switch (type) {
    case binom::VisitorType::undefied: break;
    case binom::VisitorType::RAM:
      visitor.ram_visitor->stepInside(std::move(name));
    break;
    case binom::VisitorType::DB:
      visitor.db_visitor->stepInside(std::move(name));
    break;
  }
  return *this;
}

UnionNodeVisitor& UnionNodeVisitor::stepInside(Path path) {
  switch (type) {
    case binom::VisitorType::undefied: break;
    case binom::VisitorType::RAM:
      visitor.ram_visitor->stepInside(std::move(path));
    break;
    case binom::VisitorType::DB:
      visitor.db_visitor->stepInside(std::move(path));
    break;
  }
  return *this;
}

Variable UnionNodeVisitor::getVariable() const {
  switch (type) {
    case binom::VisitorType::undefied: return nullptr;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->getVariable();
    case binom::VisitorType::DB:
    return visitor.db_visitor->getVariable();
  }
}

Variable UnionNodeVisitor::getVariable(ui64 index) const {
  switch (type) {
    case binom::VisitorType::undefied: return nullptr;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->getVariable(index);
    case binom::VisitorType::DB:
    return visitor.db_visitor->getVariable(index);
  }
}

Variable UnionNodeVisitor::getVariable(BufferArray name) const {
  switch (type) {
    case binom::VisitorType::undefied: return nullptr;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->getVariable(std::move(name));
    case binom::VisitorType::DB:
    return visitor.db_visitor->getVariable(std::move(name));
  }
}

Variable UnionNodeVisitor::getVariable(Path path) const {
  switch (type) {
    case binom::VisitorType::undefied: return nullptr;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->getVariable(std::move(path));
    case binom::VisitorType::DB:
    return visitor.db_visitor->getVariable(std::move(path));
  }
}

void UnionNodeVisitor::setVariable(Variable var) {
  switch (type) {
    case binom::VisitorType::undefied: return;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->setVariable(std::move(var));
    case binom::VisitorType::DB:
    return visitor.db_visitor->setVariable(std::move(var));
  }
}

void UnionNodeVisitor::pushBack(Variable var) {
  switch (type) {
    case binom::VisitorType::undefied: return;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->pushBack(std::move(var));
    case binom::VisitorType::DB:
    return visitor.db_visitor->pushBack(std::move(var));
  }
}

void UnionNodeVisitor::pushFront(Variable var) {
  switch (type) {
    case binom::VisitorType::undefied: return;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->pushFront(std::move(var));
    case binom::VisitorType::DB:
    return visitor.db_visitor->pushFront(std::move(var));
  }
}

void UnionNodeVisitor::insert(ui64 index, Variable var) {
  switch (type) {
    case binom::VisitorType::undefied: return;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->insert(index, std::move(var));
    case binom::VisitorType::DB:
    return visitor.db_visitor->insert(index, std::move(var));
  }
}

void UnionNodeVisitor::insert(BufferArray name, Variable var) {
  switch (type) {
    case binom::VisitorType::undefied: return;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->insert(std::move(name), std::move(var));
    case binom::VisitorType::DB:
    return visitor.db_visitor->insert(std::move(name), std::move(var));
  }
}

void UnionNodeVisitor::remove(ui64 index, ui64 count) {
  switch (type) {
    case binom::VisitorType::undefied: return;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->remove(index, count);
    case binom::VisitorType::DB:
    return visitor.db_visitor->remove(index, count);
  }
}

void UnionNodeVisitor::remove(BufferArray name) {
  switch (type) {
    case binom::VisitorType::undefied: return;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->remove(std::move(name));
    case binom::VisitorType::DB:
    return visitor.db_visitor->remove(std::move(name));
  }
}

void UnionNodeVisitor::remove(Path path) {
  switch (type) {
    case binom::VisitorType::undefied: return;
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->remove(std::move(path));
    case binom::VisitorType::DB:
    return visitor.db_visitor->remove(std::move(path));
  }
}

UnionNodeVisitor UnionNodeVisitor::getChild(ui64 index) const {return UnionNodeVisitor(*this).stepInside(index);}
UnionNodeVisitor UnionNodeVisitor::getChild(BufferArray name) const {return UnionNodeVisitor(*this).stepInside(std::move(name));}
UnionNodeVisitor UnionNodeVisitor::getChild(Path path) const {return UnionNodeVisitor(*this).stepInside(std::move(path));}

UnionNodeVisitor UnionNodeVisitor::find(Query query) {
  switch (type) {
    case binom::VisitorType::RAM:
    return visitor.ram_visitor->find(std::move(query));
    case binom::VisitorType::DB:
    return visitor.db_visitor->find(std::move(query));
    default: return UnionNodeVisitor();
  }
}

UnionNodeVisitor& UnionNodeVisitor::ifNotNull(std::function<void (UnionNodeVisitor&)> callback) {
  if(!isNull())callback(*this);return *this;
}

UnionNodeVisitor& UnionNodeVisitor::ifNull(std::function<void ()> callback) {
  if(isNull())callback();return *this;
}

void UnionNodeVisitor::foreach(std::function<void (UnionNodeVisitor)> callback) {
  if(isIterable()) return;
  switch (type) {
    case binom::VisitorType::undefied: return;
    case binom::VisitorType::RAM:
      for(NodeVisitor node : *visitor.ram_visitor) callback(node);
    case binom::VisitorType::DB:
      for(DBNodeVisitor node : *visitor.db_visitor) callback(node);
  }
}

Variable UnionNodeVisitor::getInfo() {
  Object obj = {
    {"visitor_type", ui8(type)}
  };

  switch (type) {
  case binom::VisitorType::undefied:
  break;
  case binom::VisitorType::RAM:
    obj.insert("pos", ui64(visitor.ram_visitor->ref.ptr));
  break;
  case binom::VisitorType::DB:
    obj.insert("pos", ui64(visitor.db_visitor->getNodeIndex()));
  break;
  }
  return obj.asVar();
}
