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
                      Relation next_relation = Relation::AND);

  ConditionExpression(std::initializer_list<ConditionExpression> subexprs, Relation next_relation = Relation::AND);

  ConditionExpression(const std::list<ConditionExpression>& subexprs, Relation next_relation = Relation::AND);

  ConditionExpression(std::list<ConditionExpression>&& subexprs, Relation next_relation = Relation::AND);

  ConditionExpression(const ConditionExpression& other);
  
  ConditionExpression(ConditionExpression&& other);
  
  KeyValue getColumnName() const;
  KeyValue getValue() const;
  Operator getOperator() const;
  Relation getNextRelation() const;

  std::list<ConditionExpression>* getSubexpression();

  const std::list<ConditionExpression>* getSubexpression() const;

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
                                                    ExpressionList::iterator and_block_end);

public:

  ConditionQuery(std::initializer_list<ConditionExpression> expressions);

  void simplifySubExpressions();

  Iterator begin();
  Iterator end();
  ConstIterator begin() const;
  ConstIterator end() const;
  ConstIterator cbegin() const;
  ConstIterator cend() const;
  ReverseIterator rbegin();
  ReverseIterator rend();
  ConstReverseIterator rbegin() const;
  ConstReverseIterator rend() const;
  ConstReverseIterator crbegin() const;
  ConstReverseIterator crend() const;

};

using cexp = ConditionExpression;
using cexp_list = std::initializer_list<ConditionExpression>;
using rel = Relation;
using op = Operator;

}

#endif
