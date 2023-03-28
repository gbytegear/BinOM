#ifndef QUERY_HXX
#define QUERY_HXX

#include "libbinom/include/variables/key_value.hxx"
#include "libbinom/include/utils/iterator_template.hxx"
#include <list>

namespace binom::conditions {

enum class Relation : ui8 {
  AND, OR
};

enum class Operator : ui8 {
  equal,
  not_equal,
  lower,
  lower_or_equal,
  higher,
  higher_or_equal,
  subexpression
};

class ConditionQuery;

class ConditionExpression {
  friend ConditionQuery;

  struct ConditionExpressionData {
    KeyValue column_name;
    KeyValue value;
  };

  union Data {
    void* empty = nullptr;
    ConditionExpressionData expression;
    std::list<ConditionExpression> subexpression;
    
    Data() = default;

    Data(KeyValue column_name, KeyValue value)
      : expression{
          .column_name = std::move(column_name),
          .value = std::move(value)
    } {}

    Data(std::initializer_list<ConditionExpression>& subexpression)
      : subexpression(subexpression) {}

    Data(const std::list<ConditionExpression>& subexpression)
      : subexpression(subexpression) {}

    Data(std::list<ConditionExpression>&& subexpression)
      : subexpression(std::move(subexpression)) {}

    Data(Operator op, const Data& other) {
      switch(op) {
        case Operator::subexpression:
          new(&subexpression) std::list<ConditionExpression>(other.subexpression);
        return;
        default:
          new(&expression) ConditionExpressionData(other.expression);
        return;
      }
    }

    Data(Operator op, Data&& other) {
      switch(op) {
        case Operator::subexpression:
          new(&subexpression) std::list<ConditionExpression>(std::move(other.subexpression));
        return;
        default:
          new(&expression) ConditionExpressionData(std::move(other.expression));
        return;
      }
    }

    ~Data() {}

  };

  Operator op = Operator::equal;
  Relation rel = Relation::AND;
  Data data;

  ConditionExpression() = default;

public:
  ConditionExpression(KeyValue column_name,
                      Operator op,
                      KeyValue value,
                      Relation next_relation = Relation::AND)
    : op(op), rel(next_relation), data(std::move(column_name), std::move(value)) {}

  ConditionExpression(std::initializer_list<ConditionExpression> subexprs, Relation next_relation = Relation::AND)
    : op(Operator::subexpression), rel(next_relation), data(subexprs) {}

  ConditionExpression(const std::list<ConditionExpression>& subexprs, Relation next_relation = Relation::AND)
    : op(Operator::subexpression), rel(next_relation), data(subexprs) {}

  ConditionExpression(std::list<ConditionExpression>&& subexprs, Relation next_relation = Relation::AND)
    : op(Operator::subexpression), rel(next_relation), data(std::move(subexprs)) {}

  ConditionExpression(const ConditionExpression& other)
    : op(other.op),
      rel(other.rel),
      data(other.op, other.data) {}
  
  ConditionExpression(ConditionExpression&& other)
    : op(other.op),
      rel(other.rel),
      data(other.op, std::move(other.data)) {}
  
  KeyValue getColumnName() const { if(op != Operator::subexpression) return data.expression.column_name; return nullptr; }
  KeyValue getValue() const { if(op != Operator::subexpression) return data.expression.value; return nullptr; }
  Operator getOperator() const { return op; }
  Relation getNextRelation() const { return rel; }

  std::list<ConditionExpression>* getSubexpression() {
    if(op == Operator::subexpression)
      return &data.subexpression;
    return nullptr;
  }

  const std::list<ConditionExpression>* getSubexpression() const  {
    if(op == Operator::subexpression)
      return &data.subexpression;
    return nullptr;
  }

};

class ConditionQuery {
public:
  typedef std::list<ConditionExpression> ExpressionList;
  typedef ExpressionList::iterator Iterator;
  typedef ExpressionList::const_iterator ConstIterator;
  typedef ExpressionList::reverse_iterator ReverseIterator;
  typedef ExpressionList::const_reverse_iterator ConstReverseIterator;
private:
  ExpressionList expressions;

  ExpressionList::iterator simplifyConjunctionBlock(ExpressionList::iterator and_block_begin,
                                                    ExpressionList::iterator subexpression_pos,
                                                    ExpressionList::iterator and_block_end) {

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

public:

  ConditionQuery(std::initializer_list<ConditionExpression> expressions)
    : expressions(expressions) {}

  void simplifySubExpressions() {
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

  Iterator begin() { return expressions.begin(); }
  Iterator end() { return expressions.end(); }
  ConstIterator begin() const { return expressions.begin(); }
  ConstIterator end() const { return expressions.end(); }
  ConstIterator cbegin() const { return expressions.begin(); }
  ConstIterator cend() const { return expressions.end(); }
  ReverseIterator rbegin() { return expressions.rbegin(); }
  ReverseIterator rend() { return expressions.rend(); }
  ConstReverseIterator rbegin() const { return expressions.rbegin(); }
  ConstReverseIterator rend() const { return expressions.rend(); }
  ConstReverseIterator crbegin() const { return expressions.crbegin(); }
  ConstReverseIterator crend() const { return expressions.crend(); }

};

using cexp = ConditionExpression;
using cexp_list = std::initializer_list<ConditionExpression>;
using rel = Relation;
using op = Operator;

}

#endif
