#pragma once

#include "binom/includes/structure/file_storage/data_base_node_visitor.h"

using namespace binom;

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
        default:
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
