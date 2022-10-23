#ifndef TABLE_IMPL_HXX
#define TABLE_IMPL_HXX

#include "../../variables/variable.hxx"
#include "../../variables/named_variable.hxx"
#include "../multi_avl_tree.hxx"
#include "../avl_tree.hxx"

#include <set>
#include <initializer_list>

namespace binom::literals::priv {

struct TableLiteral {
  typedef std::initializer_list<std::pair<KeyValue, IndexType>> HeaderLiteral;
  typedef std::initializer_list<std::pair<KeyValue, Variable>> RowLiteral;
  typedef std::initializer_list<RowLiteral> RowListLiteral;

  HeaderLiteral header;
  RowListLiteral row_list;
};

}

namespace binom::priv {

struct Index;
class RowHeader;
struct IndexedRowCell;
struct UnindexedRowCell;


// =============================================================================================================
// Comparators

class RowCellComparator {
public:
  using is_transparent = void;
  bool operator()(IndexedRowCell const& lhs, IndexedRowCell const& rhs) const;
  bool operator()(KeyValue const& search_value, IndexedRowCell const& cell) const;
};

class RowUnindexedCellComparator {
public:
  using is_transparent = void;
  bool operator()(UnindexedRowCell const& lhs, UnindexedRowCell const& rhs) const;
  bool operator()(KeyValue const& search_value, UnindexedRowCell const& cell) const;
};

class RowComparator {
public:
  using is_transparent = void;
  bool operator()(KeyValue const& search_value, IndexedRowCell* const& cell) const;
  bool operator()(IndexedRowCell* const& lhs, IndexedRowCell* const& rhs) const;
};

class IndexComparator {
public:
  using is_transparent = void;
  bool operator()(KeyValue const& search_value, Index const& index);
  bool operator()(Index const& lhs, Index const& rhs) const;
};


// =============================================================================================================
// Structures

struct IndexedRowCell {
  RowHeader* row_header;
  KeyValue* name;
  KeyValue value;
};

struct UnindexedRowCell {
  KeyValue name;
  Variable value;
};

class RowHeader {
  friend class TableImplementation;
  std::set<IndexedRowCell, RowCellComparator> indexed_cells;
  std::set<UnindexedRowCell, RowUnindexedCellComparator> unindexed_cells;
public:

};

struct Index {

  union IndexData {
    static constexpr size_t INDEX_DATA_SIZE =
        sizeof (std::set<IndexedRowCell, RowComparator>) > sizeof (std::multiset<IndexedRowCell, RowComparator>)
        ? sizeof (std::set<IndexedRowCell, RowComparator>)
        : sizeof (std::multiset<IndexedRowCell, RowComparator>);

    std::set<IndexedRowCell*, RowComparator> unique_index_rows;
    std::multiset<IndexedRowCell*, RowComparator> multi_index_rows;

    IndexData(IndexType type) {
      switch (type) {
      case binom::IndexType::unique_index:
        new(&unique_index_rows) std::set<IndexedRowCell*, RowComparator>();
      return;
      case binom::IndexType::multi_index:
        new(&multi_index_rows) std::multiset<IndexedRowCell*, RowComparator>();
      return;
      }
    }

    IndexData(IndexType type, IndexData&& other) {
      switch (type) {
      case binom::IndexType::unique_index:
        new(&unique_index_rows) std::set<IndexedRowCell*, RowComparator>(std::move(other.unique_index_rows));
      return;
      case binom::IndexType::multi_index:
        new(&multi_index_rows) std::multiset<IndexedRowCell*, RowComparator>(std::move(other.multi_index_rows));
      return;
      }
    }

    ~IndexData() {}
  };

  KeyValue name;
  IndexType type;
  IndexData index;

  Error insert(IndexedRowCell* indexed_row_cell_ptr) {
    switch (type) {
    case IndexType::unique_index:
      if(index.unique_index_rows.insert(indexed_row_cell_ptr).second) return ErrorType::no_error;
      else return ErrorType::binom_key_unique_error;
    case IndexType::multi_index:
      index.multi_index_rows.insert(indexed_row_cell_ptr);
    return ErrorType::binom_key_unique_error;
    }
  }

  Index(KeyValue column_name, IndexType index_type)
    : name(std::move(column_name)), type(index_type), index(index_type) {}
  Index(Index&& other)
    : name(std::move(other.name)), type(other.type), index(other.type, std::move(other.index)) {}
};


// =============================================================================================================
// Compartor functions

inline bool RowCellComparator::operator()(const KeyValue& search_value, const IndexedRowCell& cell) const {
  return search_value < *cell.name;
}

inline bool RowCellComparator::operator()(IndexedRowCell const& lhs, IndexedRowCell const& rhs) const {
  return *lhs.name < *rhs.name;
}

inline bool RowUnindexedCellComparator::operator()(const KeyValue& search_value, const UnindexedRowCell& cell) const {
  return search_value < cell.name;
}

inline bool RowUnindexedCellComparator::operator()(UnindexedRowCell const& lhs, UnindexedRowCell const& rhs) const {
  return lhs.name < rhs.name;
}

inline bool RowComparator::operator()(const KeyValue& search_value, IndexedRowCell* const& cell) const {
  return search_value < cell->value;
}

inline bool RowComparator::operator()(IndexedRowCell* const& lhs, IndexedRowCell* const& rhs) const {
  return lhs->value < rhs->value;
}

inline bool IndexComparator::operator()(const KeyValue& search_value, const Index& index) {
  return search_value < index.name;
}

inline bool IndexComparator::operator()(const Index& lhs, const Index& rhs) const {
  return lhs.name < rhs.name;
}


// =============================================================================================================

class TableImplementation {
  std::set<Index, IndexComparator> indexes;
  std::list<RowHeader> row_list;

  void initTable(literals::table::HeaderLiteral table_header) {
    for(auto& column_info : table_header)
      indexes.emplace(Index(std::move(column_info.first), column_info.second));
  }

  void initRows(literals::table::RowListLiteral row_list) {
    for(auto& row_data : row_list) {
      RowHeader& row_header = self.row_list.emplace_back();
      for(auto& cell_data : row_data) {

        auto index_it = indexes.find(cell_data.first);

        if(index_it == indexes.cend()) {
          row_header.unindexed_cells.emplace(std::move(cell_data.first), std::move(cell_data.second));
          continue;

        } else {
          auto answer = row_header.indexed_cells.emplace(&row_header, &index_it->name, std::move(cell_data.second));

          if(!answer.second) {
            self.row_list.pop_back();
            throw binom::Error(ErrorType::binom_key_unique_error);
          }

          if(auto err = const_cast<Index&>(*index_it).insert(const_cast<IndexedRowCell*>(&*answer.first));
             err) {
            self.row_list.pop_back();
            throw err;
          }

        }
      }
      if(row_header.indexed_cells.empty())
        self.row_list.pop_back();
    }
  }

public:
  TableImplementation(literals::table table_literal) {
    initTable(std::move(table_literal.header));

  }


};

}

#endif // TABLE_IMPL_HXX
