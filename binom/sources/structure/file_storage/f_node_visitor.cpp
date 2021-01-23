#include "binom/includes/structure/file_storage/f_node_visitor.h"

using namespace binom;

void FileNodeVisitor::load(ui64 index) {
  descriptor = vmemory->getNodeDescriptor(index);
}

FileNodeVisitor& FileNodeVisitor::stepInside(ui64 index) {
  if(getType() != VarType::array) throw SException(ErrCode::binom_invalid_type);
  ByteArray array_data = vmemory->getData(descriptor.descriptor);
  if(array_data.length()/sizeof(Element) >= index) throw SException(ErrCode::binom_out_of_range);
  load(reinterpret_cast<Element*>(array_data.begin() + index*sizeof(Element))->node_index);
  return *this;
}
