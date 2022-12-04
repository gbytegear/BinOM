#include "libbinom/include/binom_impl/ram_storage_implementation/table_impl.hxx"

using namespace binom;
using namespace binom::priv;

TableImplementation::TableImplementation(const binom::literals::table& table_descriptor) {
  for(auto& column_descriptor : table_descriptor.header)
    indexes.emplace(column_descriptor.second, std::move(column_descriptor.first));

  for(auto& row_descriptor : table_descriptor.row_list) insert(row_descriptor.move());
}

Error TableImplementation::insert(MultiMap row) {
  bool is_indexed = false;
  for(auto field_ref : row)
    if(auto index_it = indexes.find(field_ref.getKey()); index_it != indexes.cend()) {
      auto lk = index_it->getLock(MtxLockType::unique_locked);
      const_cast<index::Index&>(*index_it).add(*field_ref.data);
      is_indexed = true;
    } else continue;

  if(is_indexed) {
    rows.push_front(row.move());
    return ErrorType::no_error;
  } else return ErrorType::binom_row_has_no_fields_for_indexing;
}
