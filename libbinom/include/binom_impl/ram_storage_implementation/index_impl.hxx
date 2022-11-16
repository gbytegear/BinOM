#ifndef INDEX_IMPL_HXX
#define INDEX_IMPL_HXX

#include <set>
#include <list>
#include "../../variables/key_value.hxx"
#include "../../variables/variable.hxx"

namespace binom::priv {

class WeakLink {
  SharedResource* resource = nullptr;

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept;

  friend class Variable;
  friend class Link;
public:
  WeakLink() = default;
  WeakLink(const SharedResource& resource);
  WeakLink(const WeakLink& other);
  WeakLink(WeakLink&& other);
  WeakLink(const Link& other);
  WeakLink(Link&& other);

  WeakLink& operator=(const SharedResource& resource);
  WeakLink& operator=(const WeakLink& other);
  WeakLink& operator=(WeakLink&& other);
  WeakLink& operator=(const Link& other);
  WeakLink& operator=(Link&& other);

  bool operator==(WeakLink other) const;
  bool operator!=(WeakLink other) const;
  bool operator==(const Link& other) const;
  bool operator!=(const Link& other) const;
  bool operator==(Link&& other) const;
  bool operator!=(Link&& other) const;

};

}


namespace binom::index {

class Field;
class MapComparator;

class Index {
  friend Field;
  friend MapComparator;
public:
  class Comparator {
  public:
    using is_transparent = void;
    bool operator()(const KeyValue& search_value, const Field*& cell) const;
    bool operator()(const Field*& cell, const KeyValue& search_value) const;
    bool operator()(const Field*& lhs, const Field*& rhs) const;
  };

private:

  union IndexData {
    std::set<Field*, Comparator> unique_index;
    std::multiset<Field*, Comparator> multi_index;

    IndexData(IndexType type) {
      switch (type) {
      case IndexType::unique_index:
        new(&unique_index) std::set<Field*, Comparator>();
      return;
      case IndexType::multi_index:
        new(&multi_index) std::multiset<Field*, Comparator>();
      return;
      }
    }

    ~IndexData() {}
  };

  IndexType type;
  KeyValue key;
  IndexData data;

  Error unlink(std::set<Field*, Comparator>::iterator it);

public:
  typedef std::set<Field*, Comparator>::iterator Iterator;
  typedef std::set<Field*, Comparator>::const_iterator ConstIterator;
  typedef std::set<Field*, Comparator>::reverse_iterator ReverseIterator;
  typedef std::set<Field*, Comparator>::const_reverse_iterator ConstReverseIterator;

  static_assert (
  std::is_same_v<std::set<Field*, Comparator>::iterator, std::multiset<Field*, Comparator>::iterator> &&
  std::is_same_v<std::set<Field*, Comparator>::const_iterator, std::multiset<Field*, Comparator>::const_iterator> &&
  std::is_same_v<std::set<Field*, Comparator>::reverse_iterator, std::multiset<Field*, Comparator>::reverse_iterator> &&
  std::is_same_v<std::set<Field*, Comparator>::const_reverse_iterator, std::multiset<Field*, Comparator>::const_reverse_iterator>
  , "std::set and std::multiset iterators isn't same");

  Index(IndexType type, KeyValue key);
  Index(const Index&) = delete;
  Index(Index&&) = delete;

  ~Index();

  KeyValue getKey() const;
  Error add(Field& field);
  Error remove(Field& field);

};





class Field {
  friend Index;
  friend Index::Comparator;
  friend MapComparator;
private:

  union FieldData {

    struct LocalField {
      KeyValue key;
      Variable value;
    };

    struct IndexedField {

      struct IndexEntry {
        Index* index_ptr;
        Index::Iterator self_iterator;

        ~IndexEntry() {index_ptr->unlink(self_iterator);}
      };

      std::list<IndexEntry> index_list;
      KeyValue value;
    };

    static constexpr size_t SIZE = sizeof (IndexedField) > sizeof (LocalField)
        ? sizeof (IndexedField)
        : sizeof (LocalField);

    char data[SIZE]{};
    LocalField local;
    IndexedField indexed;

    ~FieldData() {}
  };


  FieldType type = FieldType::empty;
  FieldData data{.data{}};
  binom::priv::WeakLink owner;

  void setEmpty();
  Error addIndex(Index* index_ptr, Index::Iterator self_iterator);
  bool isCanBeIndexed();

public:
  Field(binom::priv::WeakLink owner, KeyValue key, Variable value);
  Field(const Field&) = delete;
  Field(Field&& other);
  ~Field();

  KeyValue getKey() const;
  KeyValue setKey(KeyValue key);

  Variable getValue();
  Variable getValue() const;
  Variable setValue(Variable value);

};


class MapComparator {
public:
  using is_transparent = void;
  bool operator()(const KeyValue& search_value, const Field& cell) const;
  bool operator()(const Field& cell, const KeyValue& search_value) const;
  bool operator()(const Field& lhs, const Field& rhs) const;
};

};




#endif // INDEX_IMPL_HXX
