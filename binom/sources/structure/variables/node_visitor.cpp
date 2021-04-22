#include "binom/includes/structure/variables/node_visitor.h"

using namespace binom;

NodeVisitor::NodeVisitor(decltype(nullptr) null) : ref_type(RefType::variable), ref(*reinterpret_cast<Variable**>(&null)) {}

NodeVisitor::NodeVisitor(Variable* var) : ref_type(RefType::variable), ref(var) {}

NodeVisitor::NodeVisitor(NamedVariable* named_var) : ref_type(RefType::named_variable), ref(named_var) {}

NodeVisitor::NodeVisitor(ValueRef val) : ref_type(RefType::value), ref(val) {}

NodeVisitor::NodeVisitor(const NodeVisitor& other) : ref_type(other.ref_type), ref(other.ref) {}

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
    default: return VarType::invlid_type;
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
  if(ref_type == RefType::value) throw Exception(ErrCode::binom_invalid_type);
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isArray()) return *this = &var.toArray().getVariable(index);
  elif(var.isBufferArray()) return *this = var.toBufferArray().getValue(index);
  else throw Exception(ErrCode::binom_invalid_type);
}

NodeVisitor& NodeVisitor::stepInside(BufferArray name) {
  if(ref_type == RefType::value) throw Exception(ErrCode::binom_invalid_type);
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isObject()) return *this = &var.toObject().getNamedVariable(name);
  else throw Exception(ErrCode::binom_invalid_type);
}

NodeVisitor& NodeVisitor::stepInside(Path path) {
  for(const Path::PathNode& path_node : path)
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

BufferArray NodeVisitor::getName() const {
  if(ref_type == RefType::named_variable) return ref.named_variable->name;
  throw Exception(ErrCode::binom_invalid_type);
}

void NodeVisitor::setVariable(Variable var) {
  if(ref_type == RefType::value)
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

// Query test function
#include "binom/includes/structure/variables/node_visitor_query.h"

NodeVector NodeVisitor::findAll(Query query, NodeVector node_vector) {
  if(!isIterable()) return node_vector;
  ui64 index = 0;
  for(NodeVisitor node : *this) {
    if(node.test(query, index))
      node_vector.push_back(node);
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
