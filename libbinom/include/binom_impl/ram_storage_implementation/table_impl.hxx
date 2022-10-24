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
  bool operator()(IndexedRowCell* const& cell, KeyValue const& search_value) const;
  bool operator()(IndexedRowCell* const& lhs, IndexedRowCell* const& rhs) const;
};

class IndexComparator {
public:
  using is_transparent = void;
  bool operator()(const KeyValue& search_value, const Index& index) const;
  bool operator()(const Index& index, const KeyValue& search_value) const;
  bool operator()(const Index& lhs, const Index& rhs) const;
};


// =============================================================================================================
// Structures

struct IndexedRowCell {
  RowHeader* row_header;
  Index* index;
  KeyValue value;
  union {
    std::set<IndexedRowCell*, RowComparator>::iterator unique;
    std::multiset<IndexedRowCell*, RowComparator>::iterator multi;
  } self_iterator;

  ~IndexedRowCell();
};

struct UnindexedRowCell {
  KeyValue name;
  Variable value;
};

class RowHeader {
  friend class TableImplementation;
  std::set<IndexedRowCell, RowCellComparator> indexed_cells;
  std::set<UnindexedRowCell, RowUnindexedCellComparator> unindexed_cells;
  std::list<RowHeader>::iterator self_iterator;
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
    case IndexType::unique_index: {
      auto result = index.unique_index_rows.insert(indexed_row_cell_ptr);
      if(!result.second) return ErrorType::binom_key_unique_error;
      indexed_row_cell_ptr->self_iterator.unique = result.first;
      return ErrorType::no_error;
    }
    case IndexType::multi_index:
      indexed_row_cell_ptr->self_iterator.multi = index.multi_index_rows.insert(indexed_row_cell_ptr);
    return ErrorType::no_error;
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
  return search_value < cell.index->name;
}

inline bool RowCellComparator::operator()(IndexedRowCell const& lhs, IndexedRowCell const& rhs) const {
  return lhs.index->name < rhs.index->name;
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

inline bool RowComparator::operator()(IndexedRowCell* const& cell, const KeyValue& search_value) const {
  return cell->value < search_value;
}

inline bool RowComparator::operator()(IndexedRowCell* const& lhs, IndexedRowCell* const& rhs) const {
  return lhs->value < rhs->value;
}

inline bool IndexComparator::operator()(const KeyValue& search_value, const Index& index) const {
  return search_value < index.name;
}

inline bool IndexComparator::operator()(const Index& index, const KeyValue& search_value) const {
  return index.name < search_value;
}

inline bool IndexComparator::operator()(const Index& lhs, const Index& rhs) const {
  return lhs.name < rhs.name;
}

inline IndexedRowCell::~IndexedRowCell() {
  switch (index->type) {
  case IndexType::unique_index:
    index->index.unique_index_rows.erase(self_iterator.unique);
  return;
  case IndexType::multi_index:
    index->index.multi_index_rows.erase(self_iterator.multi);
  return;
  }
}


// =============================================================================================================

class TableImplementation {
  std::set<Index, IndexComparator> indexes;
  std::list<RowHeader> row_list;

public:
  TableImplementation(literals::table table_literal) {

    for(auto& column_info : table_literal.header)
      indexes.emplace(Index(std::move(column_info.first), column_info.second));

    for(auto& row_data : table_literal.row_list)
      if(auto err = insert(std::move(row_data)); err) throw err;

  }

  Error insert(literals::table::RowLiteral row_data) {
    RowHeader& row_header = self.row_list.emplace_back();
    row_header.self_iterator = self.row_list.rbegin().base();

    for(auto& cell_data : row_data) {

      auto index_it = indexes.find(cell_data.first);

      if(index_it == indexes.cend()) {
        row_header.unindexed_cells.emplace(std::move(cell_data.first), std::move(cell_data.second));
        continue;

      } else {
        auto answer = row_header.indexed_cells.emplace(&row_header, const_cast<Index*>(&*index_it), std::move(cell_data.second));

        if(!answer.second) {
          self.row_list.pop_back();
          return ErrorType::binom_key_unique_error;
        }

        if(auto err = const_cast<Index&>(*index_it).insert(const_cast<IndexedRowCell*>(&*answer.first));
           err) {
          self.row_list.pop_back();
          return err;
        }

      }
    }
    return ErrorType::no_error;
  }

  Error remove(KeyValue column_name, KeyValue value, size_t index = 0) {
    auto index_it = indexes.find(std::move(column_name));
    if(index_it == indexes.cend()) return ErrorType::binom_invalid_column_name;

    switch (index_it->type) {
    case IndexType::unique_index: {
      auto value_it = index_it->index.unique_index_rows.find(std::move(value));
      if(value_it == index_it->index.unique_index_rows.cend())
        return ErrorType::binom_out_of_range;
      row_list.erase((*value_it)->row_header->self_iterator);
    } break;
    case IndexType::multi_index:
      auto value_it = index_it->index.multi_index_rows.lower_bound(std::move(value));
      if(value_it == index_it->index.multi_index_rows.cend())
        return ErrorType::binom_out_of_range;
      for(;index != 0; (++value_it, --index));
      if(value_it == index_it->index.multi_index_rows.cend())
        return ErrorType::binom_out_of_range;
      row_list.erase((*value_it)->row_header->self_iterator);
    break;
    }

    return ErrorType::no_error;
  }


};

}

#endif // TABLE_IMPL_HXX
