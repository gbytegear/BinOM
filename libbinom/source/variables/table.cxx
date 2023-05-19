#include "libbinom/include/variables/table.hxx"

using namespace binom;
using namespace binom::priv;

TableImplementation* Table::getData() { return resource_link->data.table_implementation; }

const TableImplementation* Table::getData() const { return resource_link->data.table_implementation; }

Table::Table(priv::Link&& link) : Variable(std::move(link)) {}

Table::Table(priv::TableImplementation&& tbl_impl)
  : Variable(ResourceData{VarType::table, {.table_implementation = new priv::TableImplementation(std::move(tbl_impl))}}) {}

Table::Table()
  : Variable(binom::literals::table{{}}) {}

Table::Table(const binom::literals::table table_descriptor)
  : Variable(table_descriptor) {}

Table::Table(const Table& other)
  : Variable(other) {}

Table::Table(Table&& other)
  : Variable(std::move(other)) {}

Table Table::move() noexcept { return Link(resource_link); }
const Table Table::move() const noexcept {return Link(resource_link);}

void Table::clear() {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return;
  return getData()->clear();
}

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
Error Table::insert(T row) {
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
Error Table::remove(T row) {
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
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return {};
  return const_cast<priv::TableImplementation*>(getData())->find(std::move(query));
}

Table Table::findAndMove(conditions::ConditionQuery query) const {
  auto lk = getLock(MtxLockType::shared_locked);
  if(!lk) return {};
  return const_cast<priv::TableImplementation*>(getData())->findAndMove(std::move(query));
}

size_t Table::merge(Table other_table) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return 0;
  return getData()->merge(*other_table.getData());
}

size_t Table::moveMerge(Table other_table) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return 0;
  return getData()->moveMerge(*other_table.getData());
}

Table::Iterator Table::begin() {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->begin();
  else return Iterator(nullptr);
}

Table::Iterator Table::end() {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->end();
  else return Iterator(nullptr);
}

Table::ConstIterator Table::begin() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->begin();
  else return Iterator(nullptr);
}

Table::ConstIterator Table::end() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->end();
  else return Iterator(nullptr);
}

Table::ReverseIterator Table::rbegin() {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->rbegin();
  else return ReverseIterator(Iterator(nullptr));
}

Table::ReverseIterator Table::rend() {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->rend();
  else return ReverseIterator(Iterator(nullptr));
}

Table::ConstReverseIterator Table::rbegin() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->rbegin();
  else return ConstReverseIterator(Iterator(nullptr));
}

Table::ConstReverseIterator Table::rend() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->rend();
  else return ConstReverseIterator(Iterator(nullptr));
}

Table::ConstIterator Table::cbegin() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->begin();
  else return Iterator(nullptr);
}

Table::ConstIterator Table::cend() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->cend();
  else return Iterator(nullptr);
}

Table::ConstReverseIterator Table::crbegin() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->crbegin();
  else return ConstReverseIterator(Iterator(nullptr));
}

Table::ConstReverseIterator Table::crend() const {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return getData()->crend();
  else return ConstReverseIterator(Iterator(nullptr));
}

Table& Table::operator += (Table other) {
  moveMerge(std::move(other));
  return self;
}

Table Table::operator + (Table other) {
  Table new_table(self);
  new_table.moveMerge(std::move(other));
  return new_table;
}

err::ProgressReport<Table::Column> Table::operator[](KeyValue column_name) {
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return ErrorType::binom_resource_not_available;
  return (*getData()).operator[](std::move(column_name));
}


Table& Table::operator=(const Table& other) {
  if(resource_link == other.resource_link) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

Table& Table::operator=(Table&& other) {
  if(resource_link == other.resource_link) return self;
  auto lk = getLock(MtxLockType::unique_locked);
  if(!lk) return self;
  resource_link.overwriteWithResourceCopy(**other.resource_link);
  return self;
}

Table& Table::changeLink(const Table& other) {
  if(this == &other) return self;
  this->~Table();
  return *new(this) Table(other);
}

Table& Table::changeLink(Table&& other) {
  if(this == &other) return self;
  this->~Table();
  return *new(this) Table(std::move(other));
}
