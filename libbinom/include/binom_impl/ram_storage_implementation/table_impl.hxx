#ifndef TABLE_IMPL_HXX
#define TABLE_IMPL_HXX

#include "../../variables/multi_map.hxx"
#include "../../variables/map.hxx"
#include "../query.hxx"

namespace binom::priv {

template<typename T>
constexpr bool is_map_type_v = std::is_same_v<T, binom::MultiMap> || std::is_same_v<T, binom::Map>;

class TableImplementation {
  std::set<binom::Variable, binom::Variable::ResourceComparator> rows;
  std::set<binom::index::Index, binom::index::IndexComparator> indexes;

  std::list<conditions::ConditionExpression>::iterator filterByAndBlock(TableImplementation& result, std::list<conditions::ConditionExpression>::iterator and_block_start);

  TableImplementation cloneTableHeader() const;

  TableImplementation() = default;

public:
  typedef binom::index::Index Column;

  TableImplementation(const binom::literals::table& table_descriptor);
  TableImplementation(const TableImplementation& other);
  TableImplementation(TableImplementation&& other);
  ~TableImplementation();

  template<typename T>
  requires is_map_type_v<T>
  Error insert(T row);

  Error remove(KeyValue column_name, KeyValue value, size_t index = 0, size_t count = 1);

  template<typename T>
  requires is_map_type_v<T>
  Error remove(T row);

  Variable getRow(KeyValue column_name, KeyValue value);
  TableImplementation find(conditions::ConditionQuery query);

  err::ProgressReport<Column> operator[](KeyValue column_name);
};

}

#endif // TABLE_IMPL_HXX
