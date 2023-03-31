#ifndef TABLE_IMPL_HXX
#define TABLE_IMPL_HXX

#include "../../variables/multi_map.hxx"
#include "../../variables/map.hxx"
#include "../query.hxx"

#include <concepts>

namespace binom::priv {

using namespace binom::conditions;

template<typename T>
constexpr bool is_map_type_v = std::is_same_v<T, binom::MultiMap> || std::is_same_v<T, binom::Map>;

class TableImplementation {
  std::set<binom::Variable, binom::Variable::ResourceComparator> rows;
  std::set<binom::index::Index, binom::index::IndexComparator> indexes;

  template<typename T>
  requires std::is_same_v<T, TableImplementation&> || std::is_same_v<T, std::list<binom::Variable>&>
  void filterByConjunctionBlock(T result,
                                conditions::ConditionQuery::Iterator and_block_begin,
                                conditions::ConditionQuery::Iterator and_block_end);

  static bool test(Variable row, conditions::ConditionExpression& expression);

  TableImplementation cloneTableHeader() const;

  TableImplementation() = default;

public:
  typedef binom::index::Index Column;

  TableImplementation(const binom::literals::table& table_descriptor);
  TableImplementation(const TableImplementation& other);
  TableImplementation(TableImplementation&& other);
  ~TableImplementation();

  bool isEmpty() const { return rows.empty(); }

  template<typename T>
  requires is_map_type_v<T>
  Error insert(T row);

  Error remove(KeyValue column_name, KeyValue value, size_t index = 0, size_t count = 1);
  Error remove(conditions::ConditionQuery query);

  template <typename F>
  requires std::is_invocable_v<F, Variable>
  void changeWhere(conditions::ConditionQuery query, F callback) {
    if(isEmpty()) return;

    query.simplifySubExpressions();
    std::list<Variable> result;
    auto and_block_begin = query.begin();

    for(auto it = and_block_begin, end = query.end();; ++it)
      if(it->getNextRelation() == Relation::OR || it == end) {
        auto and_block_end = (it != end) ? ++ConditionQuery::Iterator(it) : it;

        if(and_block_begin != and_block_end)
          filterByConjunctionBlock<std::list<Variable>&>(result, and_block_begin, and_block_end);

        if(it == end)
          break;

        and_block_begin = and_block_end;
        continue;
      }

    for(auto& row : result) callback(row.move());
  }

  template<typename T>
  requires is_map_type_v<T>
  Error remove(T row);

  Variable getRow(KeyValue column_name, KeyValue value);
  TableImplementation find(conditions::ConditionQuery query);

  err::ProgressReport<Column> operator[](KeyValue column_name);
};

}

#endif // TABLE_IMPL_HXX
