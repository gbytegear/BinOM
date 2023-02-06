#include "libbinom/include/binom_impl/ram_storage_implementation/table_impl.hxx"

#define ENABLE_DEBUG
#include "libbinom/include/utils/debug.hxx"

using namespace binom;
using namespace binom::priv;

TableImplementation::TableImplementation(const binom::literals::table& table_descriptor) {
  for(auto& column_descriptor : table_descriptor.header)
    indexes.emplace(column_descriptor.second, std::move(column_descriptor.first));

  for(auto& row_descriptor : table_descriptor.row_list) insert(row_descriptor.move());
}

binom::priv::TableImplementation::~TableImplementation() {}

template Error TableImplementation::insert<binom::Map>(binom::Map);
template Error TableImplementation::insert<binom::MultiMap>(binom::MultiMap);

template<typename T>
requires is_map_type_v<T>
Error TableImplementation::insert(T row) {

  if(rows.contains(row.upcast().move()))
    return ErrorType::binom_key_unique_error;

  bool is_indexed = false;

  if(auto error = row.addTable(self); error)
    return error;

  for(auto it = row.begin(), end = row.end(); it != end; ++it) {
    if(auto index_it = indexes.find(it->getKey()); index_it != indexes.cend()) {
      Error error;
      {
        auto lk = index_it->getLock(MtxLockType::unique_locked);
        error = const_cast<index::Index&>(*index_it).add(*it->data);
      }

      if(error) {
        if(is_indexed) {
          --it;
          for(auto start = row.begin(); it != start; --it) {
            if(auto index_it = indexes.find(it->getKey()); index_it != indexes.cend()) {
              if(auto error = const_cast<index::Index&>(*index_it).remove(*it->data); error)
                throw ErrorType::binom_data_corrupted;
            }
          }
        }
        return error;
      }

      is_indexed = true;
    } else continue;
  }

  if(is_indexed) {
    rows.emplace(row.upcast().move());
    return ErrorType::no_error;
  } else return ErrorType::binom_row_has_no_fields_for_indexing;
}


Error TableImplementation::remove(KeyValue column_name, KeyValue value, size_t index, size_t count) {
  if(!count) return {};

  if(auto index_it = indexes.find(column_name); index_it != indexes.cend()) {
    for (auto range = index_it->getRange(std::move(value));
        range.first != range.second && count > 0;
        (++range.first, index > 0 ? --index : --count)) {
        if(index > 0) continue;

        auto row = range.first->getOwner();

        switch (row.getType()) {
        case VarType::map:

          if(row.toMap().removeTable(self)) continue;

          for(auto field : row.toMap()) {
            if(!field.isIndexed()) continue;
            if(auto index_it = indexes.find(field.getKey()); index_it != indexes.cend()) {
              auto lk = index_it->getLock(MtxLockType::unique_locked);
              const_cast<index::Index&>(*index_it).remove(*field.data);
            } else continue;
          }

        case VarType::multimap:
          
          if(row.toMultiMap().removeTable(self)) continue;

          for(auto field : row.toMultiMap()) {
            if(!field.isIndexed()) continue;
            if(auto index_it = indexes.find(field.getKey()); index_it != indexes.cend()) {
              auto lk = index_it->getLock(MtxLockType::unique_locked);
              const_cast<index::Index&>(*index_it).remove(*field.data);
            } else continue;
          }
        }

        rows.erase(row.move());
    }
  } else return ErrorType::binom_invalid_column_name;

  return ErrorType::no_error;
}

template Error TableImplementation::remove<binom::Map>(binom::Map);
template Error TableImplementation::remove<binom::MultiMap>(binom::MultiMap);

template<typename T>
requires is_map_type_v<T>
Error TableImplementation::remove(T row) {

  if(!rows.contains(row.upcast().move()))
    return ErrorType::binom_out_of_range;
  
  row.removeTable(self);

  for(auto field : row) {
    if(!field.isIndexed()) continue;
    if(auto index_it = indexes.find(field.getKey()); index_it != indexes.cend()) {
      auto lk = index_it->getLock(MtxLockType::unique_locked);
      const_cast<index::Index&>(*index_it).remove(*field.data);
    } else continue;
  }

  rows.erase(row.upcast().move());

  return ErrorType::no_error;
}

Variable binom::priv::TableImplementation::getRow(KeyValue column_name, KeyValue value) {
  if(auto index_it = indexes.find(column_name); index_it != indexes.cend())
    if(auto field_it = index_it->find(value); field_it != index_it->cend())
      return FieldRef(*field_it).getOwner();
  return {};
}
