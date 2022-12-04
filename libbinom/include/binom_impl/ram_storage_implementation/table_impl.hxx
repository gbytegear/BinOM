#ifndef TABLE_IMPL_HXX
#define TABLE_IMPL_HXX

#include "../../variables/multi_map.hxx"

namespace binom::priv {

class TableImplementation {
  std::list<binom::MultiMap> rows;
  std::set<binom::index::Index, binom::index::IndexComparator> indexes;

public:
  TableImplementation(const binom::literals::table& table_descriptor);

  Error insert(binom::MultiMap row);
};

}

#endif // TABLE_IMPL_HXX
