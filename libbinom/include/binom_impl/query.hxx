#ifndef QUERY_HXX
#define QUERY_HXX

#include "libbinom/include/variables/key_value.hxx"
#include "libbinom/include/utils/iterator_template.hxx"
#include <list>

namespace binom::conditions {


// class Expression {
// public:
//   enum class Operator : ui8 {
//     equal,
//     not_equal,
//     lower,
//     lower_or_equal,
//     higher,
//     higher_or_equal,
//     subexpression
//   };

//   enum class Relation : ui8 { AND, OR, XOR };

// protected:
//   struct ExpressionData {
//     KeyValue column_name;
//     KeyValue value;
//   };

//   union Data {
//     ExpressionData exp;
//     Expression* subexp = nullptr;
//   };

//   Operator cmp_operator;
//   Relation next_relation = Relation::AND;
//   Expression* next = nullptr;
//   Expression* prev = nullptr;
//   Expression* last = this;
//   Data data;

//   Expression() = default;
// public:
//   Expression(KeyValue column_name)
//     : data{.exp={.column_name = std::move(column_name), .value = nullptr}} {}
  
//   Expression(Expression&& other)
//     : cmp_operator(other.cmp_operator),
//       next_relation(other.next_relation),
//       next(other.next),
//       prev(other.prev),
//       last(other.last) {
//     other.next = nullptr;
//     other.prev = nullptr;
//     other.last = nullptr;

//     switch(cmp_operator) {
//       case Operator::subexpression:
//       data.subexp = other.data.subexp;
//       other.data.subexp = nullptr;
//       return;
//       default:
//       data.exp = std::move(other.data.exp);
//       return;
//     }
//   }

//   Expression(const Expression& other)
//     : cmp_operator(other.cmp_operator),
//       next_relation(other.next_relation) {
//     if(cmp_operator == Operator::subexpression)
//       data.subexp = new Expression(*other.data.subexp);
//     if(other.next) {
//       next = new Expression(*other.next);
//       next->prev = this;
//       last = next->last;
//     }
//   }

//   ~Expression() {
//     if(cmp_operator == Operator::subexpression)
//       if(data.subexp) delete data.subexp;
//     if(next) delete next;
//   }
  
//   static Expression sub(Expression exp) {
//     Expression exp;
//     exp.cmp_operator = Operator::subexpression;
//     exp.data.subexp = new Expression(std::move(exp));
//     return exp;
//   }

//   Expression& operator == (KeyValue value) {
//     if(cmp_operator == Operator::subexpression) return self;
//     cmp_operator = Operator::equal;
//     data.exp.value = std::move(value);
//     return self;
//   }

//   Expression& operator!=(KeyValue value) {
//     if(cmp_operator == Operator::subexpression) return self;
//     cmp_operator = Operator::not_equal;
//     data.exp.value = std::move(value);
//     return self;
//   }

//   Expression& operator<(KeyValue value) {
//     if(cmp_operator == Operator::subexpression) return self;
//     cmp_operator = Operator::lower;
//     data.exp.value = std::move(value);
//     return self;
//   }

//   Expression& operator<=(KeyValue value) {
//     if(cmp_operator == Operator::subexpression) return self;
//     cmp_operator = Operator::lower_or_equal;
//     data.exp.value = std::move(value);
//     return self;
//   }

//   Expression& operator>(KeyValue value) {
//     if(cmp_operator == Operator::subexpression) return self;
//     cmp_operator = Operator::higher;
//     data.exp.value = std::move(value);
//     return self;
//   }

//   Expression& operator>=(KeyValue value) {
//     if(cmp_operator == Operator::subexpression) return self;
//     cmp_operator = Operator::higher_or_equal;
//     data.exp.value = std::move(value);
//     return self;
//   }

//   Expression& operator,(Relation rel) {
//     last->next_rel = rel;
//     return self;
//   }

//   Expression& operator,(Expression expr) {
//     last->next = new Expression(std::move(expr));
//     last->next->prev = last;
//     last = last->next;
//     return self;
//   }

// };

// typedef Expression exp;
// typedef Expression::Relation rel;

enum class Relation : ui8 {
  AND, OR
};

class ConditionExpression {
public:
  enum class Operator : ui8 {
    equal,
    not_equal,
    lower,
    lower_or_equal,
    higher,
    higher_or_equal,
    subexpression
  };

private:
  struct ConditionExpressionData {
    KeyValue column_name;
    KeyValue value;
  };

  union Data {
    void* empty = nullptr;
    ConditionExpressionData expression;
    std::list<ConditionExpression> subexpression;
    
    Data() = default;

    Data(KeyValue value)
      : expression{.column_name = std::move(value)} {}

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
  ConditionExpression(KeyValue value)
    : data(std::move(value)) {}

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
  std::list<ConditionExpression>* getSubexpression() { if(op != Operator::subexpression) return &data.subexpression; return nullptr; }
  const std::list<ConditionExpression>* getSubexpression() const  { if(op != Operator::subexpression) return &data.subexpression; return nullptr; }

  ConditionExpression& operator == (KeyValue value) {
    if(op == Operator::subexpression) return self;
    op = Operator::equal;
    data.expression.value = std::move(value);
    return self;
  }

  ConditionExpression& operator != (KeyValue value) {
    if(op == Operator::subexpression) return self;
    op = Operator::not_equal;
    data.expression.value = std::move(value);
    return self;
  }

  ConditionExpression& operator < (KeyValue value) {
    if(op == Operator::subexpression) return self;
    op = Operator::lower;
    data.expression.value = std::move(value);
    return self;
  }

  ConditionExpression& operator <= (KeyValue value) {
    if(op == Operator::subexpression) return self;
    op = Operator::lower_or_equal;
    data.expression.value = std::move(value);
    return self;
  }

  ConditionExpression& operator > (KeyValue value) {
    if(op == Operator::subexpression) return self;
    op = Operator::higher;
    data.expression.value = std::move(value);
    return self;
  }

  ConditionExpression& operator >= (KeyValue value) {
    if(op == Operator::subexpression) return self;
    op = Operator::higher_or_equal;
    data.expression.value = std::move(value);
    return self;
  }

};

class ConditionQuery {
  typedef std::list<ConditionExpression> ExpressionList;
  std::list<ConditionExpression> expressions;
public:
  typedef ExpressionList::iterator Iterator;
  typedef ExpressionList::const_iterator ConstIterator;
  typedef ExpressionList::reverse_iterator ReverseIterator;
  typedef ExpressionList::const_reverse_iterator ConstReverseIterator;

  ConditionQuery() = default;

  ConditionQuery& operator , (Relation rel) {
    auto& last = expressions.back();
    rel = rel;
    return self;
  }

  ConditionQuery& operator , (ConditionExpression exp) {
    expressions.push_back(exp);
    return self;
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
using rel = Relation;

}

#endif
