#include "libbinom/include/utils/resource_control.hxx"
#include "libbinom/include/variables/variable.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;
using namespace extended_type_traits;

//////////////////////////////////////////////////////////// SharedResource ////////////////////////////////////////////////////////


bool SharedResource::isExist() noexcept {return link_counter;}

void SharedResource::destroy() {
  switch (toTypeClass(resource_data.type)) {
  case VarTypeClass::null:
  case VarTypeClass::number: return;
  case VarTypeClass::bit_array:
    if(resource_data.data.pointer)
      delete resource_data.data.bit_array_header;
    resource_data.data.pointer = nullptr;
  return;
  case VarTypeClass::buffer_array:
    if(resource_data.data.pointer)
      delete resource_data.data.buffer_array_header;
    resource_data.data.pointer = nullptr;
  return;
  case VarTypeClass::array:
    if(resource_data.data.pointer)
      delete resource_data.data.array_header;
    resource_data.data.pointer = nullptr;
  return;
  case VarTypeClass::singly_linked_list: // TODO
  case VarTypeClass::doubly_linked_list: // TODO
  case VarTypeClass::map: return; // TODO
  case VarTypeClass::invalid_type: default:
  return;
  }
}

SharedResource::SharedResource(ResourceData resource_data) : resource_data(resource_data) {}

SharedResource::~SharedResource() {destroy();}


//////////////////////////////////////////////////////////// Link ////////////////////////////////////////////////////////



Link::Link(ResourceData resource_data) noexcept : resource(new SharedResource(resource_data)) {}

Link::Link(Link&& other) noexcept : resource(other.resource) {other.resource = nullptr;}

Link::Link(const Link& other) noexcept {
  if(!other.resource) return;
  if(auto lk = other.getLock(MtxLockType::shared_locked); lk) {
    ++other.resource->link_counter;
    resource = other.resource;
  } else return;
}

Link::~Link() {
  SharedResource* old_resource = resource;
  resource = nullptr;
  if(old_resource) {
    if(!--old_resource->link_counter) {
      old_resource->mtx.lock();
      old_resource->mtx.unlock();
      delete old_resource;
    }
  }
}

void Link::overwriteWithResourceCopy(ResourceData& resource_data) {
  resource->destroy();
  resource->resource_data.type = resource_data.type;
  switch (toTypeClass(resource_data.type)) {
  case VarTypeClass::null: return;
  case VarTypeClass::number:
    resource->resource_data.data.ui64_val = resource_data.data.ui64_val;
  return;

  case VarTypeClass::bit_array:
    resource->resource_data.data.bit_array_header = BitArrayHeader::copy(resource_data.data.bit_array_header);
  return;

  case VarTypeClass::buffer_array:
    resource->resource_data.data.buffer_array_header = BufferArrayHeader::copy(resource_data.data.buffer_array_header);
  return;

  case VarTypeClass::array:
    resource->resource_data.data.array_header = ArrayHeader::copy(resource_data.data.array_header);
  return;

  case VarTypeClass::singly_linked_list: // TODO
  case VarTypeClass::doubly_linked_list: // TODO
  case VarTypeClass::map: // TODO
  default:
  case VarTypeClass::invalid_type:
  break;
  }
  resource->resource_data.type = VarType::null;
}

OptionalSharedRecursiveLock Link::getLock(MtxLockType lock_type) const noexcept {
  if(!resource) return OptionalSharedRecursiveLock();
  if(!resource->isExist()) return OptionalSharedRecursiveLock();
  return OptionalSharedRecursiveLock(SharedRecursiveLock(&resource->mtx, lock_type));
}

ResourceData* Link::operator*() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return &resource->resource_data;
  else return nullptr;
}

ResourceData* Link::operator->() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return &resource->resource_data;
  else return nullptr;
}

VarType Link::getType() const noexcept {
  if(auto lk = getLock(MtxLockType::shared_locked); lk)
    return resource->resource_data.type;
  else return VarType::invalid_type;
}

Link Link::cloneResource(Link resource_link) noexcept {
  switch (toTypeClass(resource_link.getType())) {
  case VarTypeClass::null:
  case VarTypeClass::number:
  return **resource_link;

  case VarTypeClass::bit_array:
  return ResourceData{VarType::bit_array, {.bit_array_header = BitArrayHeader::copy(resource_link->data.bit_array_header)}};

  case VarTypeClass::buffer_array:
  return ResourceData{resource_link.getType(), {.buffer_array_header = BufferArrayHeader::copy(resource_link->data.buffer_array_header)}};

  case VarTypeClass::array:
  return ResourceData{VarType::array, {.array_header = ArrayHeader::copy(resource_link->data.array_header)}};

  case VarTypeClass::singly_linked_list: // TODO
  case VarTypeClass::doubly_linked_list: // TODO
  case VarTypeClass::map: // TODO
  default:
  case VarTypeClass::invalid_type:
  break;
  }
  return ResourceData{VarType::null, {}};
}





//////////////////////////////////////////////////////////// BitArrayHeader ////////////////////////////////////////////////////////



BitArrayHeader::BitArrayHeader(const literals::bitarr& bit_array_data)
  : bit_size(bit_array_data.size()), capacity(calculateCapacity(bit_array_data.size())) {
  auto data_it = bit_array_data.begin();
  for(auto it = begin(), _end = end(); it != _end; ++it, ++data_it)
    (*it) = *data_it;
}

BitArrayHeader::BitArrayHeader(const BitArrayHeader& other)
  : bit_size(other.bit_size), capacity(other.capacity) {
  memcpy(getData(), other.getData(), getByteSize());
}

BitArrayHeader* BitArrayHeader::create(const literals::bitarr& bit_array_data) {
  return new(new byte[ calculateCapacity(bit_array_data.size()) ]) BitArrayHeader(bit_array_data);
}

BitArrayHeader* BitArrayHeader::copy(const BitArrayHeader* other) {
  return new(new byte[ other->capacity ]) BitArrayHeader(*other);
}

size_t BitArrayHeader::calculateByteSize(size_t bit_count) noexcept {
  return size_t(std::ceil(llf_t(bit_count)/ 8));
}

BitIterator BitArrayHeader::increaseSize(BitArrayHeader*& header, size_t bit_count) {
  const size_t new_bit_size = header->bit_size + bit_count;
  const size_t new_capacity = calculateCapacity(new_bit_size);
  if(header->capacity != new_capacity) {
    const size_t old_bit_size = header->bit_size;
    BitArrayHeader* old_pointer = header;
    header = new(new byte[ sizeof(BitArrayHeader) + new_capacity ]) BitArrayHeader(*old_pointer);
    header->bit_size = new_bit_size;
    header->capacity = new_capacity;
    delete old_pointer;
    return header->getData()[old_bit_size / 8].getItearatorAt(old_bit_size % 8);
  } else {
    BitIterator it = header->end();
    header->bit_size = new_bit_size;
    return it;
  }
}

void BitArrayHeader::reduceSize(BitArrayHeader*& header, size_t bit_count) {
  if(bit_count <= header->bit_size) {
    header->bit_size -= bit_count;
    return;
  } else {
    header->bit_size = 0;
    return;
  }
}

BitIterator BitArrayHeader::insertBits(BitArrayHeader*& header, size_t at, size_t count) {
  using namespace util_functions;
  const size_t shift_start_byte = at / 8;
  const size_t shift_end_byte = (at + count) / 8;
  const ui8 shift_start_bit_in_byte = at % 8;
  const ui8 shift_end_bit_in_byte = (at + count) % 8;

  priv::BitArrayHeader::increaseSize(header, count);
  ui8* data = header->getDataAs<ui8>();

  if(!shift_start_bit_in_byte && !shift_end_bit_in_byte) {
    doLeftShift(data, header->getByteSize() - shift_start_byte, count);
    return header->getData()[shift_start_byte].begin();
  }

  if(shift_start_byte != shift_end_byte || shift_start_byte != header->getByteSize() - 1) {
    doLeftShift(data + shift_start_byte + 1, header->getByteSize() - shift_start_byte - 1, count);
    const ui8 shift_loss_size = 8 - shift_start_bit_in_byte;

    if(shift_start_bit_in_byte >= shift_end_bit_in_byte) {
      data[shift_end_byte] =
          set0Before(data[shift_end_byte], shift_end_bit_in_byte + shift_loss_size) |
          (data[shift_start_byte] >> (8 - shift_end_bit_in_byte - shift_loss_size));
    } else {
      const size_t next_byte_shift_loss = shift_loss_size - (8 - shift_end_bit_in_byte);
      data[shift_end_byte + 1] =
          set0Before(data[shift_end_byte], next_byte_shift_loss) |
          data[shift_start_byte] >> (8 - next_byte_shift_loss);
      data[shift_end_byte] =
          set0From(data[shift_start_byte], shift_start_bit_in_byte) |
          (data[shift_start_byte] << (shift_end_bit_in_byte - shift_start_bit_in_byte));
    }

  } else {
    // If bit inserting only in last byte, without allocations new bytes after last byte
    data[shift_start_byte] =
        set0From(data[shift_start_byte], shift_start_bit_in_byte) |
        set0Before(data[shift_end_byte] << count, shift_end_bit_in_byte);
  }
  return header->getData()[shift_start_byte].getItearatorAt(shift_start_bit_in_byte);
}

void BitArrayHeader::removeBits(BitArrayHeader*& header, size_t at, size_t count) {
  using namespace util_functions;
  if(at >= header->bit_size) return;
  if(at + count >= header->bit_size)
    return priv::BitArrayHeader::reduceSize(header, count - (count - (header->bit_size - at)));

  {
    const size_t shift_start_byte = at / 8;
    const size_t shift_start_bit_in_byte = at % 8;
    const size_t shift_end_byte = (at + count) / 8;
    const size_t shift_end_bit_in_byte = (at + count) % 8;
    ui8* data = header->getDataAs<ui8>();

    if(!shift_start_bit_in_byte)
      doRightShift(data + shift_start_byte, header->getByteSize() - shift_start_byte, count);
    elif(8 - shift_start_bit_in_byte >= count) {
      /*if(8 - shift_start_bit_in_byte > count)
       * [abcdefgh][ijklmnop] rm 3 at 1 => [a###bcde][fghijklm]
       * [11111111] >> (8 - 1) = [10000000], [a###bcde] & [10000000] = [a0000000]
       * [a###bcde] >> 3 = [#bcde000], [11111111] << 1 = [01111111], [#bcde000] & [01111111] = [0bcde000]
       * [fghijklm] << (8 - 3) = [00000fgh]
       * [a0000000] | [0bcde000] = [abcde000], [abcde000] | [00000fgh] = [abcdefgh] - result
       if(8 - shift_start_bit_in_byte == count)
       * [abcdefgh][ijklmnop] rm 4 at 4 => [abcd####][efghijkl]
       * [11111111] >> (8 - 4) = [11110000], [abcd####] & [11110000] = [abcd0000]
       * [abcd####] >> 4 = [####0000], [11111111] << 4 = [00001111], [####0000] & [00001111] = [00000000]
       * [efghijkl] << (8 - 4) = [0000efgh]
       * [abcd0000] | [00000000] = [abcd0000], [abcd0000] | [0000efgh] = [abcdefgh] - result
       */

      data[shift_start_byte] =
          (data[shift_start_byte] & (0xFF >> (8 - shift_start_bit_in_byte))) |
          ((data[shift_start_byte] >> count) & (0xFF << shift_start_bit_in_byte)) |
          data[shift_start_byte + 1] << (8 - count);
      doRightShift(data + shift_start_byte + 1, header->getByteSize() - shift_start_byte - 1, count);
    } elif((8 - shift_start_bit_in_byte) > (8 - shift_end_bit_in_byte)) {
      /* Example:
       * [abcdefgh]...[ijklmnop] rm 10 at 4 => [abcd####]...[######ef][ghijklmn] (end_byte = n, end_bit_in_byte = 6)
       * [11111111] >> (8 - 4) = [11110000], [abcd####] & [11110000] = [abcd0000]
       * [11111111] << 6 = [00000011], [######ef] & [00000011] = [000000ef], [000000ef] >> (6 - 4) = [0000ef00]
       * [ghijklmn] << (8 - (6 - 4)) = [000000gh]
       * [abcd0000] | [0000ef00] = [abcdef00], [abcdef00] | [000000gh] = [abcdefgh]
      */

      data[shift_start_byte] =
          (data[shift_start_byte] & (0xFF >> (8 - shift_start_bit_in_byte))) |
          ((data[shift_end_byte] & (0xFF << shift_end_bit_in_byte)) >> (shift_end_bit_in_byte - shift_start_bit_in_byte)) |
          data[shift_end_byte + 1] << (8 - (shift_end_bit_in_byte - shift_start_bit_in_byte));
      doRightShift(data + shift_start_byte + 1, header->getByteSize() - shift_start_byte - 1, count);
    } else {
      /* Example:
       * [abcdefgh]...[ijklmnop] rm 6 at 4 => [abcd####]...[##efghjk] (end_byte = n, end_bit_in_byte = 2)
       * [11111111] >> (8 - 4) = [11110000], [abcd####] & [11110000] = [abcd0000]
       * [11111111] << 2 = [00111111], [##efghjk] & [00111111] = [00efghjk], [00efghjk] << (4 - 2) = [0000efgh]
       * [abcd0000] | [0000efgh] = [abcdefgh]
      */

      data[shift_start_byte] =
          set0From(data[shift_start_byte], shift_start_bit_in_byte) |
          (set0Before(data[shift_end_byte], shift_end_bit_in_byte) << (shift_start_bit_in_byte - shift_end_bit_in_byte));
      doRightShift(data + shift_start_byte + 1, header->getByteSize() - shift_start_byte - 1, count);
    }

    return priv::BitArrayHeader::reduceSize(header, count);
  }
}

void BitArrayHeader::shrinkToFit(BitArrayHeader*& header) {
  if(header->getByteSize() == header->capacity) return;
  header->capacity = sizeof(BitArrayHeader) + header->getByteSize();
  BitArrayHeader* old_pointer = header;
  header = new(new byte[ sizeof(BitArrayHeader) + header->capacity ]) BitArrayHeader(*old_pointer);
  delete old_pointer;
}

size_t BitArrayHeader::calculateCapacity(size_t bit_count) noexcept {
  return util_functions::getNearestPow2(sizeof(BitArrayHeader) + calculateByteSize(bit_count));
}

size_t BitArrayHeader::getCapacity() const noexcept {return capacity;}

size_t BitArrayHeader::getBitSize() const noexcept {return bit_size;}

size_t BitArrayHeader::getByteSize() const noexcept {return calculateByteSize(bit_size);}

Bits* BitArrayHeader::getData() const noexcept {
  return reinterpret_cast<Bits*>(const_cast<BitArrayHeader*>(this)) + sizeof(BitArrayHeader);
}

void BitArrayHeader::operator delete(void* ptr) {return ::delete [] reinterpret_cast<byte*>(ptr);}

BitValueRef BitArrayHeader::operator[](size_t index) const noexcept {return getData()[index / 8][index % 8];}

BitIterator BitArrayHeader::begin() const noexcept {return getData()->begin();}

BitIterator BitArrayHeader::end() const noexcept {return getData()[bit_size / 8].getItearatorAt(bit_size % 8);}

BitReverseIterator BitArrayHeader::rbegin() const noexcept {if(!bit_size) return rend(); return getData()[(bit_size - 1) / 8].getReverseIteratorAt((bit_size - 1) % 8);}

BitReverseIterator BitArrayHeader::rend() const noexcept {return getData()->rend();}



//////////////////////////////////////////////////////////// BufferArrayHeader ////////////////////////////////////////////////////////


BufferArrayHeader::BufferArrayHeader(const BufferArrayHeader& other)
  : size(other.size), capacity(other.capacity) {
  memcpy(getData(), other.getData(), size);
}

size_t BufferArrayHeader::calculateCapacity(size_t size) noexcept {
  return util_functions::getNearestPow2(sizeof(BufferArrayHeader) + size);
}

BufferArrayHeader* BufferArrayHeader::copy(const BufferArrayHeader* other) {
  return new(new byte[ sizeof(BufferArrayHeader) + other->capacity ]) BufferArrayHeader(*other);
}

void* BufferArrayHeader::getData() const { return const_cast<void*>(reinterpret_cast<const void*>(this + 1)); }

size_t BufferArrayHeader::getSize() const noexcept {return size;}

size_t BufferArrayHeader::getElementCount(VarBitWidth type) const noexcept {return size_t(std::ceil(llf_t(size)/ size_t(type)));}

size_t BufferArrayHeader::getCapacity() const noexcept {return capacity;}

void* BufferArrayHeader::increaseSize(BufferArrayHeader*& header, VarBitWidth type, size_t count) {
  const size_t new_size = header->size + count * size_t(type);
  const size_t old_size = header->size;
  const size_t new_capacity = calculateCapacity(new_size);
  if(new_capacity == header->capacity) {
    BufferArrayHeader* new_header = new(new byte[ new_capacity ]) BufferArrayHeader(*header);
    new_header->size = new_size;
    new_header->capacity = new_capacity;
    delete header;
    header = new_header;
  } else {
    header->size = new_size;
  }
  return header->getDataAs<byte>() + old_size;
}

void BufferArrayHeader::reduceSize(BufferArrayHeader*& header, VarBitWidth type, size_t count) {
  const size_t size = count * size_t(type);
  if(size <= header->size) {
    header->size -= size;
    return;
  } else {
    header->size = 0;
    return;
  }
}

void BufferArrayHeader::shrinkToFit(BufferArrayHeader*& header) {
  if(header->size == header->capacity) return;
  header->capacity = sizeof(BufferArrayHeader) + header->size;
  BufferArrayHeader* old_header = header;
  header = new(new byte[ header->capacity ]) BufferArrayHeader(*old_header);
  delete old_header;
}

void* BufferArrayHeader::insert(BufferArrayHeader*& header, VarBitWidth type, size_t at, size_t count) {
  size_t old_size = header->size;
  size_t from = at * size_t(type);
  if(from >= old_size) return increaseSize(header, type, count);
  increaseSize(header, type, count);
  memmove(header->getDataAs<byte>() + from + count * size_t(type),
          header->getDataAs<byte>() + from,
          old_size - from);
  return header->getDataAs<byte>() + from;
}

void BufferArrayHeader::remove(BufferArrayHeader*& header, VarBitWidth type, size_t at, size_t count) {
  size_t rm_size = count * size_t(type);
  size_t from = at * size_t(type);
  if(from >= header->size) return;
  if(from + rm_size >= header->size)
    return reduceSize(header, VarBitWidth::byte, header->size - from);
  size_t old_size = header->size;
  memmove(header->getDataAs<byte>() + from,
          header->getDataAs<byte>() + from + rm_size,
          old_size - from - rm_size);
  return reduceSize(header, type, count);
}

void* BufferArrayHeader::get(VarBitWidth type, size_t at) const {
  switch (type) {
  case VarBitWidth::byte: return getDataAs<byte>() + at;
  case VarBitWidth::word: return getDataAs<word>() + at;
  case VarBitWidth::dword: return getDataAs<dword>() + at;
  case VarBitWidth::qword: return getDataAs<qword>() + at;
  case VarBitWidth::invalid_type:
  default: return nullptr;
  }
}

void* BufferArrayHeader::getBeginPtr() const {return getData();}

void* BufferArrayHeader::getEndPtr(VarBitWidth type) const {return getDataAs<byte>() + getElementCount(type) * size_t(type);}

void* BufferArrayHeader::getReverseBeginPtr(VarBitWidth type) const {return getDataAs<byte>() + (i64(getElementCount(type)) - 1) * size_t(type);}

void* BufferArrayHeader::getReverseEndPtr(VarBitWidth type) const {return getDataAs<byte>() - size_t(type);}

void BufferArrayHeader::operator delete(void* ptr) {return ::delete [] reinterpret_cast<byte*>(ptr);}



//////////////////////////////////////////////////////////// ArrayHeader ////////////////////////////////////////////////////////



ArrayHeader::ArrayHeader(const arr& value_list)
  : count(value_list.getSize()), capacity(calculateCapacity(count)) {
  auto it = begin();
  for(const auto& value : value_list)
    new(it++) Variable(value);
}

ArrayHeader::ArrayHeader(const ArrayHeader& other)
  : count(other.count), capacity(other.capacity) {
  auto it = begin();
  for(const auto& value : other)
    new(it++) Variable(value);
}

size_t ArrayHeader::calculateCapacity(size_t count) noexcept {
  return util_functions::getNearestPow2(sizeof(ArrayHeader) + count * sizeof(Link));
}

ArrayHeader* ArrayHeader::create(const literals::arr& value_list) {
  return new(new byte[calculateCapacity(value_list.getSize())]) ArrayHeader(value_list);
}

ArrayHeader* ArrayHeader::copy(const ArrayHeader* other) {
  return new(new byte[other->capacity]) ArrayHeader(*other);
}

size_t ArrayHeader::getElementCount() const noexcept {return count;}
size_t ArrayHeader::getCapacity() const noexcept {return capacity;}

size_t ArrayHeader::getSize() const noexcept {return count * sizeof (Link);}

Variable* ArrayHeader::getData() const { return reinterpret_cast<Variable*>(const_cast<ArrayHeader*>(this + 1)); }

ArrayHeader::Iterator ArrayHeader::increaseSize(ArrayHeader*& header, size_t count) {
  const size_t new_count = header->count + count;
  const size_t old_count = header->count;
  const size_t new_capacity = calculateCapacity(new_count);
  if(new_capacity == header->capacity) {
    ArrayHeader* new_header = new(new byte[ new_capacity ]) ArrayHeader(*header);
    new_header->count = new_count;
    new_header->capacity = new_capacity;
    delete header;
    header = new_header;
  } else {
    header->count = new_count;
  }
  return header->getData() + old_count;
}

void ArrayHeader::reduceSize(ArrayHeader*& header, size_t count) {
  if(count <= header->count) {
    header->count -= count;
    return;
  } else {
    header->count = 0;
    return;
  }
}

void ArrayHeader::popBack(ArrayHeader*& header, size_t count) {
  if(count <= header->count)
    for(auto it = header->getData() + header->count - count,
             end = header->end(); it != end; ++it)
      it->~Variable();
  else
    for(auto element : *header) element.~Variable();
  reduceSize(header, count);
}

ArrayHeader::Iterator ArrayHeader::insert(ArrayHeader*& header, size_t at, size_t count) {
  size_t old_count = header->count;
  if(at >= old_count) return increaseSize(header, count);
  increaseSize(header, count);
  memmove(header->getData() + at + count,
          header->getData() + at,
          (old_count - at) * sizeof (Link));
  return header->getData() + at;
}

void ArrayHeader::remove(ArrayHeader*& header, size_t at, size_t count) {
  if(at >= header->count) return;
  if(at + count >= header->count)
    return popBack(header, header->count - at);

  for(auto it = header->getData() + at, end = header->getData() + at + count;
      it != end; ++it) it->~Variable();

  size_t old_count = header->count;
  memmove(header->getData() + at,
          header->getData() + at + count,
          (old_count - at - count) * sizeof (Link));
  return reduceSize(header, count);
}

Variable ArrayHeader::operator[](size_t index) {
  if(index < getElementCount())
    return getData()[index].getReference();
  else return nullptr;
}

ArrayHeader::Iterator ArrayHeader::begin() const {return getData();}
ArrayHeader::Iterator ArrayHeader::end() const {return getData() + count;}
ArrayHeader::ReverseIterator ArrayHeader::rbegin() const {return ArrayHeader::ReverseIterator(getData() + count - 1);}
ArrayHeader::ReverseIterator ArrayHeader::rend() const {return ArrayHeader::ReverseIterator(getData() - 1);}

void ArrayHeader::operator delete(void* ptr) {
  for(auto element : *reinterpret_cast<ArrayHeader*>(ptr)) element.~Variable();
  return ::delete [] reinterpret_cast<byte*>(ptr);
}

//////////////////////////////////////////////////////////// ArrayHeader ////////////////////////////////////////////////////////

struct SingleLinkedListHeader::Node {
  Variable value;
  Node* next = nullptr;
};

class SingleLinkedListHeader::Iterator {
  Node* prev = nullptr; //!< Required for insert
  Node* node;

  friend class SingleLinkedListHeader;
  Iterator(Node* node, Node* prev = nullptr) : prev(prev), node(node) {}

public:
  Iterator(const Iterator& other) : prev(other.prev), node(other.node) {}
  Iterator(const Iterator&& other) : prev(other.prev), node(other.node) {}

  Iterator& operator++() {if(node) { prev = node; node = node->next;} return self;}
  Iterator operator++(int) {Iterator tmp(self); ++self; return tmp;}

  Variable operator*() {if(node) return node->value.getReference(); else return nullptr;}
  Variable* operator->() {if(node) return &node->value; else return nullptr;}

  bool operator==(const Iterator& other) const noexcept {return node == other.node;}
  bool operator==(const Iterator&& other) const noexcept {return node == other.node;}
  bool operator!=(const Iterator& other) const noexcept {return node != other.node;}
  bool operator!=(const Iterator&& other) const noexcept {return node != other.node;}
};

SingleLinkedListHeader::SingleLinkedListHeader(const sllist& value_list) {pushBack(value_list);}

Variable SingleLinkedListHeader::pushBack(Variable var) {
  if(!last) {
    first = last = new Node{std::move(var), nullptr};
  } else {
    last = last->next = new Node{std::move(var), nullptr};
  }
  return last->value.getReference();
}

SingleLinkedListHeader::Iterator SingleLinkedListHeader::pushBack(const literals::sllist& value_list) {
  Iterator result(nullptr, nullptr);

  auto it = value_list.begin();

  if(!last) {
    result.node = first = last = new Node{std::move(*it), nullptr};
  } else {
    result.prev = last;
    result.node = last = last->next = new Node{std::move(*it), nullptr};
  }

  for(auto end = value_list.end(); it != end; ++it)
    if(!last) {
      first = last = new Node{std::move(*it), nullptr};
    } else {
      last = last->next = new Node{std::move(*it), nullptr};
    }

  return result;
}

Variable SingleLinkedListHeader::pushFront(Variable var) {
  if(!first) {
    first = last = new Node{std::move(var), nullptr};
  } else {
    first = new Node{std::move(var), first};
  }
  return first->value.getReference();
}

SingleLinkedListHeader::Iterator SingleLinkedListHeader::pushFront(const literals::sllist& value_list) {
  Iterator result(nullptr);

  Node* last_first = first;
  Node** ptr_it = &first;

  for(const auto& value : value_list) {
    (*ptr_it) = new Node{std::move(value), nullptr};
    ptr_it = &(*ptr_it)->next;
  }

  (*ptr_it)->next = last_first;

  return first;
}

SingleLinkedListHeader::Iterator SingleLinkedListHeader::insert(Iterator it, Variable var) {
  if(!it.node && !it.prev) return it;
  elif(!it.node && it.prev == last) {
    last = it.prev = it.prev->next = new Node{std::move(var), it.node};
  } elif(it.prev) {
    it.prev->next = new Node{std::move(var), it.node};
    it.node = it.prev->next;
  } elif (first == it.node) {
    first = new Node{std::move(var), it.node};
    it.node = first;
  }
  return it;
}

SingleLinkedListHeader::Iterator SingleLinkedListHeader::remove(Iterator it) {
  if(!it.node) return it;
  Node* removable_node = it.node;
  if(it.prev) {
    it.node = it.prev->next = it.node->next;
    delete removable_node;
  } elif (first == it.node) {
    it.node = it.node->next;
    delete removable_node;
  }
  return it;
}
