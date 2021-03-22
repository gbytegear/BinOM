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

binom::VarType binom::NodeVisitor::getType() const {
  switch (ref_type) {
    case RefType::variable: return ref.variable->type();
    case RefType::named_variable: return ref.named_variable->variable.type();
    case RefType::value: return toVarType(ref.value.getType());
  }
}

binom::ui64 binom::NodeVisitor::getElementCount() const {
  switch (getTypeClass()) {
    case binom::VarTypeClass::primitive:
    return 1;
    case binom::VarTypeClass::buffer_array:
    return getVariable().toBufferArray().getMemberCount();
    case binom::VarTypeClass::array:
    return getVariable().toArray().getMemberCount();
    case binom::VarTypeClass::object:
    return getVariable().toObject().getMemberCount();
    default:
    return 0;
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

binom::NodeVisitor& binom::NodeVisitor::stepInside(binom::PathNode path) {
  for(const PathNode& path_node : path)
    switch (path_node.type()) {
      case PathNodeType::index: stepInside(path_node.index()); continue;
      case PathNodeType::name: stepInside(path_node.name()); continue;
    }
  return *this;
}

binom::BufferArray& binom::NodeVisitor::rename(binom::BufferArray old_name, binom::BufferArray new_name) {
  if(ref_type == RefType::value) throw SException(ErrCode::binom_invalid_type);
  Variable& var = (ref_type == RefType::variable)? *ref.variable : ref.named_variable->variable;
  if(var.isObject()) return var.toObject().rename(std::move(old_name), std::move(new_name));
  else throw SException(ErrCode::binom_invalid_type);
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

binom::Variable& binom::NodeVisitor::getVariable(binom::PathNode path) const {
  return NodeVisitor(*this).stepInside(std::move(path)).getVariable();
}

binom::BufferArray binom::NodeVisitor::getName() const {
  if(ref_type == RefType::named_variable) return ref.named_variable->name;
  throw SException(ErrCode::binom_invalid_type);
}

bool binom::NodeVisitor::test(binom::Query query, ui64 index) {

  auto getRelationResult = [](bool last, QueryNextFieldRelation rel, bool current){
    switch (rel) {
      case binom::QueryNextFieldRelation::AND:
      return last && current;
      case binom::QueryNextFieldRelation::OR:
      return last || current;
      case binom::QueryNextFieldRelation::XOR:
      return last != current;
    }
  };

  auto getName = [](NodeVisitor& node) -> BufferArray {
    try {
      return node.getName();
    }  catch (SException&) {
      return "";
    }
  };

  bool last_group_value = true;
  QueryNextFieldRelation group_rel = QueryNextFieldRelation::AND;

  for(QueryFieldGroup& group : query) {

    bool last_field_value = true;
    QueryNextFieldRelation field_rel = QueryNextFieldRelation::AND;

    for(QueryField& field : group.getFields()) {
      NodeVisitor test_node = (field.isCurrentNodeTest())
                              ? *this
                              : getChild(field.getPath());

      // Fucking node test by field
      switch (field.getPropertyType()) {

        case binom::QueryProperty::type:

          switch (field.getOperatorType()) {
            case binom::QueryOperator::equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   test_node.getType() == field.getVarType()
                                   );
            break;
            case binom::QueryOperator::not_equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   test_node.getType() != field.getVarType()
                                   );
            break;
            default:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   false
                                   );
          }
        break;

        case binom::QueryProperty::type_class:
          switch (field.getOperatorType()) {
            case binom::QueryOperator::equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   test_node.getTypeClass() == field.getVarTypeClass()
                                   );
            break;
            case binom::QueryOperator::not_equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   test_node.getTypeClass() != field.getVarTypeClass()
                                   );
            break;
            default:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   false
                                   );
          }
        break;

        case binom::QueryProperty::value_type:
          switch (field.getOperatorType()) {
            case binom::QueryOperator::equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   toValueType(test_node.getType()) == field.getValType()
                                   );
            break;
            case binom::QueryOperator::not_equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   toValueType(test_node.getType()) != field.getValType()
                                   );
            break;
            default:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   false
                                   );
          }
        break;

        case binom::QueryProperty::element_count:
          switch (field.getOperatorType()) {
            case binom::QueryOperator::equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   test_node.getElementCount() == field.getUNumber()
                                   );
            break;
            case binom::QueryOperator::not_equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   test_node.getElementCount() != field.getUNumber()
                                   );
            break;
            case binom::QueryOperator::highter:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   test_node.getElementCount() > field.getUNumber()
                                   );
            break;
            case binom::QueryOperator::highte_equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   test_node.getElementCount() >= field.getUNumber()
                                   );
            break;
            case binom::QueryOperator::lower:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   test_node.getElementCount() < field.getUNumber()
                                   );
            break;
            case binom::QueryOperator::lower_equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   test_node.getElementCount() <= field.getUNumber()
                                   );
            break;
            case binom::QueryOperator::in_range:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   field.getRange().in(test_node.getElementCount())
                                   );
            break;
            case binom::QueryOperator::out_range:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   field.getRange().out(test_node.getElementCount())
                                   );
            break;
            default:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   false
                                   );

          }

        break;
        case binom::QueryProperty::index:
          switch (field.getOperatorType()) {
            case binom::QueryOperator::equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   index == field.getUNumber()
                                   );
            break;
            case binom::QueryOperator::not_equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   index != field.getUNumber()
                                   );
            break;
            case binom::QueryOperator::highter:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   index > field.getUNumber()
                                   );
            break;
            case binom::QueryOperator::highte_equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   index >= field.getUNumber()
                                   );
            break;
            case binom::QueryOperator::lower:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   index < field.getUNumber()
                                   );
            break;
            case binom::QueryOperator::lower_equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   index <= field.getUNumber()
                                   );
            break;
            case binom::QueryOperator::in_range:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   field.getRange().in(index)
                                   );
            break;
            case binom::QueryOperator::out_range:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   field.getRange().out(index)
                                   );
            break;
            default:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   false
                                   );

          }
        break;
        case binom::QueryProperty::name: {
          BufferArray name = getName(test_node);
          if(!name.getMemberCount()) {
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 false
                                 );
            break;
          }
          switch (field.getOperatorType()) {
            case binom::QueryOperator::equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   name == field.getString()
                                   );
            break;
            case binom::QueryOperator::not_equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   name != field.getString()
                                   );
            break;
            case binom::QueryOperator::highter:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   name > field.getString()
                                   );
            break;
            case binom::QueryOperator::highte_equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   name >= field.getString()
                                   );
            break;
            case binom::QueryOperator::lower:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   name < field.getString()
                                   );
            break;
            case binom::QueryOperator::lower_equal:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   name >= field.getString()
                                   );
            break;
            default:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   false
                                   );
          }
          break;
        }
        case binom::QueryProperty::value:

          switch (test_node.getTypeClass()) {
            case binom::VarTypeClass::primitive:
              switch (field.getOperatorType()) {
                case binom::QueryOperator::equal:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       test_node
                                       .getVariable()
                                       .toPrimitive()
                                       .getValue()
                                       .asSigned() == field.getNumber()

                                       );
                break;
                case binom::QueryOperator::not_equal:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       test_node
                                       .getVariable()
                                       .toPrimitive()
                                       .getValue()
                                       .asSigned() != field.getNumber()

                                       );
                break;
                case binom::QueryOperator::highter:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       test_node
                                       .getVariable()
                                       .toPrimitive()
                                       .getValue()
                                       .asSigned() > field.getNumber()

                                       );
                break;
                case binom::QueryOperator::highte_equal:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       test_node
                                       .getVariable()
                                       .toPrimitive()
                                       .getValue()
                                       .asSigned() >= field.getNumber()

                                       );
                break;
                case binom::QueryOperator::lower:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       test_node
                                       .getVariable()
                                       .toPrimitive()
                                       .getValue()
                                       .asSigned() < field.getNumber()

                                       );
                break;
                case binom::QueryOperator::lower_equal:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       test_node
                                       .getVariable()
                                       .toPrimitive()
                                       .getValue()
                                       .asSigned() <= field.getNumber()

                                       );
                break;
                case binom::QueryOperator::in_range:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       field.getRange().in(
                                         test_node
                                         .getVariable()
                                         .toPrimitive()
                                         .getValue()
                                         .asSigned()
                                         )

                                       );
                break;
                case binom::QueryOperator::out_range:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       field.getRange().out(
                                         test_node
                                         .getVariable()
                                         .toPrimitive()
                                         .getValue()
                                         .asSigned()
                                         )

                                       );
                break;
                default:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,
                                       false
                                       );
              }

            break;
            case binom::VarTypeClass::buffer_array:
              if(toValueType(test_node.getType()) != ValType::byte)
                last_field_value = getRelationResult(
                                     last_field_value,
                                     field_rel,
                                     false
                                     );
              switch (field.getOperatorType()) {
                case binom::QueryOperator::equal:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       test_node
                                       .getVariable()
                                       .toBufferArray() == field.getString()

                                       );
                break;
                case binom::QueryOperator::not_equal:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       test_node
                                       .getVariable()
                                       .toBufferArray() != field.getString()

                                       );
                break;
                case binom::QueryOperator::highter:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       test_node
                                       .getVariable()
                                       .toBufferArray() > field.getString()

                                       );
                break;
                case binom::QueryOperator::highte_equal:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       test_node
                                       .getVariable()
                                       .toBufferArray() >= field.getString()

                                       );
                break;
                case binom::QueryOperator::lower:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       test_node
                                       .getVariable()
                                       .toBufferArray() < field.getString()

                                       );
                break;
                case binom::QueryOperator::lower_equal:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,

                                       test_node
                                       .getVariable()
                                       .toBufferArray() >= field.getString()

                                       );
                break;
                default:
                  last_field_value = getRelationResult(
                                       last_field_value,
                                       field_rel,
                                       false
                                       );
              }
            break;
            default:
              last_field_value = getRelationResult(
                                   last_field_value,
                                   field_rel,
                                   false
                                   );
          }
        break;
      }

      field_rel = field.getNextRealation();

    }

    last_group_value = getRelationResult(last_group_value, group_rel, last_field_value);
    group_rel = group.getNextRealation();
  }
  return last_group_value;
}

binom::NodeVector binom::NodeVisitor::find(binom::Query query) {
  NodeVector node_vector;
  ui64 index = 0;
  for(NodeVisitor node : *this) {
    if(node.test(query, index))
      node_vector.push_back(node);
    ++index;
  }
  return node_vector;
}

binom::NodeVisitor::NodeIterator binom::NodeVisitor::begin() {return NodeIterator(*this);}

binom::NodeVisitor::NodeIterator binom::NodeVisitor::end() {return NodeIterator(*this, true);}
