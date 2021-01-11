#include "binom/includes/structure/variables/node_visitor.h"

binom::NodeVisitor::NodeVisitor(binom::Variable* var) : ref_type(RefType::variable), ref(var) {}

binom::NodeVisitor::NodeVisitor(binom::NamedVariable* named_var) : ref_type(RefType::named_variable), ref(named_var) {}

binom::NodeVisitor::NodeVisitor(binom::ValueRef val) : ref_type(RefType::value), ref(val) {}

binom::NodeVisitor::NodeVisitor(const binom::NodeVisitor& other) : ref_type(other.ref_type), ref(other.ref) {}

binom::NodeVisitor& binom::NodeVisitor::operator=(binom::Variable* var) {
  ref_type = RefType::variable;
  ref.variable = var;
  return *this;
}

binom::NodeVisitor& binom::NodeVisitor::operator=(binom::NamedVariable* named_var) {
  ref_type = RefType::named_variable;
  ref.named_variable = named_var;
  return *this;
}

binom::NodeVisitor& binom::NodeVisitor::operator=(binom::ValueRef val) {
  ref_type = RefType::value;
  ref.value = val;
  return *this;
}

binom::NodeVisitor& binom::NodeVisitor::operator=(const binom::NodeVisitor& other) {return *new(this) NodeVisitor(other);}

binom::VarType binom::NodeVisitor::getType() {
  switch (ref_type) {
    case RefType::variable: return ref.variable->type();
    case RefType::named_variable: return ref.named_variable->variable.type();
    case RefType::value: return toVarType(ref.value.getType());
  }
}

binom::NodeVisitor& binom::NodeVisitor::stepInside(binom::ui64 index) {
  if(ref_type == RefType::value) throw SException(ErrCode::binom_invalid_type);
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isArray()) return *this = &var.toArray().getVariable(index);
  elif(var.isBufferArray()) return *this = var.toBufferArray().getValue(index);
  else throw SException(ErrCode::binom_invalid_type);
}

binom::NodeVisitor& binom::NodeVisitor::stepInside(binom::BufferArray name) {
  if(ref_type == RefType::value) throw SException(ErrCode::binom_invalid_type);
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isObject()) return *this = &var.toObject().getNamedVariable(name);
  else throw SException(ErrCode::binom_invalid_type);
}

binom::BufferArray& binom::NodeVisitor::rename(binom::BufferArray name) {
  if(ref_type != RefType::named_variable) throw SException(ErrCode::binom_invalid_type);
  return ref.named_variable->name = name;
}

binom::ValueRef binom::NodeVisitor::getValue() const {
  if(ref_type == RefType::value) return ref.value;
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isPrimitive()) return var.toPrimitive();
  throw SException(ErrCode::binom_invalid_type);
}

binom::ValueRef binom::NodeVisitor::getValue(binom::ui64 index) const {
  if(ref_type == RefType::value) throw SException(ErrCode::binom_invalid_type);
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isBufferArray()) return var.toBufferArray().getValue(index);
  else throw SException(ErrCode::binom_invalid_type);
}

binom::Variable& binom::NodeVisitor::getVariable() const {
  if(ref_type == RefType::value) throw SException(ErrCode::binom_invalid_type);
  return (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
}

binom::Variable& binom::NodeVisitor::getVariable(binom::ui64 index) const {
  if(ref_type == RefType::value) throw SException(ErrCode::binom_invalid_type);
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isArray()) return var.toArray().getVariable(index);
  throw SException(ErrCode::binom_invalid_type);
}

binom::Variable& binom::NodeVisitor::getVariable(binom::BufferArray name) const {
  if(ref_type == RefType::value) throw SException(ErrCode::binom_invalid_type);
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isObject()) return var.toObject().getVariable(name);
  throw SException(ErrCode::binom_invalid_type);
}

binom::NamedVariable& binom::NodeVisitor::getNamedVariable() const {
  if(ref_type != RefType::named_variable) throw SException(ErrCode::binom_invalid_type);
  return *ref.named_variable;
}

binom::NamedVariable& binom::NodeVisitor::getNamedVariable(binom::BufferArray name) const {
  if(ref_type == RefType::value) throw SException(ErrCode::binom_invalid_type);
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isObject()) return var.toObject().getNamedVariable(name);
  throw SException(ErrCode::binom_invalid_type);
}

binom::NodeVisitor::NodeIterator binom::NodeVisitor::begin() {return NodeIterator(*this);}

binom::NodeVisitor::NodeIterator binom::NodeVisitor::end() {return NodeIterator(*this, true);}
