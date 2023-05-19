#ifndef MAP_HXX
#define MAP_HXX

#include "../binom_impl/ram_storage_implementation/map_impl.hxx"
#include "field.hxx"

namespace binom {

class Map : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator BufferArray& () = delete;
  operator Array& () = delete;
  operator List& () = delete;
  operator MultiMap& () = delete;
  operator Table& () = delete;

  Number& toNumber() = delete;
  BitArray& toBitArray() = delete;
  BufferArray& toBufferArray() = delete;
  Array& toArray() = delete;
  List& toList() = delete;
  Map& toMap() = delete;
  MultiMap& toMultiMap() = delete;
  Table& toTable() = delete;

  operator const Number& () const = delete;
  operator const BitArray& () const = delete;
  operator const BufferArray& () const = delete;
  operator const Array& () const = delete;
  operator const List& () const = delete;
  operator const MultiMap& () const = delete;
  operator const Table& () const = delete;

  const Number& toNumber() const = delete;
  const BitArray& toBitArray() const = delete;
  const BufferArray& toBufferArray() const = delete;
  const Array& toArray() const = delete;
  const List& toList() const = delete;
  const Map& toMap() const = delete;
  const MultiMap& toMultiMap() const = delete;
  const Table& toTable() const = delete;

  Variable& operator=(const Variable& other) = delete;
  Variable& operator=(Variable&& other) = delete;

  Variable& changeLink(const Variable& other) = delete;
  Variable& changeLink(Variable&& other) = delete;

  priv::MapImplementation* getData();
  const priv::MapImplementation* getData() const;

  friend class Variable;
  Map(priv::Link&& link);

  friend class priv::TableImplementation;
  friend class index::Field;
  Error addTable(priv::TableImplementation& table);
  Error removeTable(priv::TableImplementation& table);
  bool updateKey(index::Field& field, const KeyValue& new_key);

public:
//  using FieldRef = binom::index::FieldRef;
  typedef priv::MapImplementation::Iterator             Iterator;
  typedef priv::MapImplementation::ReverseIterator      ReverseIterator;
  typedef priv::MapImplementation::ConstIterator        ConstIterator;
  typedef priv::MapImplementation::ConstReverseIterator ConstReverseIterator;

  Map();
  Map(literals::map element_list);
  Map(const Map& other);
  Map(Map&& other);

  Map move() noexcept;
  const Map move() const noexcept;

  bool isEmpty() const noexcept;
  size_t getElementCount() const noexcept;
  bool contains(KeyValue value) const noexcept;

  void clear();
  err::ProgressReport<FieldRef> insert(KeyValue key, Variable variable);
  err::Error remove(KeyValue key);
  err::ProgressReport<FieldRef> rename(KeyValue old_key, KeyValue new_key);

  Variable getVariable(KeyValue key);
  const Variable getVariable(KeyValue key) const;
  FieldRef getField(KeyValue key);
  const FieldRef getField(KeyValue key) const;
  FieldRef operator[] (KeyValue key);
  const FieldRef operator[] (KeyValue key) const;

  Iterator find(KeyValue key);
  ReverseIterator rfind(KeyValue key);
  ConstIterator find(KeyValue key) const;
  ConstReverseIterator rfind(KeyValue key) const;

  Iterator begin();
  Iterator end();

  ReverseIterator rbegin();
  ReverseIterator rend();

  ConstIterator begin() const noexcept;
  ConstIterator end() const noexcept;

  ConstReverseIterator rbegin() const noexcept;
  ConstReverseIterator rend() const noexcept;

  ConstIterator cbegin() const noexcept;
  ConstIterator cend() const noexcept;

  ConstReverseIterator crbegin() const noexcept;
  ConstReverseIterator crend() const noexcept;

  Map& operator=(const Map& other);
  Map& operator=(Map&& other);

  Map& changeLink(const Map& other);
  Map& changeLink(Map&& other);

};

}

#endif // MAP_HXX
