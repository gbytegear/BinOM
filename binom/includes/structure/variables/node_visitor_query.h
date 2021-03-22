#pragma once

#include "node_visitor.h"

bool binom::NodeVisitor::test(binom::Query query, ui64 index) noexcept {

  static const auto getRelationResult = [](bool last, QueryNextFieldRelation rel, bool current){
    switch (rel) {
      case binom::QueryNextFieldRelation::AND:
      return last && current;
      case binom::QueryNextFieldRelation::OR:
      return last || current;
      case binom::QueryNextFieldRelation::XOR:
      return last != current;
    }
  };

  static const auto getName = [](NodeVisitor& node) -> BufferArray {
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


      //======================== Test functions ================================

      static auto invalidTest = [&]() {
        last_field_value = getRelationResult(
                             last_field_value,
                             field_rel,
                             false
                             );
      };


      static auto testUNumber = [&](ui64 node_number, QueryField& field) {
        if(field.getValueType() != QueryFieldValueType::number &&
           field.getValueType() != QueryFieldValueType::range) {
          invalidTest();
          return;
        }
        switch (field.getOperatorType()) {
          case binom::QueryOperator::equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_number == field.getUNumber()
                                 );
          return;
          case binom::QueryOperator::not_equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_number != field.getUNumber()
                                 );
          return;
          case binom::QueryOperator::highter:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_number > field.getUNumber()
                                 );
          return;
          case binom::QueryOperator::highte_equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_number >= field.getUNumber()
                                 );
          return;
          case binom::QueryOperator::lower:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_number < field.getUNumber()
                                 );
          return;
          case binom::QueryOperator::lower_equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_number <= field.getUNumber()
                                 );
          return;
          case binom::QueryOperator::in_range:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 field.getRange().in(node_number)
                                 );
          return;
          case binom::QueryOperator::out_range:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 field.getRange().out(node_number)
                                 );
          return;
          default: invalidTest(); return;
        }
      };

      static auto testNumber = [&](i64 node_number, QueryField& field) {
        if(field.getValueType() != QueryFieldValueType::number &&
           field.getValueType() != QueryFieldValueType::range) {
          invalidTest();
          return;
        }
        switch (field.getOperatorType()) {
          case binom::QueryOperator::equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_number == field.getNumber()
                                 );
          return;
          case binom::QueryOperator::not_equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_number != field.getNumber()
                                 );
          return;
          case binom::QueryOperator::highter:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_number > field.getNumber()
                                 );
          return;
          case binom::QueryOperator::highte_equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_number >= field.getNumber()
                                 );
          return;
          case binom::QueryOperator::lower:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_number < field.getNumber()
                                 );
          return;
          case binom::QueryOperator::lower_equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_number <= field.getNumber()
                                 );
          return;
          case binom::QueryOperator::in_range:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 field.getRange().in(node_number)
                                 );
          return;
          case binom::QueryOperator::out_range:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 field.getRange().out(node_number)
                                 );
          return;
          default: invalidTest(); return;
        }
      };


      static auto testString = [&](BufferArray node_string, QueryField& field) {
        if(field.getValueType() != QueryFieldValueType::string) {
          invalidTest();
          return;
        }
        switch (field.getOperatorType()) {
          case binom::QueryOperator::equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_string == field.getString()
                                 );
          return;
          case binom::QueryOperator::not_equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_string != field.getString()
                                 );
          return;
          case binom::QueryOperator::highter:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_string > field.getString()
                                 );
          return;
          case binom::QueryOperator::highte_equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_string >= field.getString()
                                 );
          return;
          case binom::QueryOperator::lower:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_string < field.getString()
                                 );
          return;
          case binom::QueryOperator::lower_equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 node_string >= field.getString()
                                 );
          return;
          default: invalidTest(); return;
        }
      };

      //=========================== Node test ==================================

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
            default: invalidTest(); break;
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
            default: invalidTest(); break;
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
            default: invalidTest(); break;
          }
        break;





        case binom::QueryProperty::element_count:
          testUNumber(test_node.getElementCount(), field);
        break;




        case binom::QueryProperty::index:
          testUNumber(index, field);
        break;





        case binom::QueryProperty::name: {
          BufferArray name = getName(test_node);
          if(!name.getMemberCount()) {
            invalidTest();
            break;
          }
          testString(name, field);
          break;
        }




        case binom::QueryProperty::value:

          switch (test_node.getTypeClass()) {

            case binom::VarTypeClass::primitive:
              testNumber(test_node
                         .getVariable()
                         .toPrimitive()
                         .getValue()
                         .asSigned(), field);
            break;

            case binom::VarTypeClass::buffer_array:
              if(toValueType(test_node.getType()) != ValType::byte)
                last_field_value = getRelationResult(
                                     last_field_value,
                                     field_rel,
                                     false
                                     );
              testString(test_node.getVariable().toBufferArray(), field);
            break;

            default: invalidTest(); break;
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
