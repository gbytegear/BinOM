#ifndef DATA_BASE_NODE_VISITOR_QUERY_H
#define DATA_BASE_NODE_VISITOR_QUERY_H

#include "data_base_node_visitor.h"

using namespace binom;

#define NEW_QUERY
#ifdef NEW_QUERY

bool DBNodeVisitor::test(Query &query, ui64 index, BufferArray name) {
  class TestExpressionFrame {
    QRel last_rel = QRel::AND;
    bool last_value = true;

    void invalidTest() {last_value = getRelationResult(last_value, last_rel, false);}

    void testUNumber(ui64 node_number, Query::QueryEpression& field) {
      if(field.getQValType() != QValType::number &&
         field.getQValType() != QValType::range) {
        invalidTest();
        return;
      }
      switch (field.getOper()) {
        case QOper::equal:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_number == field.getUNumber()
                               );
        return;
        case QOper::not_equal:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_number != field.getUNumber()
                               );
        return;
        case QOper::highter:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_number > field.getUNumber()
                               );
        return;
        case QOper::highte_equal:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_number >= field.getUNumber()
                               );
        return;
        case QOper::lower:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_number < field.getUNumber()
                               );
        return;
        case QOper::lower_equal:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_number <= field.getUNumber()
                               );
        return;
        case QOper::in_range:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               field.getRange().in(node_number)
                               );
        return;
        case QOper::out_range:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               field.getRange().out(node_number)
                               );
        return;
        default: invalidTest(); return;
      }
    }

    void testNumber(i64 node_number, Query::QueryEpression& field) {
      if(field.getQValType() != QValType::number &&
         field.getQValType() != QValType::range) {
        invalidTest();
        return;
      }
      switch (field.getOper()) {
        case QOper::equal:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_number == field.getNumber()
                               );
        return;
        case QOper::not_equal:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_number != field.getNumber()
                               );
        return;
        case QOper::highter:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_number > field.getNumber()
                               );
        return;
        case QOper::highte_equal:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_number >= field.getNumber()
                               );
        return;
        case QOper::lower:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_number < field.getNumber()
                               );
        return;
        case QOper::lower_equal:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_number <= field.getNumber()
                               );
        return;
        case QOper::in_range:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               field.getRange().in(node_number)
                               );
        return;
        case QOper::out_range:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               field.getRange().out(node_number)
                               );
        return;
        default: invalidTest(); return;
      }
    };

    void testString(BufferArray node_string, Query::QueryEpression& field) {
      if(field.getQValType() != QValType::string) {
        invalidTest();
        return;
      }
      switch (field.getOper()) {
        case QOper::equal:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_string == field.getString()
                               );
        return;
        case QOper::not_equal:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_string != field.getString()
                               );
        return;
        case QOper::highter:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_string > field.getString()
                               );
        return;
        case QOper::highte_equal:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_string >= field.getString()
                               );
        return;
        case QOper::lower:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_string < field.getString()
                               );
        return;
        case QOper::lower_equal:
          last_value = getRelationResult(
                               last_value,
                               last_rel,
                               node_string >= field.getString()
                               );
        return;
        default: invalidTest(); return;
      }
    };

    bool getRelationResult(bool last, QRel rel, bool current) {
      switch (rel) {
        case binom::QRel::AND:  return last && current;
        case binom::QRel::OR:   return last || current;
        case binom::QRel::XOR:  return last != current;
      }
    }

  public:

    void operator()(Query::QueryEpression expr, DBNodeVisitor node, const ui64 index, BufferArray& name) {
      if(expr.hasPath())
        node(expr.getPath());

      switch (expr.getProp()) {
        case binom::QProp::type:
          switch (expr.getOper()) {
            case QOper::equal:
              last_value = getRelationResult(
                                   last_value,
                                   last_rel,
                                   node.getType() == expr.getVarType()
                                   );
            break;
            case QOper::not_equal:
              last_value = getRelationResult(
                                   last_value,
                                   last_rel,
                                   node.getType() != expr.getVarType()
                                   );
            break;
            default: invalidTest(); break;
          }
        break;
        case binom::QProp::type_class:
          switch (expr.getOper()) {
            case QOper::equal:
              last_value = getRelationResult(
                                   last_value,
                                   last_rel,
                                   node.getTypeClass() == expr.getVarTypeClass()
                                   );
            break;
            case QOper::not_equal:
              last_value = getRelationResult(
                                   last_value,
                                   last_rel,
                                   node.getTypeClass() != expr.getVarTypeClass()
                                   );
            break;
            default: invalidTest(); break;
          }
        break;
        case binom::QProp::value_type:
          switch (expr.getOper()) {
            case QOper::equal:
              last_value = getRelationResult(
                                   last_value,
                                   last_rel,
                                   toValueType(node.getType()) == expr.getValType()
                                   );
            break;
            case QOper::not_equal:
              last_value = getRelationResult(
                                   last_value,
                                   last_rel,
                                   toValueType(node.getType()) != expr.getValType()
                                   );
            break;
            default: invalidTest(); break;
          }
        break;
        case binom::QProp::element_count:
          testUNumber(node.getElementCount(), expr);
        break;
        case binom::QProp::index:
          testUNumber(index, expr);
        break;
        case binom::QProp::name: {
          if(!name.getMemberCount()) {
            invalidTest();
            break;
          }
          testString(name, expr);
          break;
        }
        case binom::QProp::value:
          switch (node.getTypeClass()) {

            case VarTypeClass::primitive:
              testNumber(node
                         .getVariable()
                         .toPrimitive()
                         .getValue()
                         .asSigned(), expr);
            break;

            case VarTypeClass::buffer_array:
              if(toValueType(node.getType()) != ValType::byte)
                invalidTest();
              else
                testString(node.getVariable().toBufferArray(), expr);
            break;

            default: invalidTest(); break;
          }
        break;
        case binom::QProp::sub_exp: {
          TestExpressionFrame testExpr;
          for(Query::QueryEpression& subexpr : expr) testExpr(subexpr, node, index, name);
          last_value = getRelationResult(last_value, last_rel, testExpr.last_value);
          break;}
      }
      last_rel = expr.getNextRel();
    }

    bool getValue() const {return last_value;}

  } testExpr;

  for(Query::QueryEpression& expr : query) testExpr(expr, *this, index, name);
  return testExpr.getValue();
}

#else

bool DBNodeVisitor::test(Query &query, ui64 index, BufferArray name) {

  static const auto getRelationResult = [](bool last, QueryNextFieldRelation rel, bool current){
    switch (rel) {
      default:
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

      case QueryProperty::node_index:
        testUNumber(getNodeIndex(), field);
      break;



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

#endif
#endif // DATA_BASE_NODE_VISITOR_QUERY_H
