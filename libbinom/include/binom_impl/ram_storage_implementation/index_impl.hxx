#ifndef INDEX_IMPL_HXX
#define INDEX_IMPL_HXX

#include <set>
#include <map>
#include <list>
#include "../../variables/key_value.hxx"
#include "../../variables/variable.hxx"
#include "../../utils/pseudo_pointer.hxx"


namespace binom::index {

class Field;
struct MapComparator;
struct IndexComparator;

}

namespace binom::priv {

class WeakLink {
  SharedResource* resource = nullptr;

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept;

  friend class Variable;
  friend class Link;
  friend class binom::index::Field;

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


class Index {
  friend Field;
  friend MapComparator;
  friend binom::priv::TableImplementation;
  friend IndexComparator;
public:
  class Comparator {
  public:
    using is_transparent = void;
    bool operator()(const KeyValue& search_value, const Field* const& cell) const;
    bool operator()(const Field* const& cell, const KeyValue& search_value) const;
    bool operator()(const Field* const& lhs, const Field* const& rhs) const;
  };

  static_assert (
  std::is_same_v<std::set<Field*, Comparator>::iterator, std::multiset<Field*, Comparator>::iterator> &&
  std::is_same_v<std::set<Field*, Comparator>::const_iterator, std::multiset<Field*, Comparator>::const_iterator> &&
  std::is_same_v<std::set<Field*, Comparator>::reverse_iterator, std::multiset<Field*, Comparator>::reverse_iterator> &&
  std::is_same_v<std::set<Field*, Comparator>::const_reverse_iterator, std::multiset<Field*, Comparator>::const_reverse_iterator>
  , "std::set and std::multiset iterators isn't same");

private:

  union IndexData {
    void* empty = nullptr;
    std::set<Field*, Comparator> unique_index;
    std::multiset<Field*, Comparator> multi_index;

    IndexData() {}

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

    IndexData(std::set<Field*, Comparator>&& data)
      : unique_index(std::move(data)) {}
    
    IndexData(std::multiset<Field*, Comparator>&& data)
      : multi_index(std::move(data)) {}

    ~IndexData() {}
  };

  IndexType type;
  mutable std::shared_mutex mtx;
  KeyValue key;
  IndexData data;

  Error unlink(std::set<Field*, Comparator>::iterator it);
  std::shared_mutex& getMutex() const {return mtx;}
  SharedRecursiveLock getLock(MtxLockType lock_type) const {return SharedRecursiveLock(&mtx, lock_type);}
  Error add(Field& field);
  Error remove(Field& field);

  bool operator==(const Index& other) const { return key == other.key; }
  bool operator!=(const Index& other) const { return key != other.key; }
  bool operator>(const Index& other) const { return key > other.key; }
  bool operator<(const Index& other) const { return key < other.key; }
  bool operator>=(const Index& other) const { return key >= other.key; }
  bool operator<=(const Index& other) const { return key <= other.key; }

  bool operator==(const KeyValue& key) const { return self.key == key; }
  bool operator!=(const KeyValue& key) const { return self.key != key; }
  bool operator>(const KeyValue& key) const { return self.key > key; }
  bool operator<(const KeyValue& key) const { return self.key < key; }
  bool operator>=(const KeyValue& key) const { return self.key >= key; }
  bool operator<=(const KeyValue& key) const { return self.key <= key; }

  Index(const Index&) = delete;
  Index(Index&& other) : type(other.type), key(std::move(other.key)) {
    switch(other.type) {
      case IndexType::unique_index:
        new(&data) IndexData(std::move(other.data.unique_index));
      return;
      case IndexType::multi_index:
        new(&data) IndexData(std::move(other.data.multi_index));
      return;
    }
  }

public:

  Index(IndexType type, KeyValue key);
  ~Index();

  class Iterator : public std::set<Field*, Comparator>::iterator {
  public:
    typedef std::set<Field*, Comparator>::iterator Base;
    typedef Base& BaseRef;
    typedef Base&& BaseTmp;
    typedef Iterator Self;
    typedef Self& SelfRef;
    typedef Self&& SelfTmp;

  private:
    inline const BaseRef upcast() const {return const_cast<const BaseRef>(dynamic_cast<BaseRef>(const_cast<SelfRef>(self)));}
    inline BaseRef upcast() {return dynamic_cast<BaseRef>(self);}
    static inline SelfRef downcast(BaseRef base) {return reinterpret_cast<SelfRef>(base);}
    static inline Self downcast(BaseTmp base) {return reinterpret_cast<SelfTmp>(base);}

    Index* index_ptr;

  public:
    Iterator(Index* index_ptr, Base map_it);
    Iterator(const Iterator& other);
    Iterator(Iterator&& other);

    FieldRef operator*();
    pseudo_ptr::PseudoPointer<FieldRef> operator->();
    const FieldRef operator*() const;
    pseudo_ptr::PseudoPointer<const FieldRef> operator->() const;

    inline SelfRef operator++() {return downcast(++upcast());}
    inline Self operator++(int) {return downcast(upcast()++);}
    inline SelfRef operator--() {return downcast(--upcast());}
    inline Self operator--(int) {return downcast(upcast()--);}
    inline bool operator=(const SelfRef other) { this->~Iterator(); return new(this) Iterator(other); }
    inline bool operator=(SelfTmp other) { this->~Iterator(); return new(this) Iterator(std::move(other)); }

    inline bool operator==(Self other) const {return upcast() == other.upcast();}
    inline bool operator!=(Self other) const {return upcast() != other.upcast();}
  };

  class ConstIterator : public std::set<Field*, Comparator>::const_iterator {
  public:
    typedef std::set<Field*, Comparator>::const_iterator Base;
    typedef Base& BaseRef;
    typedef Base&& BaseTmp;
    typedef ConstIterator Self;
    typedef Self& SelfRef;
    typedef Self&& SelfTmp;

  private:
    inline const BaseRef upcast() const {return const_cast<const BaseRef>(dynamic_cast<BaseRef>(const_cast<SelfRef>(self)));}
    inline BaseRef upcast() {return dynamic_cast<BaseRef>(self);}
    static inline SelfRef downcast(BaseRef base) {return reinterpret_cast<SelfRef>(base);}
    static inline Self downcast(BaseTmp base) {return reinterpret_cast<SelfTmp>(base);}

    Index* index_ptr;
  public:
    ConstIterator(const Index* index_ptr, Base map_it);
    ConstIterator(const ConstIterator& other);
    ConstIterator(ConstIterator&& other);

    const FieldRef operator*() const;
    pseudo_ptr::PseudoPointer<const FieldRef> operator->() const;

    inline SelfRef operator++() {return downcast(++upcast());}
    inline Self operator++(int) {return downcast(upcast()++);}
    inline SelfRef operator--() {return downcast(--upcast());}
    inline Self operator--(int) {return downcast(upcast()--);}
    inline bool operator=(const SelfRef other) { this->~ConstIterator(); return new(this) ConstIterator(other); }
    inline bool operator=(SelfTmp other) { this->~ConstIterator(); return new(this) ConstIterator(std::move(other)); }

    inline bool operator==(Self other) const {return upcast() == other.upcast();}
    inline bool operator!=(Self other) const {return upcast() != other.upcast();}
  };

  class ReverseIterator : public std::set<Field*, Comparator>::reverse_iterator {
  public:
    typedef std::set<Field*, Comparator>::reverse_iterator Base;
    typedef Base& BaseRef;
    typedef Base&& BaseTmp;
    typedef ReverseIterator Self;
    typedef Self& SelfRef;
    typedef Self&& SelfTmp;

  private:
    inline const BaseRef upcast() const {return const_cast<const BaseRef>(dynamic_cast<BaseRef>(const_cast<SelfRef>(self)));}
    inline BaseRef upcast() {return dynamic_cast<BaseRef>(self);}
    static inline SelfRef downcast(BaseRef base) {return reinterpret_cast<SelfRef>(base);}
    static inline Self downcast(BaseTmp base) {return reinterpret_cast<SelfTmp>(base);}

    Index* index_ptr;
  public:
    ReverseIterator(Index* index_ptr, Base map_it);
    ReverseIterator(const ReverseIterator& other);
    ReverseIterator(ReverseIterator&& other);

    FieldRef operator*();
    pseudo_ptr::PseudoPointer<FieldRef> operator->();
    const FieldRef operator*() const;
    pseudo_ptr::PseudoPointer<const FieldRef> operator->() const;

    inline SelfRef operator++() {return downcast(++upcast());}
    inline Self operator++(int) {return downcast(upcast()++);}
    inline SelfRef operator--() {return downcast(--upcast());}
    inline Self operator--(int) {return downcast(upcast()--);}
    inline bool operator=(const SelfRef other) { this->~ReverseIterator(); return new(this) ReverseIterator(other); }
    inline bool operator=(SelfTmp other) { this->~ReverseIterator(); return new(this) ReverseIterator(std::move(other)); }

    inline bool operator==(Self other) const {return upcast() == other.upcast();}
    inline bool operator!=(Self other) const {return upcast() != other.upcast();}
  };

  class ConstReverseIterator : public std::set<Field*, Comparator>::const_reverse_iterator {
  public:
    typedef std::set<Field*, Comparator>::const_reverse_iterator Base;
    typedef Base& BaseRef;
    typedef Base&& BaseTmp;
    typedef ConstReverseIterator Self;
    typedef Self& SelfRef;
    typedef Self&& SelfTmp;

  private:
    inline const BaseRef upcast() const {return const_cast<const BaseRef>(dynamic_cast<BaseRef>(const_cast<SelfRef>(self)));}
    inline BaseRef upcast() {return dynamic_cast<BaseRef>(self);}
    static inline SelfRef downcast(BaseRef base) {return reinterpret_cast<SelfRef>(base);}
    static inline Self downcast(BaseTmp base) {return reinterpret_cast<SelfTmp>(base);}

    Index* index_ptr;
  public:
    ConstReverseIterator(const Index* index_ptr, Base map_it);
    ConstReverseIterator(const ConstReverseIterator& other);
    ConstReverseIterator(ConstReverseIterator&& other);

    const FieldRef operator*() const;
    pseudo_ptr::PseudoPointer<const FieldRef> operator->() const;

    inline SelfRef operator++() {return downcast(++upcast());}
    inline Self operator++(int) {return downcast(upcast()++);}
    inline SelfRef operator--() {return downcast(--upcast());}
    inline Self operator--(int) {return downcast(upcast()--);}
    inline bool operator=(const SelfRef other) { this->~ConstReverseIterator(); return new(this) ConstReverseIterator(other); }
    inline bool operator=(SelfTmp other) { this->~ConstReverseIterator(); return new(this) ConstReverseIterator(std::move(other)); }

    inline bool operator==(Self other) const {return upcast() == other.upcast();}
    inline bool operator!=(Self other) const {return upcast() != other.upcast();}
  };

  KeyValue getKey() const;
  IndexType getType() const;
  ui64 getIndexedFieldsCount() const;

  std::pair<Iterator, Iterator> getRange(KeyValue key) {
    return std::pair<Iterator, Iterator>(
      find(key),
      ++findLast(key)
    );
  }

  std::pair<ReverseIterator, ReverseIterator> getReverseRange(KeyValue key) {
    return std::pair<ReverseIterator, ReverseIterator>(
      rfind(key),
      ++rfindLast(key)
    );
  }

  Iterator find(KeyValue key) {
    return Iterator(
      this,
      type == IndexType::unique_index
      ? data.unique_index.find(key)
      : data.multi_index.find(key)
    );
  }

  ReverseIterator rfind(KeyValue key) {
    return ReverseIterator(
      this,
      std::set<Field*, Comparator>::reverse_iterator(
        type == IndexType::unique_index
        ? --data.unique_index.upper_bound(key)
        : --data.multi_index.upper_bound(key)
      )
    );
  }

  Iterator findLast(KeyValue key) {
    return Iterator(
      this,
      type == IndexType::unique_index
      ? --data.unique_index.upper_bound(key)
      : --data.multi_index.upper_bound(key)
    );
  }

  ReverseIterator rfindLast(KeyValue key) {
    return ReverseIterator(
      this,
      std::set<Field*, Comparator>::reverse_iterator(
        type == IndexType::unique_index
        ? data.unique_index.find(key)
        : data.multi_index.find(key)
      )
    );
  }

  std::pair<ConstIterator, ConstIterator> getRange(KeyValue key) const {
    return std::pair<ConstIterator, ConstIterator>{
      find(key),
      ++findLast(key)
    };
  }

  std::pair<ConstReverseIterator, ConstReverseIterator> getReverseRange(KeyValue key) const {
    return std::pair<ConstReverseIterator, ConstReverseIterator>{
      rfind(key),
      ++rfindLast(key)
    };
  }

  std::pair<ConstIterator, ConstIterator> getLowerRange(KeyValue key) const {
    return std::pair<ConstIterator, ConstIterator>{
      begin(),
      find(std::move(key))
    };
  }

  std::pair<ConstReverseIterator, ConstReverseIterator> getLowerReverseRange(KeyValue key) const {
    return std::pair<ConstReverseIterator, ConstReverseIterator>{
      ++rfindLast(std::move(key)),
      rend()
    };
  }

  std::pair<ConstIterator, ConstIterator> getLowerEqualRange(KeyValue key) const {
    return std::pair<ConstIterator, ConstIterator>{
      begin(),
      ++findLast(std::move(key))
    };
  }

  std::pair<ConstReverseIterator, ConstReverseIterator> getLowerEqualReverseRange(KeyValue key) const {
    return std::pair<ConstReverseIterator, ConstReverseIterator>{
      ++rfind(std::move(key)),
      rend()
    };
  }

  std::pair<ConstIterator, ConstIterator> getHigherRange(KeyValue key) const {
    return std::pair<ConstIterator, ConstIterator>{
      ++findLast(std::move(key)),
      end()
    };
  }

  std::pair<ConstReverseIterator, ConstReverseIterator> getHigherReverseRange(KeyValue key) const {
    return std::pair<ConstReverseIterator, ConstReverseIterator>{
      rbegin(),
      rfind(std::move(key))
    };
  }

  std::pair<ConstIterator, ConstIterator> getHigherEqualRange(KeyValue key) const {
    return std::pair<ConstIterator, ConstIterator>{
      find(std::move(key)),
      end()
    };
  }

  std::pair<ConstReverseIterator, ConstReverseIterator> getHigherEqualReverseRange(KeyValue key) const {
    return std::pair<ConstReverseIterator, ConstReverseIterator>{
      rbegin(),
      ++rfindLast(std::move(key))
    };
  }

  std::pair<
    std::pair<ConstIterator, ConstIterator>,
    std::pair<ConstIterator, ConstIterator>
  > getNegativeRange(KeyValue key) const {
    return std::pair<std::pair<ConstIterator, ConstIterator>, std::pair<ConstIterator, ConstIterator>> {
      getLowerRange(key), getHigherRange(key)
    };
  }

  std::pair<
    std::pair<ConstReverseIterator, ConstReverseIterator>,
    std::pair<ConstReverseIterator, ConstReverseIterator>
  > getNegativeReverseRange(KeyValue key) const {
    return std::pair<std::pair<ConstReverseIterator, ConstReverseIterator>, std::pair<ConstReverseIterator, ConstReverseIterator>> {
      getHigherReverseRange(key), getLowerReverseRange(key)
    };
  }

  ConstIterator find(KeyValue key) const {
    return ConstIterator(
      this,
      type == IndexType::unique_index
      ? data.unique_index.find(key)
      : data.multi_index.find(key)
    );
  }

  ConstReverseIterator rfind(KeyValue key) const {
    return ConstReverseIterator(
          this,
          std::set<Field*, Comparator>::reverse_iterator(
            type == IndexType::unique_index
            ? --data.unique_index.upper_bound(key)
            : --data.multi_index.upper_bound(key)
          )
        );
  }

  ConstIterator findLast(KeyValue key) const {
    return ConstIterator(
      this,
      type == IndexType::unique_index
      ? --data.unique_index.upper_bound(key)
      : --data.multi_index.upper_bound(key)
    );
  }

  ConstReverseIterator rfindLast(KeyValue key) const {
    return ConstReverseIterator(
      this,
      std::set<Field*, Comparator>::const_reverse_iterator(
        type == IndexType::unique_index
        ? data.unique_index.find(key)
        : data.multi_index.find(key)
      )
    );
  }

  Variable getFirstMapByKey(KeyValue key);
  const Variable getFirstMapByKey(KeyValue key) const;

  inline Variable operator[](KeyValue key) { return getFirstMapByKey(std::move(key)); }
  inline const Variable operator[](KeyValue key) const { return getFirstMapByKey(std::move(key)); }

  Iterator begin() {return Iterator(this, type == IndexType::unique_index ? data.unique_index.begin() : data.multi_index.begin());}
  Iterator end() {return Iterator(this, type == IndexType::unique_index ? data.unique_index.end() : data.multi_index.end());}
  ConstIterator begin() const {return ConstIterator(this, type == IndexType::unique_index ? data.unique_index.begin() : data.multi_index.begin());}
  ConstIterator end() const {return ConstIterator(this, type == IndexType::unique_index ? data.unique_index.end() : data.multi_index.end());}
  ConstIterator cbegin() const {return ConstIterator(this, type == IndexType::unique_index ? data.unique_index.cbegin() : data.multi_index.cbegin());}
  ConstIterator cend() const {return ConstIterator(this, type == IndexType::unique_index ? data.unique_index.cend() : data.multi_index.cend());}
  ReverseIterator rbegin() {return ReverseIterator(this, type == IndexType::unique_index ? data.unique_index.rbegin() : data.multi_index.rbegin());}
  ReverseIterator rend() {return ReverseIterator(this, type == IndexType::unique_index ? data.unique_index.rend() : data.multi_index.rend());}
  ConstReverseIterator rbegin() const {return ConstReverseIterator(this, type == IndexType::unique_index ? data.unique_index.crbegin() : data.multi_index.crbegin());}
  ConstReverseIterator rend() const {return ConstReverseIterator(this, type == IndexType::unique_index ? data.unique_index.crend() : data.multi_index.crend());}

};

struct IndexComparator {
  using is_transparent = void;
  inline bool operator()(const KeyValue& search_value, const Index& index) const {return index > search_value;}
  inline bool operator()(const Index& index, const KeyValue& search_value) const {return index < search_value;}
  inline bool operator()(const Index& lhs, const Index& rhs) const {return lhs < rhs;}
};









class Field {
  friend Index;
  friend Index::Comparator;
  friend MapComparator;
  friend priv::TableImplementation;
private:

  union FieldData {

    struct LocalField {
      KeyValue key;
      Variable value;
    };

    struct IndexedField {
      std::map<Index*, std::set<Field*, Index::Comparator>::iterator> indexed_at;
      KeyValue value;

      TransactionLock makeTransaction(MtxLockType lock_type) {
        std::set<std::shared_mutex*> mtx_set;
        for(auto& index_entry : indexed_at) mtx_set.insert(&index_entry.first->getMutex());
        return TransactionLock(mtx_set, lock_type);
      }

      void unlinkFromIndexes() {
        auto lk = makeTransaction(MtxLockType::unique_locked);
        for(auto& index_entry : indexed_at) index_entry.first->unlink(index_entry.second);
        indexed_at.clear();
      }

      ~IndexedField() {unlinkFromIndexes();}
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
  bool isCanBeIndexed();

  Error addIndex(Index& index, std::set<Field*, Index::Comparator>::iterator self_iterator);
  Error removeIndex(Index& index);
  Error removeIndex(std::map<Index*, std::set<Field*, Index::Comparator>::iterator>::iterator entry_it);

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

  inline bool isIndexed() const { return type == FieldType::indexed; }

  Variable getOwner();
  const Variable getOwner() const;
};


struct MapComparator {
  using is_transparent = void;
  bool operator()(const KeyValue& search_value, const Field& cell) const;
  bool operator()(const Field& cell, const KeyValue& search_value) const;
  bool operator()(const Field& lhs, const Field& rhs) const;
};







// Map iterators

typedef std::set<index::Field, index::MapComparator> ContainerType;

class Iterator : public ContainerType::iterator {
public:
  typedef ContainerType::iterator Base;
  typedef Base& BaseRef;
  typedef Base&& BaseTmp;
  typedef Iterator Self;
  typedef Self& SelfRef;
  typedef Self&& SelfTmp;

private:
  inline BaseRef upcast() {return dynamic_cast<BaseRef>(self);}
  static inline SelfRef downcast(BaseRef base) {return reinterpret_cast<SelfRef>(base);}
  static inline Self downcast(BaseTmp base) {return reinterpret_cast<SelfTmp>(base);}

public:
  Iterator(ContainerType::iterator map_it);
  Iterator(const Iterator& other);
  Iterator(Iterator&& other);

  FieldRef operator*();
  pseudo_ptr::PseudoPointer<FieldRef> operator->();
  const FieldRef operator*() const;

  inline SelfRef operator++() {return downcast(++upcast());}
  inline Self operator++(int) {return downcast(upcast()++);}
  inline SelfRef operator--() {return downcast(--upcast());}
  inline Self operator--(int) {return downcast(upcast()--);}

  pseudo_ptr::PseudoPointer<const FieldRef> operator->() const;
  static Iterator nullit() noexcept {return  ContainerType::iterator(nullptr);}
};

class ReverseIterator : public ContainerType::reverse_iterator {
public:
  typedef ContainerType::reverse_iterator Base;
  typedef Base& BaseRef;
  typedef Base&& BaseTmp;
  typedef ReverseIterator Self;
  typedef Self& SelfRef;
  typedef Self&& SelfTmp;

private:
  inline BaseRef upcast() {return dynamic_cast<BaseRef>(self);}
  static inline SelfRef downcast(BaseRef base) {return reinterpret_cast<SelfRef>(base);}
  static inline Self downcast(BaseTmp base) {return reinterpret_cast<SelfTmp>(base);}

public:
  ReverseIterator(ContainerType::reverse_iterator map_rit);
  ReverseIterator(const ReverseIterator& other);
  ReverseIterator(ReverseIterator&& other);

  FieldRef operator*();
  pseudo_ptr::PseudoPointer<FieldRef> operator->();
  const FieldRef operator*() const;
  pseudo_ptr::PseudoPointer<const FieldRef> operator->() const;

  inline SelfRef operator++() {return downcast(++upcast());}
  inline Self operator++(int) {return downcast(upcast()++);}
  inline SelfRef operator--() {return downcast(--upcast());}
  inline Self operator--(int) {return downcast(upcast()--);}

  static ReverseIterator nullit() noexcept {return  ContainerType::reverse_iterator(Iterator::nullit());}
};

class ConstIterator : public ContainerType::const_iterator {
public:
  typedef ContainerType::const_iterator Base;
  typedef Base& BaseRef;
  typedef Base&& BaseTmp;
  typedef ConstIterator Self;
  typedef Self& SelfRef;
  typedef Self&& SelfTmp;

private:
  inline BaseRef upcast() {return dynamic_cast<BaseRef>(self);}
  static inline SelfRef downcast(BaseRef base) {return reinterpret_cast<SelfRef>(base);}
  static inline Self downcast(BaseTmp base) {return reinterpret_cast<SelfTmp>(base);}

public:
  ConstIterator(ContainerType::const_iterator map_it);
  ConstIterator(const ConstIterator& other);
  ConstIterator(ConstIterator&& other);

  const FieldRef operator*() const;
  pseudo_ptr::PseudoPointer<const FieldRef> operator->() const;

  inline SelfRef operator++() {return downcast(++upcast());}
  inline Self operator++(int) {return downcast(upcast()++);}
  inline SelfRef operator--() {return downcast(--upcast());}
  inline Self operator--(int) {return downcast(upcast()--);}

  static ConstIterator nullit() noexcept {return  ContainerType::const_iterator(nullptr);}
};

class ConstReverseIterator : public ContainerType::const_reverse_iterator {
public:
  typedef ContainerType::const_reverse_iterator Base;
  typedef Base& BaseRef;
  typedef Base&& BaseTmp;
  typedef ConstReverseIterator Self;
  typedef Self& SelfRef;
  typedef Self&& SelfTmp;

private:
  inline BaseRef upcast() {return dynamic_cast<BaseRef>(self);}
  static inline SelfRef downcast(BaseRef base) {return reinterpret_cast<SelfRef>(base);}
  static inline Self downcast(BaseTmp base) {return reinterpret_cast<SelfTmp>(base);}

public:
  ConstReverseIterator(ContainerType::const_reverse_iterator map_rit);
  ConstReverseIterator(const ConstReverseIterator& other);
  ConstReverseIterator(ConstReverseIterator&& other);

  const FieldRef operator*() const;
  pseudo_ptr::PseudoPointer<const FieldRef> operator->() const;

  inline SelfRef operator++() {return downcast(++upcast());}
  inline Self operator++(int) {return downcast(upcast()++);}
  inline SelfRef operator--() {return downcast(--upcast());}
  inline Self operator--(int) {return downcast(upcast()--);}

  static ConstReverseIterator nullit() noexcept {return  ContainerType::const_reverse_iterator(ConstIterator::nullit());}
};




};




#endif // INDEX_IMPL_HXX
