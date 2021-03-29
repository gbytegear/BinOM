#ifndef DATA_BASE_NODE_VISITOR_QUERY_H
#define DATA_BASE_NODE_VISITOR_QUERY_H

#include "data_base_node_visitor.h"

using namespace binom;

bool DBNodeVisitor::test(Query &query, ui64 index, BufferArray name) {

  static const auto getRelationResult = [](bool last, QueryNextFieldRelation rel, bool current){
    switch (rel) {
      case QueryNextFieldRelation::AND:
      return last && current;
      case QueryNextFieldRelation::OR:
      return last || current;
      case QueryNextFieldRelation::XOR:
      return last != current;
    }
  };

  bool last_group_value = true;
  QueryNextFieldRelation group_rel = QueryNextFieldRelation::AND;

  for(QueryFieldGroup& group : query) {

    bool last_field_value = true;
    QueryNextFieldRelation field_rel = QueryNextFieldRelation::AND;

    for(QueryField& field : group.getFields()) {
      DBNodeVisitor test_node = (field.isCurrentNodeTest())
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
        case QueryOperator::equal:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_number == field.getUNumber()
                               );
        return;
        case QueryOperator::not_equal:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_number != field.getUNumber()
                               );
        return;
        case QueryOperator::highter:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_number > field.getUNumber()
                               );
        return;
        case QueryOperator::highte_equal:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_number >= field.getUNumber()
                               );
        return;
        case QueryOperator::lower:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_number < field.getUNumber()
                               );
        return;
        case QueryOperator::lower_equal:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_number <= field.getUNumber()
                               );
        return;
        case QueryOperator::in_range:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               field.getRange().in(node_number)
                               );
        return;
        case QueryOperator::out_range:
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
        case QueryOperator::equal:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_number == field.getNumber()
                               );
        return;
        case QueryOperator::not_equal:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_number != field.getNumber()
                               );
        return;
        case QueryOperator::highter:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_number > field.getNumber()
                               );
        return;
        case QueryOperator::highte_equal:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_number >= field.getNumber()
                               );
        return;
        case QueryOperator::lower:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_number < field.getNumber()
                               );
        return;
        case QueryOperator::lower_equal:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_number <= field.getNumber()
                               );
        return;
        case QueryOperator::in_range:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               field.getRange().in(node_number)
                               );
        return;
        case QueryOperator::out_range:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               field.getRange().out(node_number)
                               );
        return;
        default: invalidTest(); return;
      }
    };

    // TODO: RegExp test...
    static auto testString = [&](BufferArray node_string, QueryField& field) {
      if(field.getValueType() != QueryFieldValueType::string) {
        invalidTest();
        return;
      }
      switch (field.getOperatorType()) {
        case QueryOperator::equal:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_string == field.getString()
                               );
        return;
        case QueryOperator::not_equal:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_string != field.getString()
                               );
        return;
        case QueryOperator::highter:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_string > field.getString()
                               );
        return;
        case QueryOperator::highte_equal:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_string >= field.getString()
                               );
        return;
        case QueryOperator::lower:
          last_field_value = getRelationResult(
                               last_field_value,
                               field_rel,
                               node_string < field.getString()
                               );
        return;
        case QueryOperator::lower_equal:
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


      // Doesn't support with local structures
      case QueryProperty::node_index: invalidTest(); break;



      case QueryProperty::type:

        switch (field.getOperatorType()) {
          case QueryOperator::equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 test_node.getType() == field.getVarType()
                                 );
          break;
          case QueryOperator::not_equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 test_node.getType() != field.getVarType()
                                 );
          break;
          default: invalidTest(); break;
        }
      break;





      case QueryProperty::type_class:
        switch (field.getOperatorType()) {
          case QueryOperator::equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 test_node.getTypeClass() == field.getVarTypeClass()
                                 );
          break;
          case QueryOperator::not_equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 test_node.getTypeClass() != field.getVarTypeClass()
                                 );
          break;
          default: invalidTest(); break;
        }
      break;




      case QueryProperty::value_type:
        switch (field.getOperatorType()) {
          case QueryOperator::equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 toValueType(test_node.getType()) == field.getValType()
                                 );
          break;
          case QueryOperator::not_equal:
            last_field_value = getRelationResult(
                                 last_field_value,
                                 field_rel,
                                 toValueType(test_node.getType()) != field.getValType()
                                 );
          break;
          default: invalidTest(); break;
        }
      break;





      case QueryProperty::element_count:
        testUNumber(test_node.getElementCount(), field);
      break;




      case QueryProperty::index:
        testUNumber(index, field);
      break;





      case QueryProperty::name: {
        if(!name.getMemberCount()) {
          invalidTest();
          break;
        }
        testString(name, field);
        break;
      }




      case QueryProperty::value:

        switch (test_node.getTypeClass()) {

          case VarTypeClass::primitive:
            testNumber(test_node
                       .getVariable()
                       .toPrimitive()
                       .getValue()
                       .asSigned(), field);
          break;

          case VarTypeClass::buffer_array:
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

#endif // DATA_BASE_NODE_VISITOR_QUERY_H
