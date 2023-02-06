#include "libbinom/include/binom_impl/ram_storage_implementation/index_impl.hxx"
#include "libbinom/include/variables/field.hxx"

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
  // TODO for-loop remove last *it
  switch (type) {
  case IndexType::unique_index:
    while(!data.unique_index.empty()) remove(**data.unique_index.begin());
//    for(auto field : data.unique_index) remove(*field);
    data.unique_index.~set();
  return;
  case IndexType::multi_index:
    while(!data.multi_index.empty()) remove(**data.multi_index.begin());
//    for(auto field : data.multi_index) remove(*field);
    data.multi_index.~multiset();
  return;
  }
}

binom::KeyValue Index::getKey() const { return key; }

binom::Error Index::add(Field& field) {

  // Compare keys
  if(key != field.getKey()) return ErrorType::invalid_data;

  if(!field.isCanBeIndexed()) return ErrorType::binom_invalid_type;
  if(field.data.local.key != key) return ErrorType::invalid_data;

  switch (type) {
  case IndexType::unique_index:
    if(auto result = data.unique_index.insert(&field); result.second)
      return field.addIndex(self, result.first);
    else return ErrorType::binom_key_unique_error;
    return ErrorType::no_error;

  case IndexType::multi_index:
    return field.addIndex(self, data.multi_index.insert(&field));
  return ErrorType::no_error;
  }
}

Error Index::remove(Field& field) {
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

binom::Error Index::unlink(std::set<Field*, Index::Comparator>::iterator it) {
  switch (type) {
  case IndexType::unique_index:
    data.unique_index.erase(it);
  return ErrorType::no_error;

  case IndexType::multi_index:
    data.multi_index.erase(it);
  return ErrorType::no_error;
  }
}




bool Index::Comparator::operator()(const KeyValue& search_value, const Field* const& field) const {
  return search_value < field->data.indexed.value;
}

bool Index::Comparator::operator()(const Field* const& field, const KeyValue& search_value) const {
  return field->data.indexed.value < search_value;
}

bool Index::Comparator::operator()(const Field* const& lhs, const Field* const& rhs) const {
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
  }
}

Error Field::removeIndex(Index& index) {
  switch (type) {
  case FieldType::empty: return ErrorType::binom_out_of_range;
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
          .value = value.move()
        };
        type = FieldType::local;
      } else data.indexed.indexed_at.erase(entry_it);
      return ErrorType::no_error;
    } else return ErrorType::binom_out_of_range;
  }
}

Error Field::removeIndex(std::map<Index*, std::set<Field*, Index::Comparator>::iterator>::iterator entry_it) {
  switch (type) {
  case FieldType::empty: return ErrorType::binom_out_of_range;
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

    // TODO: Reindex
    // Get table list from owner
    // find in tables field(s) with name 'key'
    // * If field(s) is finded - add this field to index
    // * If field(s) isn't finded - change this field type to local

  return data.indexed.indexed_at.begin()->first->getKey(); // TEMPORARY STUB!!!
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







Index::Iterator::Iterator(Index* index_ptr, Base map_it)
  : index_ptr(index_ptr), Base(map_it) {}

Index::Iterator::Iterator(const Iterator& other)
  : index_ptr(other.index_ptr), Base(other) {}

Index::Iterator::Iterator(Iterator&& other)
  : index_ptr(other.index_ptr), Base(other) {}

FieldRef Index::Iterator::operator*() {return **dynamic_cast<const Base&>(self);}

pseudo_ptr::PseudoPointer<FieldRef> Index::Iterator::operator->() {return *self;}

const FieldRef Index::Iterator::operator*() const {return **dynamic_cast<const Base&>(self);}

pseudo_ptr::PseudoPointer<const FieldRef> Index::Iterator::operator->() const {return *self;}





Index::ConstIterator::ConstIterator(const Index* index_ptr, Base map_it)
  : index_ptr(const_cast<Index*>(index_ptr)), Base(map_it) {}

Index::ConstIterator::ConstIterator(const ConstIterator& other)
  : index_ptr(other.index_ptr), Base(other) {}

Index::ConstIterator::ConstIterator(ConstIterator&& other)
  : index_ptr(other.index_ptr), Base(other) {}

const FieldRef Index::ConstIterator::operator*() const {return **dynamic_cast<const Base&>(self);}

pseudo_ptr::PseudoPointer<const FieldRef> Index::ConstIterator::operator->() const {return *self;}






Index::ReverseIterator::ReverseIterator(Index* index_ptr, Base map_it)
  : index_ptr(index_ptr), Base(map_it) {}

Index::ReverseIterator::ReverseIterator(const ReverseIterator& other)
  : index_ptr(other.index_ptr), Base(other) {}

Index::ReverseIterator::ReverseIterator(ReverseIterator&& other)
  : index_ptr(other.index_ptr), Base(other) {}

FieldRef Index::ReverseIterator::operator*() {return **dynamic_cast<const Base&>(self);}

pseudo_ptr::PseudoPointer<FieldRef> Index::ReverseIterator::operator->() {return *self;}

const FieldRef Index::ReverseIterator::operator*() const {return **dynamic_cast<const Base&>(self);}

pseudo_ptr::PseudoPointer<const FieldRef> Index::ReverseIterator::operator->() const {return *self;}





Index::ConstReverseIterator::ConstReverseIterator(const Index* index_ptr, Base map_it)
  : index_ptr(const_cast<Index*>(index_ptr)), Base(map_it) {}

Index::ConstReverseIterator::ConstReverseIterator(const ConstReverseIterator& other)
  : index_ptr(other.index_ptr), Base(other) {}

Index::ConstReverseIterator::ConstReverseIterator(ConstReverseIterator&& other)
  : index_ptr(other.index_ptr), Base(other) {}

const FieldRef Index::ConstReverseIterator::operator*() const {return **dynamic_cast<const Base&>(self);}

pseudo_ptr::PseudoPointer<const FieldRef> Index::ConstReverseIterator::operator->() const {return *self;}
