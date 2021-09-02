#pragma once

#include "binom/includes/file_storage/file_node_visitor.h"

using namespace binom;

class TestExpressionFrame {
  QRel last_rel = QRel::AND;
  bool last_value = true;

  void invalidTest();
  void testUNumber(ui64 node_number, Query::QueryEpression& field);
  void testNumber(i64 node_number, Query::QueryEpression& field);;
  void testString(BufferArray node_string, Query::QueryEpression& field);;
  bool getRelationResult(bool last, QRel rel, bool current);

public:
  void operator()(Query::QueryEpression expr, NodeVisitor node, const ui64 index);
  bool getValue() const {return last_value;}
};
