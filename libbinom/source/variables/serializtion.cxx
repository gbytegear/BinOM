#include "libbinom/include/variables/variable.hxx"
#include "libbinom/include/variables/number.hxx"
#include "libbinom/include/variables/bit_array.hxx"
#include "libbinom/include/variables/buffer_array.hxx"
#include "libbinom/include/variables/array.hxx"
#include "libbinom/include/variables/singly_linked_list.hxx"
#include "libbinom/include/variables/doubly_linked_list.hxx"
#include "libbinom/include/variables/map.hxx"

using namespace binom;
using namespace binom::priv;

BufferController Variable::serialize() const {
  BufferController buffer;
  serializeImpl(self, buffer);
  return buffer;
}

void Variable::serializeImpl(const Variable& variable, BufferController& buffer) {
  auto lk = variable.getLock(MtxLockType::shared_locked);
  if(!lk) return;

  switch (variable.getTypeClass()) {

  case binom::VarTypeClass::null:
    buffer.pushBack(VarType::null);
  return;

  case binom::VarTypeClass::number:
    serializeImpl(variable.toNumber(), buffer);
  return;

  case binom::VarTypeClass::bit_array:
    serializeImpl(variable.toBitArray(), buffer);
  return;

  case binom::VarTypeClass::buffer_array:
    serializeImpl(variable.toBufferArray(), buffer);
  return;

  case binom::VarTypeClass::array:
    serializeImpl(variable.toArray(), buffer);
  return;

  case binom::VarTypeClass::singly_linked_list:
    serializeImpl(variable.toSinglyLinkedList(), buffer);
  return;

  case binom::VarTypeClass::doubly_linked_list:
    serializeImpl(variable.toDoublyLinkedList(), buffer);
  return;

  case binom::VarTypeClass::map:
    serializeImpl(variable.toMap(), buffer);
  return;

  case binom::VarTypeClass::table:
//    TODO: serializeImpl(variable.toTable(), buffer);
  return;

  default:
  case binom::VarTypeClass::invalid_type:
    buffer.pushBack(VarType::invalid_type);
  break;

  }
}


void Variable::serializeImpl(const Number& number, BufferController& buffer) {
  auto lk = number.getLock(MtxLockType::shared_locked);
  if(!lk) return;

  buffer.pushBack(number.getType());
  switch (number.getValType()) {
  case binom::ValType::boolean: buffer.pushBack(bool(number)); return;
  case binom::ValType::ui8:  buffer.pushBack(ui8(number)); return;
  case binom::ValType::si8:  buffer.pushBack(i8(number)); return;
  case binom::ValType::ui16: buffer.pushBack(ui16(number)); return;
  case binom::ValType::si16: buffer.pushBack(i16(number)); return;
  case binom::ValType::ui32: buffer.pushBack(ui32(number)); return;
  case binom::ValType::si32: buffer.pushBack(i32(number)); return;
  case binom::ValType::f32:  buffer.pushBack(f32(number)); return;
  case binom::ValType::ui64: buffer.pushBack(ui64(number)); return;
  case binom::ValType::si64: buffer.pushBack(i64(number)); return;
  case binom::ValType::f64:  buffer.pushBack(f64(number)); return;
  default:
  case binom::ValType::invalid_type: return;
  }
}

void Variable::serializeImpl(const BitArray& bit_array, BufferController& buffer) {
  auto lk = bit_array.getLock(MtxLockType::shared_locked);
  if(!lk) return;

  buffer.pushBack(bit_array.getType());
  size_t element_count = bit_array.getElementCount();
  buffer.pushBack(element_count);
  buffer.reserve(buffer.getCapacity() + element_count);
  buffer.pushBack(bit_array.getDataPointer(), bit_array.getSize());
}

void Variable::serializeImpl(const BufferArray& buffer_array, BufferController& buffer) {
  auto lk = buffer_array.getLock(MtxLockType::shared_locked);
  if(!lk) return;

}

void Variable::serializeImpl(const Array& array, BufferController& buffer) {
  auto lk = array.getLock(MtxLockType::shared_locked);
  if(!lk) return;

}

void Variable::serializeImpl(const SinglyLinkedList& sl_list, BufferController& buffer) {
  auto lk = sl_list.getLock(MtxLockType::shared_locked);
  if(!lk) return;

}

void Variable::serializeImpl(const DoublyLinkedList& dl_list, BufferController& buffer) {
  auto lk = dl_list.getLock(MtxLockType::shared_locked);
  if(!lk) return;

}

void Variable::serializeImpl(const Map& map, BufferController& buffer) {
  auto lk = map.getLock(MtxLockType::shared_locked);
  if(!lk) return;

}

void Variable::serializeImpl(const Table& bit_array, BufferController& buffer) {

}
