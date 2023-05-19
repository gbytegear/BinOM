#ifndef TABLE_IMPL_HXX
#define TABLE_IMPL_HXX

#include "../../variables/multi_map.hxx"
#include "../../variables/map.hxx"
#include "../../variables/field.hxx"
#include "../../utils/pseudo_pointer.hxx"
#include "../query.hxx"

#include <concepts>
#include <list>


namespace binom::priv {

using namespace binom::conditions;

template<typename T>
constexpr bool is_map_type_v = std::is_same_v<T, binom::MultiMap> || std::is_same_v<T, binom::Map>;

class TableImplementation {
public:
  typedef std::set<binom::Variable, binom::Variable::ResourceComparator> RowSet;
  typedef std::set<binom::index::Index, binom::index::IndexComparator> ColumnSet;
  typedef std::list<binom::conditions::ConditionQuery> ConstraitList;
  typedef binom::index::Index Column;

private:
  RowSet rows;
  ColumnSet indexes;
  ConstraitList constraits;

  template<typename F>
  requires std::is_invocable_v<F, Variable>
  void filterByConjunctionBlock(F callback,
                                conditions::ConditionQuery::Iterator and_block_begin,
                                conditions::ConditionQuery::Iterator and_block_end);

  static bool test(Variable row, conditions::ConditionExpression& expression);
  static bool test(Variable& row, conditions::ConditionQuery& query);

  TableImplementation cloneTableHeader() const;

  TableImplementation() = default;

public:

  TableImplementation(const binom::literals::table& table_descriptor);
  TableImplementation(const TableImplementation& other);
  TableImplementation(TableImplementation&& other);
  ~TableImplementation();

  bool isEmpty() const { return rows.empty(); }
  size_t getSize() const { return rows.size(); }
  bool contains(conditions::ConditionQuery query) const;

  template<typename T>
  requires is_map_type_v<T> || std::is_same_v<T, Variable>
  Error insert(T row);

  Error remove(KeyValue column_name, KeyValue value, size_t index = 0, size_t count = 1);
  Error remove(conditions::ConditionQuery query);
  void clear();

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
          filterByConjunctionBlock(callback, and_block_begin, and_block_end);

        if(it == end)
          break;

        and_block_begin = and_block_end;
        continue;
      }
  }

  template<typename T>
  requires is_map_type_v<T> || std::is_same_v<T, Variable>
  Error remove(T row);

  Variable getRow(KeyValue column_name, KeyValue value);
  TableImplementation find(conditions::ConditionQuery query);
  TableImplementation findAndMove(conditions::ConditionQuery query);

  size_t merge(TableImplementation& other_table);
  size_t moveMerge(TableImplementation& other_table);

  err::ProgressReport<Column> operator[](KeyValue column_name);

  inline ColumnSet::iterator begin() { return indexes.begin(); }
  inline ColumnSet::iterator end() { return indexes.end(); }
  inline ColumnSet::const_iterator begin() const { return indexes.begin(); }
  inline ColumnSet::const_iterator end() const { return indexes.end(); }
  inline ColumnSet::const_iterator cbegin() const { return indexes.cbegin(); }
  inline ColumnSet::const_iterator cend() const { return indexes.cend(); }
  inline ColumnSet::reverse_iterator rbegin() { return indexes.rbegin(); }
  inline ColumnSet::reverse_iterator rend() { return indexes.rend(); }
  inline ColumnSet::const_reverse_iterator rbegin() const { return indexes.rbegin(); }
  inline ColumnSet::const_reverse_iterator rend() const { return indexes.rend(); }
  inline ColumnSet::const_reverse_iterator crbegin() const { return indexes.crbegin(); }
  inline ColumnSet::const_reverse_iterator crend() const { return indexes.crend(); }
};

}

#endif // TABLE_IMPL_HXX/
