#include "libbinom/include/binom_impl/ram_storage_implementation/index_impl.hxx"
#include "libbinom/include/variables/named_variable.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::index;


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






Index::Index(IndexType type, KeyValue key) :
  type(type), key(std::move(key)), data(type) {}

Index::~Index() {
  switch (type) {
  case IndexType::unique_index: data.unique_index.~set(); return;
  case IndexType::multi_index: data.multi_index.~multiset(); return;
  }
}

binom::KeyValue Index::getKey() const { return key; }

inline binom::Error Index::add(Field& field) {
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

inline binom::Error Index::unlink(Iterator it) {
  switch (type) {
  case IndexType::unique_index:
    data.unique_index.erase(it);
    return ErrorType::no_error;

  case IndexType::multi_index:
    data.multi_index.erase(it);
  return ErrorType::no_error;
  }
}




inline bool Index::Comparator::operator()(const KeyValue& search_value, const Field*& field) const {
  return search_value < field->data.indexed.value;
}

inline bool Index::Comparator::operator()(const Field*& field, const KeyValue& search_value) const {
  return field->data.indexed.value < search_value;
}

inline bool Index::Comparator::operator()(Field* const& lhs, Field* const& rhs) const {
  return lhs->data.indexed.value < rhs->data.indexed.value;
}





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

Error Field::addIndex(Index* index_ptr, Index::Iterator self_iterator) {
  switch (type) {
  case FieldType::empty: return ErrorType::invalid_data;
  case FieldType::local: {
    KeyValue value = data.local.value;

    data.local.~LocalField();
    new(&data.indexed) FieldData::IndexedField{
      .index_list{{index_ptr, self_iterator}},
      .value = std::move(value)
    };
  } return ErrorType::no_error;


  case FieldType::indexed:
    data.indexed.index_list.emplace_back(index_ptr, self_iterator);
  return ErrorType::no_error;
  }
}

bool Field::isCanBeIndexed() {
  return type == FieldType::empty
      ? false
      : type == FieldType::indexed
        ? true
        : toKeyType(data.local.value.getType()) != VarKeyType::invalid_type;
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
      .index_list = std::move(other.data.indexed.index_list),
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
  case FieldType::indexed: return data.indexed.index_list.front().index_ptr->getKey();
  }
}

KeyValue Field::setKey(KeyValue key) {
  switch (type) {
  default:
  case FieldType::empty: return {};
  case FieldType::local: return data.local.key = key;
  case FieldType::indexed:
  // TODO: Reindex
  // Get table list from owner
  // find in tables field(s) with name 'key'
  // * If field(s) is finded - add this field to index
  // * If field(s) isn't finded - change this field type to local

  return data.indexed.index_list.front().index_ptr->getKey(); // TEMPORARY STUB!!!
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
  case FieldType::indexed: return data.indexed.value = value.move();
  }
}



inline bool MapComparator::operator()(const KeyValue& search_value, const Field& field) const {
  switch (field.type) {
  default: case FieldType::empty: return false;
  case FieldType::local: return search_value < field.data.local.key;
  case FieldType::indexed: return search_value < field.data.indexed.index_list.front().index_ptr->key;
  }
}

inline bool MapComparator::operator()(const Field& field, const KeyValue& search_value) const {
  switch (field.type) {
  default: case FieldType::empty: return true;
  case FieldType::local: return field.data.local.key < search_value;
  case FieldType::indexed: return field.data.indexed.index_list.front().index_ptr->key < search_value;
  }
}

inline bool MapComparator::operator()(const Field& lhs, const Field& rhs) const {
  switch (lhs.type) {
  default: case FieldType::empty: return rhs.type != FieldType::empty;
  case FieldType::local:
    switch (rhs.type) {
    default: case FieldType::empty: return false;
    case FieldType::local: return lhs.data.local.key < rhs.data.local.key;
    case FieldType::indexed: return lhs.data.local.key < rhs.data.indexed.index_list.front().index_ptr->key;
    }
  case FieldType::indexed: switch (rhs.type) {
    default: case FieldType::empty: return false;
    case FieldType::local: return lhs.data.indexed.index_list.front().index_ptr->key < rhs.data.local.key;
    case FieldType::indexed: return lhs.data.indexed.index_list.front().index_ptr->key < rhs.data.indexed.index_list.front().index_ptr->key;
    }
  }
}








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
