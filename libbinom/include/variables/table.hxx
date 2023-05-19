#ifndef TABLE_HXX
#define TABLE_HXX

#include "../binom_impl/ram_storage_implementation/table_impl.hxx"
#include <concepts>

namespace binom {

class Table : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator BufferArray& () = delete;
  operator Array& () = delete;
  operator List& () = delete;
  operator Map& () = delete;
  operator MultiMap& () = delete;

  Number& toNumber() = delete;
  BitArray& toBitArray() = delete;
  BufferArray& toBufferArray() = delete;
  Array& toArray() = delete;
  List& toList() = delete;
  Map& toMap() = delete;
  MultiMap& toMultiMap() = delete;

  operator const Number& () const = delete;
  operator const BitArray& () const = delete;
  operator const BufferArray& () const = delete;
  operator const Array& () const = delete;
  operator const List& () const = delete;
  operator const Map& () const = delete;
  operator const MultiMap& () const = delete;

  const Number& toNumber() const = delete;
  const BitArray& toBitArray() const = delete;
  const BufferArray& toBufferArray() const = delete;
  const Array& toArray() const = delete;
  const List& toList() const = delete;
  const Map& toMap() const = delete;
  const MultiMap& toMultiMap() const = delete;

  Variable& operator=(const Variable& other) = delete;
  Variable& operator=(Variable&& other) = delete;

  Variable& changeLink(const Variable& other) = delete;
  Variable& changeLink(Variable&& other) = delete;

  priv::TableImplementation* getData();
  const priv::TableImplementation* getData() const;

  friend class Variable;
  Table(priv::Link&& link);

  Table(priv::TableImplementation&& tbl_impl);

public:
  using Column = priv::TableImplementation::Column;
  using ColumnSet = priv::TableImplementation::ColumnSet;
  using Iterator = priv::TableImplementation::ColumnSet::iterator;
  using ConstIterator = priv::TableImplementation::ColumnSet::const_iterator;
  using ReverseIterator = priv::TableImplementation::ColumnSet::reverse_iterator;
  using ConstReverseIterator = priv::TableImplementation::ColumnSet::const_reverse_iterator;

  Table();
  Table(const binom::literals::table table_descriptor);
  Table(const Table& other);
  Table(Table&& other);

  Table move() noexcept;
  const Table move() const noexcept;

  void clear();
  bool isEmpty() const noexcept;
  size_t getElementCount() const noexcept;
//  TODO: bool contains(conditions::ConditionQuery query) const noexcept;

  template<typename T>
  requires priv::is_map_type_v<T> || std::is_same_v<T, Variable>
  Error insert(T row);

  Error remove(KeyValue column_name, KeyValue value, size_t index = 0, size_t count = 1);
  Error remove(conditions::ConditionQuery query);
  template<typename T>
  requires priv::is_map_type_v<T> || std::is_same_v<T, Variable>
  Error remove(T row);

  template <typename F>
  requires std::is_invocable_v<F, Variable>
  void changeWhere(conditions::ConditionQuery query, F callback) {
    auto lk = getLock(MtxLockType::unique_locked);
    if(!lk) return;
    if constexpr (std::movable<F>) return getData()->changeWhere(std::move(query), std::move(callback));
    else return getData()->changeWhere(std::move(query), callback);
  }

  Variable getRow(KeyValue column_name, KeyValue value);
  Variable getRow(KeyValue column_name, KeyValue value) const;

  Table find(conditions::ConditionQuery query) const;
  Table findAndMove(conditions::ConditionQuery query) const;

  size_t merge(Table other_table);
  size_t moveMerge(Table other_table);

  Iterator begin();
  Iterator end();
  ConstIterator begin() const;
  ConstIterator end() const;
  ConstIterator cbegin() const;
  ConstIterator cend() const;
  ReverseIterator rbegin();
  ReverseIterator rend();
  ConstReverseIterator rbegin() const;
  ConstReverseIterator rend() const;
  ConstReverseIterator crbegin() const;
  ConstReverseIterator crend() const;

  Table& operator += (Table other);
  Table operator + (Table other);

  err::ProgressReport<Column> operator[](KeyValue column_name);

  Table& operator=(const Table& other);
  Table& operator=(Table&& other);

  Table& changeLink(const Table& other);
  Table& changeLink(Table&& other);


};

}

#endif // TABLE_HXX
