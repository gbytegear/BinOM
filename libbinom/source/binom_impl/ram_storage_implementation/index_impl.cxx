#include "libbinom/include/binom_impl/ram_storage_implementation/index_impl.hxx"
#include "libbinom/include/variables/map.hxx"
#include "libbinom/include/variables/multi_map.hxx"

#include "libbinom/include/utils/shared_recursive_mutex_wrapper.hxx"

#include <cstring>

using namespace binom;
using namespace binom::priv;
using namespace binom::index;

// ==============================================================================================

binom::OptionalSharedRecursiveLock WeakLink::getLock(shared_recursive_mtx::MtxLockType lock_type) const noexcept {
  if(!resource) return OptionalSharedRecursiveLock();
  if(!resource->isExist()) return OptionalSharedRecursiveLock();
  return OptionalSharedRecursiveLock(SharedRecursiveLock(&resource->mtx, lock_type));
}

WeakLink::WeakLink(const SharedResource& resource)
  : resource(const_cast<SharedResource*>(&resource)) {}

WeakLink::WeakLink(const WeakLink& other)
  : resource(const_cast<WeakLink&>(other).resource) {}

WeakLink::WeakLink(WeakLink&& other)
  : resource(other.resource) {}

WeakLink::WeakLink(const Link& other)
  : resource(const_cast<Link&>(other).resource) {}

WeakLink::WeakLink(Link&& other)
  : resource(other.resource) {}

WeakLink& WeakLink::operator=(const SharedResource& resource) {return *new(this) WeakLink(resource);}

WeakLink& WeakLink::operator=(const WeakLink& other) {return *new(this) WeakLink(other);}

WeakLink& WeakLink::operator=(WeakLink&& other) {return *new(this) WeakLink(std::move(other));}

WeakLink& WeakLink::operator=(const Link& other) {return *new(this) WeakLink(other);}

WeakLink& WeakLink::operator=(Link&& other) {return *new(this) WeakLink(std::move(other));}

bool WeakLink::operator==(WeakLink other) const {return resource == other.resource;}

bool WeakLink::operator!=(WeakLink other) const {return resource == other.resource;}

bool WeakLink::operator==(const Link& other) const {return resource == other.resource;}

bool WeakLink::operator!=(const Link& other) const {return resource == other.resource;}

bool WeakLink::operator==(Link&& other) const {return resource == other.resource;}

bool WeakLink::operator!=(Link&& other) const {return resource == other.resource;}

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




// ==============================================================================================

Index::Index(IndexType type, KeyValue key) :
  type(type), key(std::move(key)), data(type) {}

Index::~Index() {
  clear();
  switch (type) {
  case IndexType::unique_index:
    data.unique_index.~set();
  return;
  case IndexType::multi_index:
    data.multi_index.~multiset();
  return;
  }
}

binom::KeyValue Index::getKey() const { return key; }

binom::IndexType Index::getType() const { return type; }

ui64 Index::getIndexedFieldsCount() const {
  return (type == IndexType::multi_index)
      ? data.multi_index.size()
      : data.unique_index.size();
}

std::pair<Index::Iterator, Index::Iterator> Index::getRange(KeyValue key) {
  return std::pair<Iterator, Iterator>(
        find(key),
        ++findLast(key)
        );
}

std::pair<Index::ReverseIterator, Index::ReverseIterator> Index::getReverseRange(KeyValue key) {
  return std::pair<ReverseIterator, ReverseIterator>(
        rfind(key),
        ++rfindLast(key)
        );
}

std::pair<Index::ConstIterator, Index::ConstIterator> Index::getRange(KeyValue key) const {
  return std::pair<ConstIterator, ConstIterator>{
    find(key),
        findEnd(key)
  };
}

std::pair<Index::ConstReverseIterator, Index::ConstReverseIterator> Index::getReverseRange(KeyValue key) const {
  return std::pair<ConstReverseIterator, ConstReverseIterator>{
    rfind(key),
        rfindEnd(key)
  };
}

std::pair<Index::ConstIterator, Index::ConstIterator> Index::getLowerRange(KeyValue key) const {
  return std::pair<ConstIterator, ConstIterator>{
    begin(),
        find(std::move(key))
  };
}

std::pair<Index::ConstReverseIterator, Index::ConstReverseIterator> Index::getLowerReverseRange(KeyValue key) const {
  return std::pair<ConstReverseIterator, ConstReverseIterator>{
    ++rfindLast(std::move(key)),
        rend()
  };
}

std::pair<Index::ConstIterator, Index::ConstIterator> Index::getLowerEqualRange(KeyValue key) const {
  return std::pair<ConstIterator, ConstIterator>{
    begin(),
        findEnd(std::move(key))
  };
}

std::pair<Index::ConstReverseIterator, Index::ConstReverseIterator> Index::getLowerEqualReverseRange(KeyValue key) const {
  return std::pair<ConstReverseIterator, ConstReverseIterator>{
    rfind(std::move(key)),
        rend()
  };
}

std::pair<Index::ConstIterator, Index::ConstIterator> Index::getHigherRange(KeyValue key) const {
  return std::pair<ConstIterator, ConstIterator>{
    findEnd(std::move(key)),
        end()
  };
}

std::pair<Index::ConstReverseIterator, Index::ConstReverseIterator> Index::getHigherReverseRange(KeyValue key) const {
  return std::pair<ConstReverseIterator, ConstReverseIterator>{
    rbegin(),
        rfind(std::move(key))
  };
}

std::pair<Index::ConstIterator, Index::ConstIterator> Index::getHigherEqualRange(KeyValue key) const {
  return std::pair<ConstIterator, ConstIterator>{
    find(std::move(key)),
        end()
  };
}

std::pair<Index::ConstReverseIterator, Index::ConstReverseIterator> Index::getHigherEqualReverseRange(KeyValue key) const {
  return std::pair<ConstReverseIterator, ConstReverseIterator>{
    rbegin(),
        rfindEnd(std::move(key))
  };
}

std::pair<
  std::pair<Index::ConstIterator, Index::ConstIterator>,
  std::pair<Index::ConstIterator, Index::ConstIterator>
> Index::getNegativeRange(KeyValue key) const {
  return std::pair<std::pair<ConstIterator, ConstIterator>, std::pair<ConstIterator, ConstIterator>> {
    getLowerRange(key), getHigherRange(key)
  };
}

std::pair<std::pair<Index::ConstReverseIterator, Index::ConstReverseIterator>, std::pair<Index::ConstReverseIterator, Index::ConstReverseIterator> > Index::getNegativeReverseRange(KeyValue key) const {
  return std::pair<
    std::pair<ConstReverseIterator, ConstReverseIterator>,
    std::pair<ConstReverseIterator, ConstReverseIterator>
  > {
    getHigherReverseRange(key), getLowerReverseRange(key)
};
}

Index::Iterator Index::find(KeyValue key) {
  return Iterator(
        this,
        type == IndexType::unique_index
        ? data.unique_index.find(key)
        : data.multi_index.find(key)
          );
}

Index::ConstIterator Index::find(KeyValue key) const {
  return ConstIterator(
        this,
        type == IndexType::unique_index
        ? data.unique_index.find(key)
        : data.multi_index.find(key)
          );
}

Index::ReverseIterator Index::rfind(KeyValue key) {
  return ReverseIterator(
        this,
        std::set<Field*, Comparator>::reverse_iterator(
          type == IndexType::unique_index
          ? --data.unique_index.upper_bound(key)
          : --data.multi_index.upper_bound(key)
            )
        );
}

Index::ConstReverseIterator Index::rfind(KeyValue key) const {
  return ConstReverseIterator(
        this,
        std::set<Field*, Comparator>::reverse_iterator(
          type == IndexType::unique_index
          ? --data.unique_index.upper_bound(key)
          : --data.multi_index.upper_bound(key)
            )
        );
}

Index::Iterator Index::findLast(KeyValue key) {
  return Iterator(
        this,
        type == IndexType::unique_index
        ? --data.unique_index.upper_bound(key)
        : --data.multi_index.upper_bound(key)
          );
}

Index::ConstIterator Index::findLast(KeyValue key) const {
  return ConstIterator(
        this,
        type == IndexType::unique_index
        ? --data.unique_index.upper_bound(key)
        : --data.multi_index.upper_bound(key)
          );
}

Index::ReverseIterator Index::rfindLast(KeyValue key) {
  return ReverseIterator(
        this,
        std::set<Field*, Comparator>::reverse_iterator(
          type == IndexType::unique_index
          ? data.unique_index.find(key)
          : data.multi_index.find(key)
            )
        );
}

Index::ConstReverseIterator Index::rfindLast(KeyValue key) const {
  return ConstReverseIterator(
        this,
        std::set<Field*, Comparator>::const_reverse_iterator(
          type == IndexType::unique_index
          ? data.unique_index.find(key)
          : data.multi_index.find(key)
            )
        );
}

Index::ConstIterator Index::findEnd(KeyValue key) const {
  return ConstIterator(
        this,
        type == IndexType::unique_index
        ? data.unique_index.upper_bound(key)
        : data.multi_index.upper_bound(key)
          );
}

Index::ConstReverseIterator Index::rfindEnd(KeyValue key) const {
  return ConstReverseIterator(
        this,
        std::set<Field*, Comparator>::const_reverse_iterator(
          type == IndexType::unique_index
          ? --data.unique_index.find(key)
          : --data.multi_index.find(key)
            )
        );
}

Variable Index::getFirstMapByKey(KeyValue key) {
  if(auto it = find(std::move(key)); it != end())
    return *it;
  else return nullptr;
}

const Variable Index::getFirstMapByKey(KeyValue key) const {
  if(auto it = find(std::move(key)); it != cend())
    return *it;
  else return nullptr;
}

Variable Index::operator[](KeyValue key) { return getFirstMapByKey(std::move(key)); }

const Variable Index::operator[](KeyValue key) const { return getFirstMapByKey(std::move(key)); }

Index::Iterator Index::begin() {return Iterator(this, type == IndexType::unique_index ? data.unique_index.begin() : data.multi_index.begin());}

Index::Iterator Index::end() {return Iterator(this, type == IndexType::unique_index ? data.unique_index.end() : data.multi_index.end());}

Index::ConstIterator Index::begin() const {return ConstIterator(this, type == IndexType::unique_index ? data.unique_index.begin() : data.multi_index.begin());}

Index::ConstIterator Index::end() const {return ConstIterator(this, type == IndexType::unique_index ? data.unique_index.end() : data.multi_index.end());}

Index::ConstIterator Index::cbegin() const {return ConstIterator(this, type == IndexType::unique_index ? data.unique_index.cbegin() : data.multi_index.cbegin());}

Index::ConstIterator Index::cend() const {return ConstIterator(this, type == IndexType::unique_index ? data.unique_index.cend() : data.multi_index.cend());}

Index::ReverseIterator Index::rbegin() {return ReverseIterator(this, type == IndexType::unique_index ? data.unique_index.rbegin() : data.multi_index.rbegin());}

Index::ReverseIterator Index::rend() {return ReverseIterator(this, type == IndexType::unique_index ? data.unique_index.rend() : data.multi_index.rend());}

Index::ConstReverseIterator Index::rbegin() const {return ConstReverseIterator(this, type == IndexType::unique_index ? data.unique_index.crbegin() : data.multi_index.crbegin());}

Index::ConstReverseIterator Index::rend() const {return ConstReverseIterator(this, type == IndexType::unique_index ? data.unique_index.crend() : data.multi_index.crend());}

binom::Error Index::add(Field& field) {
  SharedRecursiveLock lk(&mtx, MtxLockType::unique_locked);

  // Compare keys
  if(key != field.getKey()) return ErrorType::invalid_data;

  if(!field.isCanBeIndexed()) return ErrorType::binom_invalid_type;

  switch (type) {
  case IndexType::unique_index:
    if(auto result = data.unique_index.insert(&field); result.second)
      return field.addIndex(self, result.first);
    else return ErrorType::binom_key_unique_error;
    return ErrorType::no_error;

  case IndexType::multi_index:
    return field.addIndex(self, data.multi_index.insert(&field));
  return ErrorType::no_error;
  default: return ErrorType::invalid_data;
  }
}

Error Index::remove(Field& field) {
  SharedRecursiveLock lk(&mtx, MtxLockType::unique_locked);
  if(field.type != FieldType::indexed) return ErrorType::binom_out_of_range;

  if(auto entry_it = field.data.indexed.indexed_at.find(this);
     entry_it != field.data.indexed.indexed_at.cend()) {
    switch (type) {
    case IndexType::unique_index:
      data.unique_index.erase(entry_it->second);
    break;
    case IndexType::multi_index:
      data.multi_index.erase(entry_it->second);
    break;
    }
    field.removeIndex(entry_it);
    return ErrorType::no_error;
  } else return ErrorType::binom_out_of_range;
}

void Index::clear() {
  SharedRecursiveLock lk(&mtx, MtxLockType::unique_locked);
  switch (type) {
  case IndexType::unique_index:
    while(!data.unique_index.empty()) remove(**data.unique_index.begin());
  return;
  case IndexType::multi_index:
    while(!data.multi_index.empty()) remove(**data.multi_index.begin());
  return;
  }
}

bool Index::operator==(const Index &other) const { return key == other.key; }

bool Index::operator!=(const Index &other) const { return key != other.key; }

bool Index::operator>(const Index &other) const { return key > other.key; }

bool Index::operator<(const Index &other) const { return key < other.key; }

bool Index::operator>=(const Index &other) const { return key >= other.key; }

bool Index::operator<=(const Index &other) const { return key <= other.key; }

bool Index::operator==(const KeyValue &key) const { return self.key == key; }

bool Index::operator!=(const KeyValue &key) const { return self.key != key; }

bool Index::operator>(const KeyValue &key) const { return self.key > key; }

bool Index::operator<(const KeyValue &key) const { return self.key < key; }

bool Index::operator>=(const KeyValue &key) const { return self.key >= key; }

bool Index::operator<=(const KeyValue &key) const { return self.key <= key; }

Index::Index(Index &&other) : type(other.type), key(std::move(other.key)) {
  switch(other.type) {
  case IndexType::unique_index:
    new(&data) IndexData(std::move(other.data.unique_index));
    return;
  case IndexType::multi_index:
    new(&data) IndexData(std::move(other.data.multi_index));
    return;
  }
}

binom::Error Index::unlink(std::set<Field*, Index::Comparator>::iterator it) {
  switch (type) {
  case IndexType::unique_index:
    data.unique_index.erase(it);
    return ErrorType::no_error;

  case IndexType::multi_index:
    data.multi_index.erase(it);
    return ErrorType::no_error;
  default: return ErrorType::invalid_data;
  }
}

std::shared_mutex &Index::getMutex() const {return mtx;}

shared_recursive_mtx::SharedRecursiveLock Index::getLock(shared_recursive_mtx::MtxLockType lock_type) const {return SharedRecursiveLock(&mtx, lock_type);}



// ==============================================================================================

bool Index::Comparator::operator()(const KeyValue& search_value, const Field* const& field) const {
  return search_value < field->data.indexed.value;
}

bool Index::Comparator::operator()(const Field* const& field, const KeyValue& search_value) const {
  return field->data.indexed.value < search_value;
}

bool Index::Comparator::operator()(const Field* const& lhs, const Field* const& rhs) const {
//  return lhs->data.indexed.value < rhs->data.indexed.value;
  auto getValue = [](const Field* const& field) -> KeyValue {
    switch (field->type) {
    case FieldType::local: return field->data.local.value;
    case FieldType::indexed: return field->data.indexed.value;
    default:
    case FieldType::empty: return nullptr;
    }
  };
  return getValue(lhs) < getValue(rhs);
}




// ==============================================================================================

void Field::setEmpty() {
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

bool Field::isCanBeIndexed() {
  return type == FieldType::empty
      ? false
      : type == FieldType::indexed
        ? true
        : toKeyType(data.local.value.getType()) != VarKeyType::invalid_type;
}

Error Field::addIndex(Index& index, std::set<Field*, Index::Comparator>::iterator self_iterator) {
  switch (type) {
  case FieldType::empty: return ErrorType::invalid_data;
  case FieldType::local: {
    KeyValue value = data.local.value;

    data.local.~LocalField();
    type = FieldType::indexed;
    new(&data.indexed) FieldData::IndexedField{
      .indexed_at{{&index, self_iterator}},
      .value = std::move(value)
    };
  } return ErrorType::no_error;


  case FieldType::indexed:
    data.indexed.indexed_at.emplace(&index, self_iterator);
  return ErrorType::no_error;
  default: return ErrorType::invalid_data;
  }
}

Error Field::removeIndex(Index& index) {
  switch (type) {
  default:
  case FieldType::empty:
  case FieldType::local: return ErrorType::binom_out_of_range;
  case FieldType::indexed:
    if(auto entry_it = data.indexed.indexed_at.find(&index); entry_it != data.indexed.indexed_at.cend()) {
      if(data.indexed.indexed_at.size() == 1) {
        KeyValue key = entry_it->first->key;
        Variable value = data.indexed.value;
        data.indexed.indexed_at.erase(entry_it);
        data.indexed.~IndexedField();
        new(&data.local) FieldData::LocalField {
          .key = std::move(key),
          .value = std::move(value)
        };
        type = FieldType::local;
      } else data.indexed.indexed_at.erase(entry_it);
      return ErrorType::no_error;
    } else return ErrorType::binom_out_of_range;
  }
}

Error Field::removeIndex(std::map<Index*, std::set<Field*, Index::Comparator>::iterator>::iterator entry_it) {
  switch (type) {
  default:
  case FieldType::empty:
  case FieldType::local: return ErrorType::binom_out_of_range;
  case FieldType::indexed:
    if(data.indexed.indexed_at.size() == 1) {
      KeyValue key = entry_it->first->key;
      Variable value = data.indexed.value;
      data.indexed.indexed_at.erase(entry_it);
      data.indexed.~IndexedField();
      type = FieldType::local;
      new(&data.local) FieldData::LocalField {
        .key = std::move(key),
        .value = value.move()
      };
    } else data.indexed.indexed_at.erase(entry_it);
  return ErrorType::no_error;
  }
}

Field::Field(binom::priv::WeakLink owner, KeyValue key, Variable value)
  : type(FieldType::local), data{.local{std::move(key), std::move(value)}}, owner(std::move(owner)) {}

Field::Field(Field&& other)
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
      .indexed_at = std::move(other.data.indexed.indexed_at),
      .value = std::move(other.data.indexed.value)
    };
    other.setEmpty();
  return;
  }
}

Field::~Field() {
  switch (type) {
  default:
  case FieldType::empty: return;
  case FieldType::local: data.local.~LocalField(); return;
  case FieldType::indexed: data.indexed.~IndexedField(); return;
  }
}

KeyValue Field::getKey() const {
  switch (type) {
  default:
  case FieldType::empty: return {};
  case FieldType::local: return data.local.key;
  case FieldType::indexed: return data.indexed.indexed_at.begin()->first->getKey();
  }
}

KeyValue Field::setKey(KeyValue key) {
  switch (type) {
  default:
  case FieldType::empty: return {};
  case FieldType::local: return data.local.key = key;
  case FieldType::indexed:
    data.indexed.unlinkFromIndexes();
    Variable owner_var = Link(owner);
    auto lk = owner_var.getLock(MtxLockType::unique_locked);

    bool is_reindexed = false;
    switch (owner_var.getType()) {
    case binom::VarType::map: is_reindexed = owner_var.toMap().updateKey(*this, key); break;
    case binom::VarType::multimap: is_reindexed = owner_var.toMultiMap().updateKey(*this, key); break;
    default: break;
    }

    if(!is_reindexed) {
      Variable value = data.indexed.value;
      data.indexed.~IndexedField();
      type = FieldType::local;
      new(&data.local) FieldData::LocalField {
        .key = std::move(key),
        .value = std::move(value)
      };
    }

  return key;
  }
}

Variable Field::getValue() {
  switch (type) {
  default:
  case FieldType::empty: return {};
  case FieldType::local: return data.local.value.move();
  case FieldType::indexed: return data.indexed.value;
  }
}

Variable Field::getValue() const {
  switch (type) {
  default:
  case FieldType::empty: return {};
  case FieldType::local: return data.local.value;
  case FieldType::indexed: return data.indexed.value;
  }
}

Variable Field::setValue(Variable value) {
  switch (type) {
  default:
  case FieldType::empty: return {};
  case FieldType::local: return (data.local.value = value).move();
  case FieldType::indexed:
    return data.indexed.value = value.move();
  }
}

Variable binom::index::Field::getOwner() { return Link(owner); }
const Variable binom::index::Field::getOwner() const { return Link(owner); }






// ==============================================================================================

bool MapComparator::operator()(const KeyValue& search_value, const Field& field) const {
  switch (field.type) {
  default: case FieldType::empty: return false;
  case FieldType::local: return search_value < field.data.local.key;
  case FieldType::indexed: return search_value < field.data.indexed.indexed_at.begin()->first->key;
  }
}

bool MapComparator::operator()(const Field& field, const KeyValue& search_value) const {
  switch (field.type) {
  default: case FieldType::empty: return true;
  case FieldType::local: return field.data.local.key < search_value;
  case FieldType::indexed: return field.data.indexed.indexed_at.begin()->first->key < search_value;
  }
}

bool MapComparator::operator()(const Field& lhs, const Field& rhs) const {
  switch (lhs.type) {
  default: case FieldType::empty: return rhs.type != FieldType::empty;
  case FieldType::local:
    switch (rhs.type) {
    default: case FieldType::empty: return false;
    case FieldType::local: return lhs.data.local.key < rhs.data.local.key;
    case FieldType::indexed: return lhs.data.local.key < rhs.data.indexed.indexed_at.begin()->first->key;
    }
  case FieldType::indexed: switch (rhs.type) {
    default: case FieldType::empty: return false;
    case FieldType::local: return lhs.data.indexed.indexed_at.begin()->first->key < rhs.data.local.key;
    case FieldType::indexed: return lhs.data.indexed.indexed_at.begin()->first->key < rhs.data.indexed.indexed_at.begin()->first->key;
    }
  }
}






// ==============================================================================================

Iterator::Iterator(ContainerType::iterator map_it) : ContainerType::iterator(map_it) {}

Iterator::Iterator(const Iterator& other) : ContainerType::iterator(dynamic_cast<const ContainerType::iterator&>(other)) {}

Iterator::Iterator(Iterator&& other) : ContainerType::iterator(dynamic_cast<ContainerType::iterator&&>(other)) {}

FieldRef Iterator::operator*() {return FieldRef(*dynamic_cast<ContainerType::iterator&>(self));}

pseudo_ptr::PseudoPointer<FieldRef> Iterator::operator->() {return *self;}

const FieldRef Iterator::operator*() const {return *dynamic_cast<const ContainerType::iterator&>(self);}

pseudo_ptr::PseudoPointer<const FieldRef> Iterator::operator->() const {return *self;}






ReverseIterator::ReverseIterator(ContainerType::reverse_iterator map_rit) : ContainerType::reverse_iterator(map_rit) {}

ReverseIterator::ReverseIterator(const ReverseIterator& other) : ContainerType::reverse_iterator(dynamic_cast<const ContainerType::reverse_iterator&>(other)) {}

ReverseIterator::ReverseIterator(ReverseIterator&& other) : ContainerType::reverse_iterator(dynamic_cast<ContainerType::reverse_iterator&&>(other)) {}

FieldRef ReverseIterator::operator*() {return *dynamic_cast<ContainerType::reverse_iterator&>(self);}

pseudo_ptr::PseudoPointer<FieldRef> ReverseIterator::operator->() {return *self;}

const FieldRef ReverseIterator::operator*() const {return *dynamic_cast<const ContainerType::reverse_iterator&>(self);}

pseudo_ptr::PseudoPointer<const FieldRef> ReverseIterator::operator->() const {return *self;}




ConstIterator::ConstIterator(ContainerType::const_iterator map_it) : ContainerType::const_iterator(map_it) {}

ConstIterator::ConstIterator(const ConstIterator& other) : ContainerType::const_iterator(dynamic_cast<const ContainerType::const_iterator&>(other)) {}

ConstIterator::ConstIterator(ConstIterator&& other) : ContainerType::const_iterator(dynamic_cast<ContainerType::const_iterator&&>(other)) {}

const FieldRef ConstIterator::operator*() const {return *dynamic_cast<const ContainerType::const_iterator&>(self);}

pseudo_ptr::PseudoPointer<const FieldRef> ConstIterator::operator->() const {return *self;}




ConstReverseIterator::ConstReverseIterator(ContainerType::const_reverse_iterator map_rit) : ContainerType::const_reverse_iterator(map_rit) {}

ConstReverseIterator::ConstReverseIterator(const ConstReverseIterator& other) : ContainerType::const_reverse_iterator(dynamic_cast<const ContainerType::const_reverse_iterator&>(other)) {}

ConstReverseIterator::ConstReverseIterator(ConstReverseIterator&& other) : ContainerType::const_reverse_iterator(dynamic_cast<ContainerType::const_reverse_iterator&&>(other)) {}

const FieldRef ConstReverseIterator::operator*() const {return *dynamic_cast<const ContainerType::const_reverse_iterator&>(self);}

pseudo_ptr::PseudoPointer<const FieldRef> ConstReverseIterator::operator->() const {return *self;}





// ==============================================================================================

Index::Iterator::Iterator(Index* index_ptr, Base map_it)
  : index_ptr(index_ptr), Base(map_it) {}

Index::Iterator::Iterator(const Iterator& other)
  : index_ptr(other.index_ptr), Base(other) {}

Index::Iterator::Iterator(Iterator&& other)
  : index_ptr(other.index_ptr), Base(other) {}

Variable Index::Iterator::operator*() {return (*dynamic_cast<const Base&>(self))->getOwner().move();}

pseudo_ptr::PseudoPointer<Variable> Index::Iterator::operator->() {return (*self).move();}

Variable Index::Iterator::operator*() const {return (*dynamic_cast<const Base&>(self))->getOwner();}

pseudo_ptr::PseudoPointer<Variable> Index::Iterator::operator->() const {return (*self).move();}





Index::ConstIterator::ConstIterator(const Index* index_ptr, Base map_it)
  : index_ptr(const_cast<Index*>(index_ptr)), Base(map_it) {}

Index::ConstIterator::ConstIterator(const ConstIterator& other)
  : index_ptr(other.index_ptr), Base(other) {}

Index::ConstIterator::ConstIterator(ConstIterator&& other)
  : index_ptr(other.index_ptr), Base(other) {}

Variable Index::ConstIterator::operator*() const {return (*dynamic_cast<const Base&>(self))->getOwner();}

pseudo_ptr::PseudoPointer<Variable> Index::ConstIterator::operator->() const {return (*self).move();}






Index::ReverseIterator::ReverseIterator(Index* index_ptr, Base map_it)
  : index_ptr(index_ptr), Base(map_it) {}

Index::ReverseIterator::ReverseIterator(const ReverseIterator& other)
  : index_ptr(other.index_ptr), Base(other) {}

Index::ReverseIterator::ReverseIterator(ReverseIterator&& other)
  : index_ptr(other.index_ptr), Base(other) {}

Variable Index::ReverseIterator::operator*() {return (*dynamic_cast<const Base&>(self))->getOwner().move();}

pseudo_ptr::PseudoPointer<Variable> Index::ReverseIterator::operator->() {return (*self).move();}

Variable Index::ReverseIterator::operator*() const {return (*dynamic_cast<const Base&>(self))->getOwner();}

pseudo_ptr::PseudoPointer<Variable> Index::ReverseIterator::operator->() const {return (*self).move();}




Index::ConstReverseIterator::ConstReverseIterator(const Index* index_ptr, Base map_it)
  : index_ptr(const_cast<Index*>(index_ptr)), Base(map_it) {}

Index::ConstReverseIterator::ConstReverseIterator(const ConstReverseIterator& other)
  : index_ptr(other.index_ptr), Base(other) {}

Index::ConstReverseIterator::ConstReverseIterator(ConstReverseIterator&& other)
  : index_ptr(other.index_ptr), Base(other) {}

Variable Index::ConstReverseIterator::operator*() const {return (*dynamic_cast<const Base&>(self))->getOwner();}

pseudo_ptr::PseudoPointer<Variable> Index::ConstReverseIterator::operator->() const {return (*self).move();}


// ==============================================================================================

Index::IndexData::IndexData() {}

Index::IndexData::IndexData(IndexType type) {
  switch (type) {
  case IndexType::unique_index:
    new(&unique_index) std::set<Field*, Comparator>();
    return;
  case IndexType::multi_index:
    new(&multi_index) std::multiset<Field*, Comparator>();
    return;
  }
}

Index::IndexData::IndexData(std::set<Field *, Comparator> &&data)
  : unique_index(std::move(data)) {}

Index::IndexData::IndexData(std::multiset<Field *, Comparator> &&data)
  : multi_index(std::move(data)) {}




shared_recursive_mtx::TransactionLock Field::FieldData::IndexedField::makeTransaction(shared_recursive_mtx::MtxLockType lock_type) {
  std::set<std::shared_mutex*> mtx_set;
  for(auto& index_entry : indexed_at) mtx_set.insert(&index_entry.first->getMutex());
  return TransactionLock(mtx_set, lock_type);
}

void Field::FieldData::IndexedField::unlinkFromIndexes() {
  auto lk = makeTransaction(MtxLockType::unique_locked);
  for(auto& index_entry : indexed_at) index_entry.first->unlink(index_entry.second);
  indexed_at.clear();
}

Field::FieldData::IndexedField::~IndexedField() {unlinkFromIndexes();}
