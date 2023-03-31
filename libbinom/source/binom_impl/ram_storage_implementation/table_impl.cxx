#include "libbinom/include/binom_impl/ram_storage_implementation/table_impl.hxx"

//#define ENABLE_DEBUG
//#include "libbinom/include/utils/debug.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::index;
using namespace binom::conditions;

TableImplementation TableImplementation::cloneTableHeader() const {
  TableImplementation new_table;
  for(const auto& index : indexes) {
    new_table.indexes.emplace(index.getType(), index.getKey());
  }
  return new_table;
}

TableImplementation::TableImplementation(const binom::literals::table& table_descriptor) {
  for(auto& column_descriptor : table_descriptor.header)
    indexes.emplace(column_descriptor.second, std::move(column_descriptor.first));

  for(auto& row_descriptor : table_descriptor.row_list) insert(row_descriptor.move());
}

TableImplementation::TableImplementation(const TableImplementation& other) {
  for(const auto& index : other.indexes)
    indexes.emplace(index.getType(), index.getKey());
  
  for(const auto& row : other.rows) 
    switch(row.getTypeClass()) {
      case VarTypeClass::map: insert(row.toMap()); continue;
      case VarTypeClass::multimap: insert(row.toMultiMap()); continue;
      default: continue;
    }
}

TableImplementation::TableImplementation(TableImplementation&& other)
  : rows{std::move(other.rows)}, indexes{std::move(other.indexes)} {}

binom::priv::TableImplementation::~TableImplementation() {}

template Error TableImplementation::insert<binom::Map>(binom::Map);
template Error TableImplementation::insert<binom::MultiMap>(binom::MultiMap);

template<typename T>
requires is_map_type_v<T>
Error TableImplementation::insert(T row) {

  if(rows.contains(row.upcast().move()))
    return ErrorType::binom_key_unique_error;

  bool is_indexed = false;

  auto lk = row.getLock(MtxLockType::unique_locked);
  if(!lk) return ErrorType::binom_resource_not_available;
  if(auto error = row.addTable(self); error)
    return error;

  for(auto it = row.begin(), end = row.end(); it != end; ++it) {
    if(auto index_it = indexes.find(KeyValue(it->getKey())); index_it != indexes.cend()) {
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
  if(!count || isEmpty()) return {};

  if(auto index_it = indexes.find(column_name); index_it != indexes.cend()) {
    for (auto range = index_it->getRange(std::move(value));
        range.first != range.second && count > 0;
        (++range.first, index > 0 ? --index : --count)) {
        if(index > 0) continue;

        auto row = range.first->getOwner();
        auto lk = row.getLock(MtxLockType::unique_locked);
        if(!lk) return ErrorType::binom_resource_not_available;

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

Error TableImplementation::remove(conditions::ConditionQuery query) {
  if(isEmpty()) return {};

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

  for(auto& row : result) {
    auto lk = row.getLock(MtxLockType::unique_locked);
    if(!lk) continue; // Resource not avalible

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
  if(auto index_it = indexes.find(column_name); index_it != indexes.cend()) {
    auto lk = index_it->getLock(MtxLockType::shared_locked);
    if(auto field_it = index_it->find(value); field_it != index_it->cend())
      return FieldRef(*field_it).getOwner();
  }
  return {};
}



template void TableImplementation::filterByConjunctionBlock<TableImplementation&>(TableImplementation&,
                                                                                  ConditionQuery::Iterator,
                                                                                  ConditionQuery::Iterator);
template void TableImplementation::filterByConjunctionBlock<std::list<binom::Variable>&>(std::list<binom::Variable>&,
                                                                                         ConditionQuery::Iterator,
                                                                                         ConditionQuery::Iterator);

template<typename T>
requires std::is_same_v<T, TableImplementation&> || std::is_same_v<T, std::list<binom::Variable>&>
void TableImplementation::filterByConjunctionBlock(T result,
                                                   ConditionQuery::Iterator and_block_begin,
                                                   ConditionQuery::Iterator and_block_end) {

  auto insertIntoResult = [&result](Variable row) {
    if constexpr (std::is_same_v<T, TableImplementation&>)
      switch (row.getType()) {
      case binom::VarType::map: result.insert(std::move(row.toMap())); break;
      case binom::VarType::multimap: result.insert(std::move(row.toMultiMap())); break;
      }
    else if constexpr (std::is_same_v<T, std::list<binom::Variable>&>)
      result.emplace_back(std::move(row));
  };

  const Index* search_index = nullptr;
  conditions::ConditionQuery::Iterator expression_for_index_search;

  { // Find the best search index
    ui16 index_score = 0;
    for(auto it = and_block_begin; it != and_block_end; ++it) {
      auto index_it = indexes.find(it->getColumnName());
      if(index_it == indexes.cend()) continue;

      ui16 current_index_score = 0;

      switch(it->getOperator()) {
      case Operator::equal: current_index_score += 1000; break;
      case Operator::higher: current_index_score += 500; break;
      case Operator::higher_or_equal: current_index_score += 250; break;
      case Operator::lower: current_index_score += 500; break;
      case Operator::lower_or_equal: current_index_score += 250; break;
      case Operator::not_equal: current_index_score += 125; break;
      default: break;
      }

      switch(index_it->getType()) {
      case IndexType::unique_index: current_index_score += 1000; break;
      case IndexType::multi_index: current_index_score += 500; break;
      }

      if(current_index_score > index_score) {
        search_index = &*index_it;
        index_score = current_index_score;
        expression_for_index_search = it;

      } elif(current_index_score == index_score) {
        if(index_it->getIndexedFieldsCount() < search_index->getIndexedFieldsCount()) {
          search_index = &*index_it;
          index_score = current_index_score;
          expression_for_index_search = it;
        }
      }

    }
  }

  // Scan search
  if(!search_index) {
    for(auto row_it = rows.begin(), end = rows.end(); row_it != end; ++row_it) {
      bool insert_this_row = true;

      for(auto expr_it = and_block_begin; expr_it != and_block_end; ++expr_it)
        if(test(row_it->move(), *expr_it)) continue;
        else { insert_this_row = false; break; }

      if(insert_this_row) insertIntoResult(row_it->move());
    }
    return;
  }

  // Indexed search
  switch(expression_for_index_search->getOperator()) {
  case binom::conditions::Operator::equal: {
    auto range = search_index->getRange(expression_for_index_search->getValue());

    for(auto [row_it, end] = range; row_it != end; ++row_it) {
      bool insert_this_row = true;

      for(auto expr_it = and_block_begin; expr_it != and_block_end; ++expr_it) {
        if(expr_it == expression_for_index_search) continue;
        if(test(row_it->getOwner().move(), *expr_it)) continue;
        else { insert_this_row = false; break; }
      }

      if(insert_this_row) insertIntoResult(row_it->getOwner().move());
    }
  } return;
  case binom::conditions::Operator::not_equal:{
    auto range = search_index->getNegativeRange(expression_for_index_search->getValue());

    auto [row_it, first_end] = range.first;
    auto [second_begin, end] = range.second;

    for(; row_it != end; ++row_it) {
      if(row_it == first_end) row_it = second_begin;
      bool insert_this_row = true;

      for(auto expr_it = and_block_begin; expr_it != and_block_end; ++expr_it) {
        if(expr_it == expression_for_index_search) continue;
        if(test(row_it->getOwner(), *expr_it)) continue;
        else { insert_this_row = false; break; }
      }

      if(insert_this_row) insertIntoResult(row_it->getOwner().move());
    }
  } return;
  case binom::conditions::Operator::lower:{
    auto range = search_index->getLowerRange(expression_for_index_search->getValue());

    for(auto [row_it, end] = range; row_it != end; ++row_it) {
      bool insert_this_row = true;

      for(auto expr_it = and_block_begin; expr_it != and_block_end; ++expr_it) {
        if(expr_it == expression_for_index_search) continue;
        if(test(row_it->getOwner(), *expr_it)) continue;
        else { insert_this_row = false; break; }
      }

      if(insert_this_row) insertIntoResult(row_it->getOwner().move());
    }
  } return;
  case binom::conditions::Operator::lower_or_equal:{
    auto range = search_index->getLowerEqualRange(expression_for_index_search->getValue());

    for(auto [row_it, end] = range; row_it != end; ++row_it) {
      bool insert_this_row = true;

      for(auto expr_it = and_block_begin; expr_it != and_block_end; ++expr_it) {
        if(expr_it == expression_for_index_search) continue;
        if(test(row_it->getOwner(), *expr_it)) continue;
        else { insert_this_row = false; break; }
      }

      if(insert_this_row) insertIntoResult(row_it->getOwner().move());
    }
  } return;
  case binom::conditions::Operator::higher:{
    auto range = search_index->getHigherRange(expression_for_index_search->getValue());

    for(auto [row_it, end] = range; row_it != end; ++row_it) {
      bool insert_this_row = true;

      for(auto expr_it = and_block_begin; expr_it != and_block_end; ++expr_it) {
        if(expr_it == expression_for_index_search) continue;
        if(test(row_it->getOwner(), *expr_it)) continue;
        else { insert_this_row = false; break; }
      }

      if(insert_this_row) insertIntoResult(row_it->getOwner().move());
    }
  } return;
  case binom::conditions::Operator::higher_or_equal:{
    auto range = search_index->getHigherEqualRange(expression_for_index_search->getValue());

    for(auto [row_it, end] = range; row_it != end; ++row_it) {
      bool insert_this_row = true;

      for(auto expr_it = and_block_begin; expr_it != and_block_end; ++expr_it) {
        if(expr_it == expression_for_index_search) continue;
        if(test(row_it->getOwner(), *expr_it)) continue;
        else { insert_this_row = false; break; }
      }

      if(insert_this_row) insertIntoResult(row_it->getOwner().move());
    }
  } return;
  default: return;
  }
}

bool TableImplementation::test(Variable row, conditions::ConditionExpression &expression) {
  switch(expression.getOperator()) {
  case binom::conditions::Operator::equal:
    switch (row.getType()) {
    case VarType::map:
      return KeyValue(row.toMap().getVariable(expression.getColumnName())) == expression.getValue();
    case VarType::multimap:
      for(auto [it, end] = row.toMultiMap().getRange(expression.getColumnName()); it != end; ++it)
        if(KeyValue(it->getValue()) == expression.getValue()) return true;
      return false;
    break;
    }
  break;
  case binom::conditions::Operator::not_equal:
    switch (row.getType()) {
    case VarType::map:
      return KeyValue(row.toMap().getVariable(expression.getColumnName())) != expression.getValue();
    case VarType::multimap:
      for(auto [it, end] = row.toMultiMap().getRange(expression.getColumnName()); it != end; ++it)
        if(KeyValue(it->getValue()) != expression.getValue()) return true;
      return false;
    }
  break;
  case binom::conditions::Operator::lower:
    switch (row.getType()) {
    case VarType::map:
      return KeyValue(row.toMap().getVariable(expression.getColumnName())) < expression.getValue();
    case VarType::multimap:
      for(auto [it, end] = row.toMultiMap().getRange(expression.getColumnName()); it != end; ++it)
        if(KeyValue(it->getValue()) < expression.getValue()) return true;
      return false;
    }
  break;
  case binom::conditions::Operator::lower_or_equal:
    switch (row.getType()) {
    case VarType::map:
      return KeyValue(row.toMap().getVariable(expression.getColumnName())) <= expression.getValue();
    case VarType::multimap:
      for(auto [it, end] = row.toMultiMap().getRange(expression.getColumnName()); it != end; ++it)
        if(KeyValue(it->getValue()) <= expression.getValue()) return true;
      return false;
    }
  break;
  case binom::conditions::Operator::higher:
    switch (row.getType()) {
    case VarType::map:
      return KeyValue(row.toMap().getVariable(expression.getColumnName())) > expression.getValue();
    case VarType::multimap:
      for(auto [it, end] = row.toMultiMap().getRange(expression.getColumnName()); it != end; ++it)
        if(KeyValue(it->getValue()) > expression.getValue()) return true;
      return false;
    }
  break;
  case binom::conditions::Operator::higher_or_equal:
    switch (row.getType()) {
    case VarType::map:
      return KeyValue(row.toMap().getVariable(expression.getColumnName())) >= expression.getValue();
    case VarType::multimap:
      for(auto [it, end] = row.toMultiMap().getRange(expression.getColumnName()); it != end; ++it)
        if(KeyValue(it->getValue()) > expression.getValue()) return true;
      return false;
    }
  break;
  }
  return false;
}


TableImplementation TableImplementation::find(conditions::ConditionQuery query) {
  query.simplifySubExpressions();
  TableImplementation result = cloneTableHeader();
  auto and_block_begin = query.begin();

  for(auto it = and_block_begin, end = query.end();; ++it) {

    if(it->getNextRelation() == Relation::OR || it == end) {
      auto and_block_end = (it != end) ? ++ConditionQuery::Iterator(it) : it;

      if(and_block_begin != and_block_end)
        filterByConjunctionBlock<TableImplementation&>(result, and_block_begin, and_block_end);

      if(it == end)
        return result;

      and_block_begin = and_block_end;
      continue;
    }

  }
}

err::ProgressReport<TableImplementation::Column> binom::priv::TableImplementation::operator[](KeyValue column_name)  {
  if(auto column_it = indexes.find(std::move(column_name)); column_it != indexes.cend())
    return err::ProgressReport<TableImplementation::Column>(&const_cast<Column&>(*column_it));
  else return ErrorType::binom_out_of_range;
}
