#ifndef INDEX_IMPL_HXX
#define INDEX_IMPL_HXX

#include <set>
#include <list>
#include "../../variables/key_value.hxx"
#include "../../variables/variable.hxx"

namespace binom::priv {

class WeakLink {
  SharedResource* resource = nullptr;

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept {
    if(!resource) return OptionalSharedRecursiveLock();
    if(!resource->isExist()) return OptionalSharedRecursiveLock();
    return OptionalSharedRecursiveLock(SharedRecursiveLock(&resource->mtx, lock_type));
  }

  friend class Variable;
  friend class Link;
public:
  WeakLink() = default;
  WeakLink(const SharedResource& resource)
    : resource(const_cast<SharedResource*>(&resource)) {}
  WeakLink(const WeakLink& other)
    : resource(const_cast<WeakLink&>(other).resource) {}
  WeakLink(WeakLink&& other)
    : resource(other.resource) {}
  WeakLink(const Link& other)
    : resource(const_cast<Link&>(other).resource) {}
  WeakLink(Link&& other)
    : resource(other.resource) {}

  inline WeakLink& operator=(const SharedResource& resource) {return *new(this) WeakLink(resource);}
  inline WeakLink& operator=(const WeakLink& other) {return *new(this) WeakLink(other);}
  inline WeakLink& operator=(WeakLink&& other) {return *new(this) WeakLink(std::move(other));}
  inline WeakLink& operator=(const Link& other) {return *new(this) WeakLink(other);}
  inline WeakLink& operator=(Link&& other) {return *new(this) WeakLink(std::move(other));}

  inline bool operator==(WeakLink other) const {return resource == other.resource;}
  inline bool operator!=(WeakLink other) const {return resource == other.resource;}
  inline bool operator==(const Link& other) const {return resource == other.resource;}
  inline bool operator!=(const Link& other) const {return resource == other.resource;}
  inline bool operator==(Link&& other) const {return resource == other.resource;}
  inline bool operator!=(Link&& other) const {return resource == other.resource;}

};

Link::Link(WeakLink&& other) noexcept {
  if(!other.resource) return;
  if(auto lk = other.getLock(MtxLockType::shared_locked); lk) {
    ++other.resource->link_counter;
    resource = other.resource;
  } else return;
}

Link::Link(const WeakLink& other) noexcept {
  if(!other.resource) return;
  if(auto lk = other.getLock(MtxLockType::shared_locked); lk) {
    ++other.resource->link_counter;
    resource = other.resource;
  } else return;
}

}


namespace binom::index {

class Field;

class Index {
  friend Field;
public:
  class Comparator {
  public:
    using is_transparent = void;
    bool operator()(KeyValue const& search_value, Field* const& cell) const;
    bool operator()(Field* const& cell, KeyValue const& search_value) const;
    bool operator()(Field* const& lhs, Field* const& rhs) const;
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

  Index(IndexType type, KeyValue key) :
    type(type), key(std::move(key)), data(type) {}

  ~Index() {
    switch (type) {
    case IndexType::unique_index: data.unique_index.~set(); return;
    case IndexType::multi_index: data.multi_index.~multiset(); return;
    }
  }


  KeyValue getKey() const { return key; }

  Error add(Field& field);

  Error remove(Field& field);

};











class Field {
  friend Index;
  friend Index::Comparator;
public:
  enum class FieldType : ui8 {
    local = 0x00,
    indexed = 0x01,
    empty = 0xFF
  };


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

    char data[sizeof (IndexedField) > sizeof (LocalField)
              ? sizeof (IndexedField)
              : sizeof (LocalField)]{};
    LocalField local;
    IndexedField indexed;

    ~FieldData() {}
  };


  FieldType type = FieldType::empty;
  FieldData data{.data{}};
  binom::priv::WeakLink owner;

  void setEmpty() {
    switch (type) {
    case FieldType::local:
      data.local.~LocalField();
      std::memset(data.data, 0, sizeof(data.data));
      type = FieldType::empty;
    return;
    case FieldType::indexed:
      data.indexed.~IndexedField();
      std::memset(data.data, 0, sizeof(data.data));
      type = FieldType::empty;
    return;
    case FieldType::empty: return;
    }
  }

  Error addIndex(Index* index_ptr, Index::Iterator self_iterator) {
    switch (type) {
    case Field::FieldType::empty: return ErrorType::invalid_data;
    case Field::FieldType::local: {
      KeyValue value = data.local.value;

      data.local.~LocalField();
      new(&data.indexed) FieldData::IndexedField{
        .index_list{{index_ptr, self_iterator}},
        .value = std::move(value)
      };
    } return ErrorType::no_error;


    case Field::FieldType::indexed:
      data.indexed.index_list.emplace_back(index_ptr, self_iterator);
    return ErrorType::no_error;
    }
  }


  bool isCanBeIndexed() {
    return type == FieldType::empty
        ? false
        : type == FieldType::indexed
        ? true
        : toKeyType(data.local.value.getType()) != VarKeyType::invalid_type;
  }

public:
  Field(binom::priv::WeakLink owner, KeyValue key, Variable value)
    : type(FieldType::local), data{.local{std::move(key), std::move(value)}}, owner(std::move(owner)) {}

  Field(const Field&) = delete;

  Field(Field&& other)
    : type(other.type), owner(other.owner) {
    switch (other.type) {

    default:
    case FieldType::empty: return;

    case FieldType::local:
      new(&data.local) FieldData::LocalField {
        .key = std::move(other.data.local.key),
        .value = other.data.local.value.move()
      };
      other.setEmpty();
    return;

    case FieldType::indexed:
      new(&data.indexed) FieldData::IndexedField {
        .index_list = std::move(other.data.indexed.index_list),
        .value = std::move(other.data.indexed.value)
      };
      other.setEmpty();
    return;
    }
  }

  ~Field() {
    switch (type) {
    default:
    case FieldType::empty: return;
    case FieldType::local: data.local.~LocalField(); return;
    case FieldType::indexed: data.indexed.~IndexedField(); return;
    }
  }

  KeyValue getKey() const {
    switch (type) {
    default:
    case FieldType::empty: return {};
    case FieldType::local: return data.local.key;
    case FieldType::indexed: return data.indexed.index_list.front().index_ptr->getKey();
    }
  }

  KeyValue setKey(KeyValue key) {
    switch (type) {
    default:
    case FieldType::empty:
      new(&data.local) FieldData::LocalField {
        .key = std::move(key),
        .value = nullptr
      };
      type = FieldType::local;
    return data.local.key;
    case FieldType::local: return data.local.key = key;
    case FieldType::indexed:



    return data.indexed.index_list.front().index_ptr->getKey();
    }
  }

  Variable getValue() {
    switch (type) {
    default:
    case FieldType::empty: return {};
    case FieldType::local: return data.local.value.move();
    case FieldType::indexed: return data.indexed.value;
    }
  }

  const Variable getValue() const {
    switch (type) {
    default:
    case FieldType::empty: return {};
    case FieldType::local: return data.local.value.move();
    case FieldType::indexed: return data.indexed.value;
    }
  }


};



















inline Error Index::add(Field& field) {
  if(!field.isCanBeIndexed()) return ErrorType::binom_invalid_type;
  if(field.data.local.key != key) return ErrorType::invalid_data;

  switch (type) {
  case IndexType::unique_index:
    if(auto result = data.unique_index.insert(&field); result.second)
      field.addIndex(this, result.first);
    else return ErrorType::binom_key_unique_error;
    return ErrorType::no_error;

  case IndexType::multi_index:
    field.addIndex(this, data.multi_index.insert(&field));
  return ErrorType::no_error;
  }
}

inline Error Index::unlink(Iterator it) {
  switch (type) {
  case IndexType::unique_index:
    data.unique_index.erase(it);
    return ErrorType::no_error;

  case IndexType::multi_index:
    data.multi_index.erase(it);
  return ErrorType::no_error;
  }
}

// field->data.indexed.index_list.front().index_ptr->key;

inline bool Index::Comparator::operator()(const KeyValue& search_value, Field* const& field) const {
  return search_value < field->data.indexed.value;
}

inline bool Index::Comparator::operator()(Field* const& field, const KeyValue& search_value) const {
  return field->data.indexed.value < search_value;
}

inline bool Index::Comparator::operator()(Field* const& lhs, Field* const& rhs) const {
  return lhs->data.indexed.value < rhs->data.indexed.value;
}


};




#endif // INDEX_IMPL_HXX
