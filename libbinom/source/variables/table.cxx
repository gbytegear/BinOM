#include "libbinom/include/variables/table.hxx"

using namespace binom;
using namespace binom::priv;

TableImplementation* Table::getData() { return resource_link->data.table_implementation; }

const TableImplementation* Table::getData() const { return resource_link->data.table_implementation; }

Table::Table(priv::Link&& link) : Variable(std::move(link)) {}

Table::Table(const binom::literals::table table_descriptor)
  : Variable(table_descriptor) {}

Table::Table(const Table& other)
  : Variable(other) {}

Table::Table(Table&& other)
  : Variable(std::move(other)) {}

Table Table::move() noexcept { return Link(resource_link); }
const Table Table::move() const noexcept {return Link(resource_link);}

bool Table::isEmpty() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return false;
  return getData()->isEmpty();
}

size_t Table::getElementCount() const noexcept {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return 0;
  return getData()->getSize();
}

template Error Table::insert<binom::Map>(binom::Map);
template Error Table::insert<binom::MultiMap>(binom::MultiMap);
template Error Table::insert<binom::Variable>(binom::Variable);

template<typename T>
requires is_map_type_v<T> || std::is_same_v<T, Variable>
Error Table::insert(binom::Table::T row) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return ErrorType::binom_resource_not_available;
  return getData()->insert(row.move());
}


Error Table::remove(KeyValue column_name, KeyValue value, size_t index, size_t count) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return ErrorType::binom_resource_not_available;
  return getData()->remove(std::move(column_name), std::move(value), index, count);
}

Error Table::remove(conditions::ConditionQuery query) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return ErrorType::binom_resource_not_available;
  return getData()->remove(std::move(query));
}

template Error Table::remove<binom::Map>(binom::Map);
template Error Table::remove<binom::MultiMap>(binom::MultiMap);
template Error Table::remove<binom::Variable>(binom::Variable);

template<typename T>
requires is_map_type_v<T> || std::is_same_v<T, Variable>
Error Table::remove(binom::Table::T row) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return ErrorType::binom_resource_not_available;
  return getData()->remove(row.move());
}

Variable Table::getRow(KeyValue column_name, KeyValue value) {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return nullptr;
  return getData()->getRow(std::move(column_name), std::move(value));
}

Table Table::find(conditions::ConditionQuery query) const {

}

Table Table::findAndMove(conditions::ConditionQuery query) {

}

size_t Table::merge(Table other_table) {

}

size_t Table::moveMerge(Table other_table) {

}

err::ProgressReport<Column> Table::operator[](KeyValue column_name) {

}
