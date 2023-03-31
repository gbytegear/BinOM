#include "libbinom/include/binom_impl/query.hxx"

using namespace binom::conditions;

ConditionExpression::ConditionExpression(KeyValue column_name, Operator op, KeyValue value, Relation next_relation)
  : op(op), rel(next_relation), data(std::move(column_name), std::move(value)) {}

ConditionExpression::ConditionExpression(std::initializer_list<ConditionExpression> subexprs, Relation next_relation)
  : op(Operator::subexpression), rel(next_relation), data(subexprs) {}

ConditionExpression::ConditionExpression(const std::list<ConditionExpression> &subexprs, Relation next_relation)
  : op(Operator::subexpression), rel(next_relation), data(subexprs) {}

ConditionExpression::ConditionExpression(std::list<ConditionExpression> &&subexprs, Relation next_relation)
  : op(Operator::subexpression), rel(next_relation), data(std::move(subexprs)) {}

ConditionExpression::ConditionExpression(const ConditionExpression &other)
  : op(other.op),
    rel(other.rel),
    data(other.op, other.data) {}

ConditionExpression::ConditionExpression(ConditionExpression &&other)
  : op(other.op),
    rel(other.rel),
    data(other.op, std::move(other.data)) {}

binom::KeyValue ConditionExpression::getColumnName() const { if(op != Operator::subexpression) return data.expression.column_name; return nullptr; }

binom::KeyValue ConditionExpression::getValue() const { if(op != Operator::subexpression) return data.expression.value; return nullptr; }

Operator ConditionExpression::getOperator() const { return op; }

Relation ConditionExpression::getNextRelation() const { return rel; }

std::list<ConditionExpression> *ConditionExpression::getSubexpression() {
  if(op == Operator::subexpression)
    return &data.subexpression;
  return nullptr;
}

const std::list<ConditionExpression> *ConditionExpression::getSubexpression() const  {
  if(op == Operator::subexpression)
    return &data.subexpression;
  return nullptr;
}







std::list<ConditionExpression>::iterator ConditionQuery::simplifyConjunctionBlock(ExpressionList::iterator and_block_begin, ExpressionList::iterator subexpression_pos, ExpressionList::iterator and_block_end) {

  auto& subexpression = *subexpression_pos->getSubexpression();
  auto sub_and_block_begin = subexpression.begin();
  auto sub_and_block_end = subexpression.end();
  auto add_iterator = and_block_end;

  const bool subexpression_is_first = subexpression_pos == and_block_begin;
  const bool subexpression_is_last = subexpression_pos == --ExpressionList::iterator(and_block_end);

  for(auto it = sub_and_block_begin, end = sub_and_block_end;; ++it) {

    if(it->getNextRelation() == Relation::OR || it == end) {
      sub_and_block_end = (it != end) ? ++ExpressionList::iterator(it) : it;

      // Insert expressions before subexpression
      if(!subexpression_is_first)
        for(auto it = and_block_begin; it != subexpression_pos; ++it)
          if(add_iterator == and_block_end)
            and_block_end = expressions.insert(add_iterator, *it);
          else expressions.insert(add_iterator, *it);

      // Insert expressions from conjunction block of subexpression
      for(auto it = sub_and_block_begin; it != sub_and_block_end; ++it)
        if(add_iterator == and_block_end)
          and_block_end = expressions.insert(add_iterator, *it);
        else expressions.insert(add_iterator, *it);

      // Insert expressions after subexpression
      if(!subexpression_is_last) {
        (--ExpressionList::iterator(add_iterator))->rel = Relation::AND;
        for(auto it = ++ExpressionList::iterator(subexpression_pos); it != and_block_end; ++it)
          expressions.insert(add_iterator, *it);
      }

      if(it == end)
        return expressions.erase(and_block_begin, and_block_end);

      sub_and_block_begin = sub_and_block_end;
      continue;
    }

  }
}

ConditionQuery::ConditionQuery(std::initializer_list<ConditionExpression> expressions)
  : expressions(expressions) {}

void ConditionQuery::simplifySubExpressions() {
  auto and_block_begin = expressions.begin();
  for (auto it = expressions.begin(), end = expressions.end(); it != end;) {

    if(it->getOperator() == Operator::subexpression) {
      auto subexpression_pos = it;
      for(;it != end && it->getNextRelation() != Relation::OR; ++it);
      auto and_block_end = (it != end) ? ++it : it;
      and_block_begin = it = simplifyConjunctionBlock(and_block_begin, subexpression_pos, and_block_end);
      continue;
    }

    if (it->getNextRelation() == Relation::OR) {
      and_block_begin = it;
      ++and_block_begin;
    }

    ++it;
  }
}

ConditionQuery::Iterator ConditionQuery::begin() { return expressions.begin(); }

ConditionQuery::Iterator ConditionQuery::end() { return expressions.end(); }

ConditionQuery::ConstIterator ConditionQuery::begin() const { return expressions.begin(); }

ConditionQuery::ConstIterator ConditionQuery::end() const { return expressions.end(); }

ConditionQuery::ConstIterator ConditionQuery::cbegin() const { return expressions.begin(); }

ConditionQuery::ConstIterator ConditionQuery::cend() const { return expressions.end(); }

ConditionQuery::ReverseIterator ConditionQuery::rbegin() { return expressions.rbegin(); }

ConditionQuery::ReverseIterator ConditionQuery::rend() { return expressions.rend(); }

ConditionQuery::ConstReverseIterator ConditionQuery::rbegin() const { return expressions.rbegin(); }

ConditionQuery::ConstReverseIterator ConditionQuery::rend() const { return expressions.rend(); }

ConditionQuery::ConstReverseIterator ConditionQuery::crbegin() const { return expressions.crbegin(); }

ConditionQuery::ConstReverseIterator ConditionQuery::crend() const { return expressions.crend(); }
