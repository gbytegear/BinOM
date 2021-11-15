#include "libbinom/include/utils/node_visitor.h"

using namespace binom;

void NodeVisitor::setNull() {
  ref_type = RefType::variable;
  ref.ptr = nullptr;
}

NodeVisitor::NodeVisitor(decltype(nullptr) null) : ref_type(RefType(-1)), ref(*reinterpret_cast<Variable**>(&null)) {}

NodeVisitor::NodeVisitor(Variable* var) : ref_type(RefType::variable), ref(var) {}

NodeVisitor::NodeVisitor(NamedVariable* named_var) : ref_type(RefType::named_variable), ref(named_var) {}

NodeVisitor::NodeVisitor(ValueRef val) : ref_type(RefType::value), ref(val) {}

NodeVisitor::NodeVisitor(const NodeVisitor& other) : ref_type(other.ref_type), ref(other.ref) {}

NodeVisitor::NodeVisitor(NodeVisitor&& other) : ref_type(other.ref_type), ref(other.ref) {}

NodeVisitor& NodeVisitor::operator=(Variable* var) {
  ref_type = RefType::variable;
  ref.variable = var;
  return *this;
}

NodeVisitor& NodeVisitor::operator=(NamedVariable* named_var) {
  ref_type = RefType::named_variable;
  ref.named_variable = named_var;
  return *this;
}

NodeVisitor& NodeVisitor::operator=(ValueRef val) {
  ref_type = RefType::value;
  ref.value = val;
  return *this;
}

NodeVisitor& NodeVisitor::operator=(const NodeVisitor& other) {return *new(this) NodeVisitor(other);}

VarType NodeVisitor::getType() const {
  switch (ref_type) {
    case RefType::variable: return ref.variable->type();
    case RefType::named_variable: return ref.named_variable->variable.type();
    case RefType::value: return toVarType(ref.value.getType());
    default: return VarType::invalid_type;
  }
}

bool NodeVisitor::isNull() const {
  return ref.ptr
      ?(
         ref_type == RefType::variable
         ? ref.variable->isNull()
         :(
             ref_type == RefType::named_variable
             ? ref.named_variable->variable.isNull()
             : false
          )
       )
     : true;
}

ui64 NodeVisitor::getElementCount() const {
  switch (getTypeClass()) {
    case VarTypeClass::primitive:
    return 1;
    case VarTypeClass::buffer_array:
    return getVariable().toBufferArray().getMemberCount();
    case VarTypeClass::array:
    return getVariable().toArray().getMemberCount();
    case VarTypeClass::object:
    return getVariable().toObject().getMemberCount();
    default:
    return 0;
  }
}

NodeVisitor& NodeVisitor::stepInside(ui64 index) {
  if(ref_type == RefType::value) {
    setNull();
    return *this;
  }
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  try {
    if(var.isArray()) return *this = &var.toArray().getVariable(index);
    elif(var.isBufferArray()) return *this = var.toBufferArray().getValue(index);
    else {
      setNull();
      return *this;
    }
  } catch(const Exception&) {
    setNull();
    return *this;
  }
}

NodeVisitor& NodeVisitor::stepInside(BufferArray name) {
  if(ref_type == RefType::value) {
    setNull();
    return *this;
  }
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isObject()) try {
    return *this = &var.toObject().getNamedVariable(name);
  } catch(const Exception&) {
    setNull();
    return *this;
  }
  else {
    setNull();
    return *this;
  }
}

NodeVisitor& NodeVisitor::stepInside(Path path) {
  for(const Path::PathNode& path_node : path)
    if(isNull()) return *this;
    else
      switch (path_node.type()) {
        case PathNodeType::index: stepInside(path_node.index()); continue;
        case PathNodeType::name:  stepInside(path_node.name()); continue;
      }
  return *this;
}

BufferArray& NodeVisitor::rename(BufferArray old_name, BufferArray new_name) {
  if(ref_type == RefType::value) throw Exception(ErrCode::binom_invalid_type);
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isObject()) return var.toObject().rename(std::move(old_name), std::move(new_name));
  else throw Exception(ErrCode::binom_invalid_type);
}

ValueRef NodeVisitor::getValue() const {
  if(ref_type == RefType::value) return ref.value;
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isPrimitive()) return var.toPrimitive();
  throw Exception(ErrCode::binom_invalid_type);
}

ValueRef NodeVisitor::getValue(ui64 index) const {
  if(ref_type == RefType::value) throw Exception(ErrCode::binom_invalid_type);
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isBufferArray()) return var.toBufferArray().getValue(index);
  else throw Exception(ErrCode::binom_invalid_type);
}

Variable& NodeVisitor::getVariable() const {
  if(ref_type == RefType::value) throw Exception(ErrCode::binom_invalid_type);
  return (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
}

Variable& NodeVisitor::getVariable(ui64 index) const {
  if(ref_type == RefType::value) throw Exception(ErrCode::binom_invalid_type);
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isArray()) return var.toArray().getVariable(index);
  throw Exception(ErrCode::binom_invalid_type);
}

Variable& NodeVisitor::getVariable(BufferArray name) const {
  if(ref_type == RefType::value) throw Exception(ErrCode::binom_invalid_type);
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isObject()) return var.toObject().getVariable(name);
  throw Exception(ErrCode::binom_invalid_type);
}

Variable& NodeVisitor::getVariable(Path path) const {
  return NodeVisitor(*this).stepInside(std::move(path)).getVariable();
}

std::optional<BufferArray> NodeVisitor::getName() const {
  if(ref_type == RefType::named_variable) return ref.named_variable->name;
  return std::optional<BufferArray>();
}

bool NodeVisitor::contains(ui64 index) {
  switch (getTypeClass()) {
    case binom::VarTypeClass::primitive: return false;
    case binom::VarTypeClass::buffer_array: return index < getVariable().toBufferArray().getMemberCount();
    case binom::VarTypeClass::array: return index < getVariable().toArray().getMemberCount();
    case binom::VarTypeClass::object: return false;
    default: return false;
  }
}

bool NodeVisitor::contains(BufferArray name) {
  switch (getTypeClass()) {
    case binom::VarTypeClass::primitive: return false;
    case binom::VarTypeClass::buffer_array: return false;
    case binom::VarTypeClass::array: return false;
    case binom::VarTypeClass::object: return getVariable().toObject().contains(std::move(name));
    default: return false;
  }
}

bool NodeVisitor::contains(Path path) {
  NodeVisitor node(*this);
  for(const Path::PathNode& path_node : path)
    if(isNull()) return false;
    else
      switch (path_node.type()) {
        case PathNodeType::index: node.stepInside(path_node.index()); continue;
        case PathNodeType::name:  node.stepInside(path_node.name()); continue;
      }
  return !node.isNull();
}

void NodeVisitor::setVariable(Variable var) {
  switch (ref_type) {
    case binom::NodeVisitor::RefType::variable:
      ref.variable->destroy();
      new(ref.variable) Variable(std::move(var));
    break;
    case binom::NodeVisitor::RefType::named_variable:
      ref.named_variable->variable.destroy();
      new(&ref.named_variable->variable) Variable(std::move(var));
    break;
    case binom::NodeVisitor::RefType::value: throw Exception(ErrCode::binom_invalid_type);
  }
}

void NodeVisitor::setVariable(ui64 index, Variable var) {return contains(index)? getChild(index).setVariable(std::move(var)) : insert(index, std::move(var));}
void NodeVisitor::setVariable(BufferArray name, Variable var) {return contains(name)? getChild(std::move(name)).setVariable(std::move(var)) : insert(std::move(name), std::move(var));}
void NodeVisitor::setVariable(Path path, Variable var) {return getChild(std::move(path)).setVariable(std::move(var));}

void NodeVisitor::pushBack(Variable var) {
  switch (getTypeClass()) {
    default: return;

    case binom::VarTypeClass::buffer_array:
      switch (var.typeClass()) {
        case binom::VarTypeClass::primitive:
          getVariable().toBufferArray().pushBack(var.toPrimitive().getValue());
        break;
        case binom::VarTypeClass::buffer_array:
          getVariable().toBufferArray().pushBack(var.toBufferArray());
        break;
        default: return;
      }
    break;

    case binom::VarTypeClass::array:
      getVariable().toArray().pushBack(std::move(var));
    break;
  }
}

void NodeVisitor::pushFront(Variable var) {
  switch (getTypeClass()) {
    default: return;

    case binom::VarTypeClass::buffer_array:
      switch (var.typeClass()) {
        case binom::VarTypeClass::primitive:
          getVariable().toBufferArray().pushFront(var.toPrimitive().getValue());
        break;
        case binom::VarTypeClass::buffer_array:
          getVariable().toBufferArray().pushFront(var.toBufferArray());
        break;
        default: return;
      }
    break;

    case binom::VarTypeClass::array:
      getVariable().toArray().pushFront(std::move(var));
    break;
  }
}

void NodeVisitor::insert(ui64 index, Variable var) {
  switch (getTypeClass()) {
    default: return;

    case VarTypeClass::buffer_array:
      switch (var.typeClass()) {
        case VarTypeClass::primitive:
          getVariable().toBufferArray().insert(index, var.toPrimitive().getValue());
        break;

        case VarTypeClass::buffer_array:
          getVariable().toBufferArray().insert(index, var.toBufferArray());
        break;
        default: return;
      }
    break;

    case VarTypeClass::array:
      getVariable().toArray().insert(index, std::move(var));
    break;
  }
}

void NodeVisitor::insert(BufferArray name, Variable var) {
  if(getType() != VarType::object) return;
  getVariable().toObject().insert(std::move(name), std::move(var));
}

void NodeVisitor::remove(ui64 index, ui64 count) {
  switch (getTypeClass()) {
    default: return;

    case VarTypeClass::array:
      getVariable().toArray().remove(index, count);
    break;

    case VarTypeClass::buffer_array:
      getVariable().toBufferArray().remove(index, count);
    break;
  }
}

void NodeVisitor::remove(BufferArray name) {
  if(getType() != VarType::object) return;
  getVariable().toObject().remove(std::move(name));
}

void NodeVisitor::remove(Path path) {
  if(Path::iterator it = ++path.begin(), rm_it = path.begin();it == path.end())
    switch (rm_it->type()) {
    case binom::PathNodeType::index: remove(rm_it->index()); return;
    case binom::PathNodeType::name: remove(rm_it->name()); return;
    }

  Path::PathNode last_node = *path.begin();
  NodeVisitor visitor(*this);
  for(Path::iterator it = ++path.begin(), end = path.end(); it != end ; ++it) {
    switch (last_node.type()) {
      case PathNodeType::index: visitor.stepInside(last_node.index()); break;
      case PathNodeType::name:  visitor.stepInside(last_node.name()); break;
    }
    last_node = *it;
  }
  switch (last_node.type()) {
    case binom::PathNodeType::index: visitor.remove(last_node.index()); break;
    case binom::PathNodeType::name: visitor.remove(last_node.name()); break;
  }
}

bool NodeVisitor::test(Query query, ui64 index) noexcept {
    TestExpressionFrame test_expr;
    for(Query::QueryEpression& expr : query) test_expr(expr, *this, index);
    return test_expr.getValue();
}

NodeVector NodeVisitor::findAll(Query query, NodeVector node_vector) {
  if(!isIterable()) return node_vector;
  ui64 index = 0;
  for(NodeVisitor node : *this) {
    if(node.test(query, index))
      node_vector.emplace_back(std::unique_ptr<NodeVisitorBase>(new NodeVisitor(node)));
    ++index;
  }
  return node_vector;
}

NodeVisitor NodeVisitor::find(Query query) {
  if(!isIterable()) return nullptr;
  ui64 index = 0;
  for(NodeVisitor node : *this) {
    if(node.test(query, index))
      return node;
    ++index;
  }
  return nullptr;
}

NodeVisitor::NodeIterator NodeVisitor::begin() {return NodeIterator(*this);}

NodeVisitor::NodeIterator NodeVisitor::end() {return NodeIterator(*this, true);}

NodeVisitor& NodeVisitor::ifNotNull(std::function<void (NodeVisitor&)> callback) {
  if(!isNull())callback(*this);return *this;
}

NodeVisitor& NodeVisitor::ifNull(std::function<void ()> callback) {
  if(isNull())callback();return *this;
}

void NodeVisitor::foreach(std::function<void (NodeVisitor&)> callback) {
  if(!isIterable()) return;
  for(NodeVisitor node : *this) callback(node);
}
