#ifndef TABLE_IMPL_HXX
#define TABLE_IMPL_HXX

#include "../../variables/variable.hxx"
#include "../../variables/named_variable.hxx"
#include "../multi_avl_tree.hxx"
#include "../avl_tree.hxx"

#include <vector>
#include <initializer_list>

namespace binom::literals::priv {
enum class ColumnType {
  unique = 0,
  multi  = 1,
};

union Index {
       binom::priv::AVLTree unique;
  binom::priv::MultiAVLTree multi;
};

struct ColumnDescriptor {
  ColumnType column_type;
  KeyValue column_name;
};




struct TableDescriptor;
struct TableLiteral;
}

namespace binom::priv {

struct Column {
  binom::literals::priv::ColumnType column_type;
  binom::literals::priv::Index index;
};

class TableImplementation {
  size_t              column_count;
  size_t              row_count = 0;
  ValueStoringAVLTree<Column> columns;

public:

  inline size_t getRowCount() const noexcept    { return row_count; }
  inline size_t getColumnCount() const noexcept { return column_count; }

};

}

#endif // TABLE_IMPL_HXX
