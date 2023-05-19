#include "libbinom/include/binom_impl/ram_storage_implementation/array_impl.hxx"
#include "libbinom/include/utils/util_functions.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;


ArrayImplementation::ArrayImplementation(const arr& value_list)
  : count(value_list.getSize()), capacity(calculateCapacity(count)) {
  auto it = begin();
  for(const auto& value : value_list)
    new(it++) Variable(value);
}

ArrayImplementation::ArrayImplementation(const ArrayImplementation& other)
  : count(other.count), capacity(other.capacity) {
  auto it = begin();
  for(const auto& value : other)
    new(it++) Variable(value);
}

size_t ArrayImplementation::calculateCapacity(size_t count) noexcept {
  return util_functions::getNearestPow2(sizeof(ArrayImplementation) + count * sizeof(Link));
}

ArrayImplementation* ArrayImplementation::create(const literals::arr& value_list) {
  return new(new byte[calculateCapacity(value_list.getSize())]) ArrayImplementation(value_list);
}

ArrayImplementation* ArrayImplementation::copy(const ArrayImplementation* other) {
  return new(new byte[other->capacity]) ArrayImplementation(*other);
}

size_t ArrayImplementation::getElementCount() const noexcept {return count;}
size_t ArrayImplementation::getCapacity() const noexcept {return capacity;}

size_t ArrayImplementation::getSize() const noexcept {return count * sizeof (Link);}

Variable* ArrayImplementation::getData() const { return reinterpret_cast<Variable*>(const_cast<ArrayImplementation*>(this + 1)); }

Variable ArrayImplementation::pushBack(ArrayImplementation*& implementation, Variable variable) {
  auto allocated_memory = increaseSize(implementation, 1);
  new(allocated_memory) Variable(std::move(variable));
  return allocated_memory->move();
}

ArrayImplementation::Iterator ArrayImplementation::pushBack(ArrayImplementation*& implementation, const literals::arr variable_list) {
  auto allocated_memory = increaseSize(implementation, variable_list.getSize());
  auto it = allocated_memory;
  for(const auto& variable : variable_list)
    new(it++) Variable(std::move(variable));
  return allocated_memory;
}

Variable ArrayImplementation::pushFront(ArrayImplementation*& implementation, Variable variable) {
  auto allocated_memory = insert(implementation, 0, 1);
  new(allocated_memory) Variable(std::move(variable));
  return allocated_memory->move();
}

ArrayImplementation::Iterator ArrayImplementation::pushFront(ArrayImplementation*& implementation, const literals::arr& variable_list) {
  auto allocated_memory = insert(implementation, 0, variable_list.getSize());
  auto it = allocated_memory;
  for(const auto& variable : variable_list)
    new(it++) Variable(std::move(variable));
  return allocated_memory;
}

Variable ArrayImplementation::insert(ArrayImplementation*& implementation, size_t at, Variable variable) {
  auto allocated_memory = insert(implementation, at, 1);
  new(allocated_memory) Variable(std::move(variable));
  return allocated_memory->move();
}

ArrayImplementation::Iterator ArrayImplementation::insert(ArrayImplementation*& implementation, size_t at, const literals::arr& variable_list) {
  auto allocated_memory = insert(implementation, at, variable_list.getSize());
  auto it = allocated_memory;
  for(const auto& variable : variable_list)
    new(it++) Variable(std::move(variable));
  return allocated_memory;
}

ArrayImplementation::Iterator ArrayImplementation::increaseSize(ArrayImplementation*& implementation, size_t count) {
  const size_t new_count = implementation->count + count;
  const size_t old_count = implementation->count;
  const size_t new_capacity = calculateCapacity(new_count);
  if(new_capacity == implementation->capacity) {
    ArrayImplementation* new_implementation = new(new byte[ new_capacity ]) ArrayImplementation(*implementation);
    new_implementation->count = new_count;
    new_implementation->capacity = new_capacity;
    delete implementation;
    implementation = new_implementation;
  } else {
    implementation->count = new_count;
  }
  return implementation->getData() + old_count;
}

void ArrayImplementation::reduceSize(ArrayImplementation*& implementation, size_t count) {
  if(count <= implementation->count) {
    implementation->count -= count;
    return;
  } else {
    implementation->count = 0;
    return;
  }
}

void ArrayImplementation::popBack(ArrayImplementation*& implementation, size_t count) {
  if(count <= implementation->count)
    for(auto it = implementation->getData() + implementation->count - count,
             end = implementation->end(); it != end; ++it)
      it->~Variable();
  else
    for(auto element : *implementation) element.~Variable();
  reduceSize(implementation, count);
}

void ArrayImplementation::popFront(ArrayImplementation*& implementation, size_t count) {remove(implementation, 0, count);}

ArrayImplementation::Iterator ArrayImplementation::insert(ArrayImplementation*& implementation, size_t at, size_t count) {
  size_t old_count = implementation->count;
  if(at >= old_count) return increaseSize(implementation, count);
  increaseSize(implementation, count);
  memmove(implementation->getData() + at + count,
          implementation->getData() + at,
          (old_count - at) * sizeof (Link));
  return implementation->getData() + at;
}

void ArrayImplementation::remove(ArrayImplementation*& implementation, size_t at, size_t count) {
  if(at >= implementation->count) return;
  if(at + count >= implementation->count)
    return popBack(implementation, implementation->count - at);

  for(auto it = implementation->getData() + at, end = implementation->getData() + at + count;
      it != end; ++it) it->~Variable();

  size_t old_count = implementation->count;
  memmove(implementation->getData() + at,
          implementation->getData() + at + count,
          (old_count - at - count) * sizeof (Link));
  return reduceSize(implementation, count);
}

void ArrayImplementation::clear(ArrayImplementation*& implementation) {popBack(implementation, implementation->getSize());}

Variable ArrayImplementation::operator[](size_t index) {
  if(index < getElementCount())
    return getData()[index].move();
  else return nullptr;
}

ArrayImplementation::Iterator ArrayImplementation::begin() {return getData();}
ArrayImplementation::Iterator ArrayImplementation::end() {return getData() + count;}
ArrayImplementation::ReverseIterator ArrayImplementation::rbegin() {return ArrayImplementation::ReverseIterator(getData() + count - 1);}
ArrayImplementation::ReverseIterator ArrayImplementation::rend() {return ArrayImplementation::ReverseIterator(getData() - 1);}
ArrayImplementation::ConstIterator ArrayImplementation::cbegin() const {return getData();}
ArrayImplementation::ConstIterator ArrayImplementation::cend() const {return getData() + count;}
ArrayImplementation::ConstReverseIterator ArrayImplementation::crbegin() const {return ArrayImplementation::ReverseIterator(getData() + count - 1);}
ArrayImplementation::ConstReverseIterator ArrayImplementation::crend() const {return ArrayImplementation::ReverseIterator(getData() - 1);}

void ArrayImplementation::operator delete(void* ptr) {
  for(auto element : *reinterpret_cast<ArrayImplementation*>(ptr)) element.~Variable();
  return ::delete [] reinterpret_cast<byte*>(ptr);
}
