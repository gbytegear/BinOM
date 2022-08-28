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

std::vector<byte> Variable::serialize() const {
  std::vector<byte> buffer;
//  serializeImpl(self, buffer);
  return buffer;
}

void Variable::serializeToStraem(std::ostream& os) const {

}

//void Variable::serializeImpl(const Variable& variable, std::vector<byte>& buffer) {
//  auto lk = variable.getLock(MtxLockType::shared_locked);
//  if(!lk) return;

//  switch (variable.getTypeClass()) {

//  case binom::VarTypeClass::null:
//    buffer.pushBack(VarType::null);
//  return;

//  case binom::VarTypeClass::number:
//    serializeImpl(variable.toNumber(), buffer);
//  return;

//  case binom::VarTypeClass::bit_array:
//    serializeImpl(variable.toBitArray(), buffer);
//  return;

//  case binom::VarTypeClass::buffer_array:
//    serializeImpl(variable.toBufferArray(), buffer);
//  return;

//  case binom::VarTypeClass::array:
//    serializeImpl(variable.toArray(), buffer);
//  return;

//  case binom::VarTypeClass::singly_linked_list:
//    serializeImpl(variable.toSinglyLinkedList(), buffer);
//  return;

//  case binom::VarTypeClass::doubly_linked_list:
//    serializeImpl(variable.toDoublyLinkedList(), buffer);
//  return;

//  case binom::VarTypeClass::map:
//    serializeImpl(variable.toMap(), buffer);
//  return;

//  case binom::VarTypeClass::table:
////    TODO: serializeImpl(variable.toTable(), buffer);
//  return;

//  default:
//  case binom::VarTypeClass::invalid_type:
//    buffer.push_back(VarType::invalid_type);
//  break;

//  }
//}


//void Variable::serializeImpl(const Number& number, std::vector<byte>& buffer) {
//  auto lk = number.getLock(MtxLockType::shared_locked);
//  if(!lk) return;

//  buffer.push_back(number.getType());
//  switch (number.getValType()) {
//  case binom::ValType::boolean: buffer.push_back(bool(number)); return;
//  case binom::ValType::ui8:  buffer.push_back(ui8(number)); return;
//  case binom::ValType::si8:  buffer.push_back(i8(number)); return;
//  case binom::ValType::ui16: buffer.push_back(ui16(number)); return;
//  case binom::ValType::si16: buffer.push_back(i16(number)); return;
//  case binom::ValType::ui32: buffer.push_back(ui32(number)); return;
//  case binom::ValType::si32: buffer.push_back(i32(number)); return;
//  case binom::ValType::f32:  buffer.push_back(f32(number)); return;
//  case binom::ValType::ui64: buffer.push_back(ui64(number)); return;
//  case binom::ValType::si64: buffer.push_back(i64(number)); return;
//  case binom::ValType::f64:  buffer.push_back(f64(number)); return;
//  default:
//  case binom::ValType::invalid_type: return;
//  }
//}

//void Variable::serializeImpl(const BitArray& bit_array, std::vector<byte>& buffer) {
//  auto lk = bit_array.getLock(MtxLockType::shared_locked);
//  if(!lk) return;

//  buffer.pushBack(bit_array.getType());
//  size_t element_count = bit_array.getElementCount();
//  buffer.pushBack(element_count);
//  buffer.reserve(buffer.getCapacity() + element_count);
//  buffer.pushBack(bit_array.getDataPointer(), bit_array.getSize());
//}

//void Variable::serializeImpl(const BufferArray& buffer_array, std::vector<byte>& buffer) {
//  auto lk = buffer_array.getLock(MtxLockType::shared_locked);
//  if(!lk) return;

//}

//void Variable::serializeImpl(const Array& array, std::vector<byte>& buffer) {
//  auto lk = array.getLock(MtxLockType::shared_locked);
//  if(!lk) return;

//}

//void Variable::serializeImpl(const SinglyLinkedList& sl_list, std::vector<byte>& buffer) {
//  auto lk = sl_list.getLock(MtxLockType::shared_locked);
//  if(!lk) return;

//}

//void Variable::serializeImpl(const DoublyLinkedList& dl_list, std::vector<byte>& buffer) {
//  auto lk = dl_list.getLock(MtxLockType::shared_locked);
//  if(!lk) return;

//}

//void Variable::serializeImpl(const Map& map, std::vector<byte>& buffer) {
//  auto lk = map.getLock(MtxLockType::shared_locked);
//  if(!lk) return;

//}

//void Variable::serializeImpl(const Table& bit_array, BufferController& buffer) {

//}
