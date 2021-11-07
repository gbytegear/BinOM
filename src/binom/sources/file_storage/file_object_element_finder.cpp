#include "binom/includes/file_storage/file_object_element_finder.h"

using namespace binom;

ObjectDescriptor FileNodeVisitor::ObjectElementFinder::getObjectDescriptor() {
  ScopedRWGuard rwg(rw_gurad, LockType::shared_lock);
  return fmm.getNodeDataPart(node_index, 0, sizeof (ObjectDescriptor)).get<ObjectDescriptor>(0);
}

ObjectNameLength FileNodeVisitor::ObjectElementFinder::getNameBlockDescriptor(ui64 index) {
  ScopedRWGuard rwg(rw_gurad, LockType::shared_lock);
  return fmm.getNodeDataPart(node_index, sizeof (ObjectDescriptor) + sizeof (ObjectNameLength)*index, sizeof (ObjectNameLength)).get<ObjectNameLength>(0);
}

FileNodeVisitor::ObjectElementFinder::ObjectElementFinder(FileNodeVisitor& node_visitor)
  : fmm(node_visitor.fmm),
    node_index(node_visitor.node_index),
    rw_gurad(node_visitor.current_rwg),
    pos{getObjectDescriptor(), getNameBlockDescriptor(0), 0, 0, 0} {}

FileNodeVisitor::ObjectElementFinder::ObjectElementFinder(FileMemoryManager& fmm, virtual_index node_index, RWGuard& rw_gurad)
  : fmm(fmm),
    node_index(node_index),
    rw_gurad(rw_gurad),
    pos{getObjectDescriptor(), getNameBlockDescriptor(0), 0, 0, 0} {}

binom::FileNodeVisitor::ObjectElementFinder& FileNodeVisitor::ObjectElementFinder::dropPosition() {
  pos.name_block_index = 0;
  pos.name_shift = 0;
  pos.name_index = 0;
  pos.index = 0;
  pos.name_block_descriptor = getNameBlockDescriptor(0);
  return *this;
}

binom::FileNodeVisitor::ObjectElementFinder& FileNodeVisitor::ObjectElementFinder::findBlock(ValType type, block_size name_length) {
  if(pos.name_block_index)
    dropPosition();

  rw_gurad.lockShared();
  ByteArray name_blocks = fmm.getNodeDataPart(node_index, sizeof (ObjectDescriptor), sizeof (ObjectNameLength) * pos.descriptor.length_element_count);
  rw_gurad.unlock();

  for(auto it = name_blocks.begin<ObjectNameLength>(), end = name_blocks.end<ObjectNameLength>(); it != end; (++it, ++pos.name_block_index)) {
    if(it->char_type < type || it->name_length < name_length) {
      pos.index += it->name_count;
      pos.name_shift += toSize(it->char_type) * it->name_length * it->name_count;
    } elif(it->char_type == type && it->name_length == name_length) {
      pos.name_block_descriptor = *it;
      break;
    } else {
      pos.name_block_index = pos.descriptor.length_element_count;
      break;
    }
  }
  return *this;
}

binom::FileNodeVisitor::ObjectElementFinder& FileNodeVisitor::ObjectElementFinder::findNameInBlock(void* name) {
  ScopedRWGuard rwg(rw_gurad, LockType::shared_lock);
  const ui8 char_size = toSize(pos.name_block_descriptor.char_type);
  const i64 name_count = pos.name_block_descriptor.name_count;
  const ui64 name_byte_length = pos.name_block_descriptor.name_length*char_size;

  i64 middle = 0;
  i64 left = 0;
  i64 right = pos.name_block_descriptor.name_count;

  while(true) {
    middle = (left + right) / 2;

    if(left > right || middle > name_count) {
      middle = -1;
      break;
    }

    int cmp = memcmp(fmm.getNodeDataPart(node_index,
                                         sizeof (ObjectDescriptor) +
                                         sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                                         pos.name_shift+middle*name_byte_length,
                                         name_byte_length)
                     .begin(),
                     name,
                     name_byte_length);

    if(cmp > 0) right = middle - 1;
    elif(cmp < 0) left = middle + 1;
    else break;
  }

  if(middle == -1)
    pos.name_shift = pos.descriptor.name_block_size;
  else {
    pos.name_shift += middle*name_byte_length;
    pos.name_index = middle;
    pos.index += middle;
  }

  return *this;
}

bool FileNodeVisitor::ObjectElementFinder::findElement(BufferArray name) {
  if(!findBlock(name.getValType(), name.getMemberCount()).isNameBlockFinded()){
    dropPosition();
    return false;
  } elif(!findNameInBlock(name.getDataPointer()).isNameFinded()) {
    dropPosition();
    return false;
  } else return true;
}

FileNodeVisitor::NamePosition FileNodeVisitor::ObjectElementFinder::getNamePosition() {
  return NamePosition{
    node_index,
        pos.name_block_descriptor.char_type,
        pos.name_block_descriptor.name_length,
        sizeof (ObjectDescriptor) +
        sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
        pos.name_shift
  };
}

BufferArray FileNodeVisitor::ObjectElementFinder::getName() {
  ScopedRWGuard rwg(rw_gurad, LockType::shared_lock);
  return BufferArray(pos.name_block_descriptor.char_type,
                     fmm.getNodeDataPart(node_index, pos.name_shift, toSize(pos.name_block_descriptor.char_type)*pos.name_block_descriptor.name_length).begin(),
                     pos.name_block_descriptor.name_length);
}

virtual_index FileNodeVisitor::ObjectElementFinder::getNodeIndex() const {
  ScopedRWGuard rwg(rw_gurad, LockType::shared_lock);
  return fmm.getNodeDataPart(
        node_index,
        sizeof (ObjectDescriptor) +
        sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
        pos.descriptor.name_block_size +
        sizeof (virtual_index)*pos.index,
        sizeof (virtual_index))
      .get<virtual_index>(0);
}

void FileNodeVisitor::ObjectElementFinder::insert(BufferArray name, virtual_index new_node_index) {
  ScopedRWGuard rwg(rw_gurad, LockType::unique_lock);
  ObjectElementPosition pos{this->pos.descriptor, getNameBlockDescriptor(0)};
  const ValType type = name.getValType();
  const ui64 name_length = name.getMemberCount();

  NodeDescriptor node_descriptor = fmm.getNodeDescriptor(node_index);

  {
    ByteArray name_blocks = fmm.getNodeDataPart(node_index, sizeof (ObjectDescriptor), sizeof (ObjectNameLength) * pos.descriptor.length_element_count);
    for(auto it = name_blocks.begin<ObjectNameLength>(), end = name_blocks.end<ObjectNameLength>(); it != end; (++it, ++pos.name_block_index)) {
      if(it->char_type < type || it->name_length < name_length) {
        pos.index += it->name_count;
        pos.name_shift += toSize(it->char_type) * it->name_length * it->name_count;
      } elif(it->char_type == type && it->name_length == name_length) {
        pos.name_block_descriptor = *it;
        break;
      } else { // Insertion between name blocks
        ObjectNameLength name_length_block{type, name_length, 1};
        fmm.insertNodeDataPart(node_index,
                               {
                                 { // Insert name length block
                                   sizeof (ObjectDescriptor) +
                                   sizeof (ObjectNameLength)*pos.name_block_index,
                                   ByteArray(&name_length_block, sizeof (ObjectNameLength))

                                 },

                                 { // Insert name
                                   sizeof (ObjectDescriptor) +
                                   sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                                   pos.name_shift,
                                   name.toByteArray()
                                 },

                                 { // Insert index
                                   sizeof (ObjectDescriptor) +
                                   sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                                   pos.descriptor.name_block_size +
                                   sizeof (virtual_index) * pos.index,
                                   ByteArray(&new_node_index, sizeof (virtual_index))
                                 }

                               }, &node_descriptor);
        ++pos.descriptor.length_element_count;
        pos.descriptor.name_block_size += name.getDataSize();
        ++pos.descriptor.index_count;
        fmm.updateNodeDataPart(node_index, 0, ByteArray(&pos.descriptor, sizeof (ObjectDescriptor)), &node_descriptor);
        return;
      }
    }

    if(pos.name_block_index == pos.descriptor.length_element_count) {
      ObjectNameLength name_length_block{type, name_length, 1};
      fmm.insertNodeDataPart(node_index,
                             {
                               { // Insert name length block
                                 sizeof (ObjectDescriptor) +
                                 sizeof (ObjectNameLength)*pos.descriptor.length_element_count,
                                 ByteArray(&name_length_block, sizeof (ObjectNameLength))

                               },

                               { // Insert name
                                 sizeof (ObjectDescriptor) +
                                 sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                                 pos.descriptor.name_block_size,
                                 name.toByteArray()
                               },

                               { // Insert index
                                 sizeof (ObjectDescriptor) +
                                 sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                                 pos.descriptor.name_block_size +
                                 sizeof (virtual_index) * pos.descriptor.index_count,
                                 ByteArray(&new_node_index, sizeof (virtual_index))
                               }

                             }, &node_descriptor);
      ++pos.descriptor.length_element_count;
      pos.descriptor.name_block_size += name.getDataSize();
      ++pos.descriptor.index_count;
      fmm.updateNodeDataPart(node_index, 0, ByteArray(&pos.descriptor, sizeof (ObjectDescriptor)), &node_descriptor);
      return;
    }
  }

  const i64 name_count = pos.name_block_descriptor.name_count;
  const i64 name_byte_length = name_length * toSize(type);


  i64 middle = 0;
  i64 left = 0;
  i64 right = pos.name_block_descriptor.name_count;

  while(true) {
    middle = (left + right) / 2;

    if(left > right || middle > name_count) break;

    int cmp = memcmp(fmm.getNodeDataPart(node_index,
                                         sizeof (ObjectDescriptor) +
                                         sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                                         pos.name_shift+middle*name_byte_length,
                                         name_byte_length)
                     .begin(),
                     name.getDataPointer(),
                     name_byte_length);

    if(cmp > 0) right = middle - 1;
    elif(cmp < 0) left = middle + 1;
    else {
      dropPosition();
      throw Exception(ErrCode::binom_object_key_error);
    }
  }

  for(;(middle < name_count)
      ? memcmp(fmm.getNodeDataPart(node_index,
                                   sizeof (ObjectDescriptor) +
                                   sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                                   pos.name_shift+middle*name_byte_length,
                                   name_byte_length)
               .begin(),
               name.getDataPointer(),
               name_byte_length) < 0
      : false;
      ++middle);

  pos.name_shift += middle*name_byte_length;
  pos.index += middle;
  fmm.insertNodeDataPart(node_index,
                         {
                           { // Insert name
                             sizeof (ObjectDescriptor) +
                             sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                             pos.name_shift,
                             name.toByteArray()
                           },

                           { // Insert index
                             sizeof (ObjectDescriptor) +
                             sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                             pos.descriptor.name_block_size +
                             sizeof (virtual_index) * pos.index,
                             ByteArray(&new_node_index, sizeof (virtual_index))
                           }
                         }, &node_descriptor);
  ++pos.name_block_descriptor.name_count;
  fmm.updateNodeDataPart(node_index,
                         sizeof (ObjectDescriptor) +
                         sizeof (ObjectNameLength)*pos.name_block_index,
                         ByteArray(&pos.name_block_descriptor, sizeof(ObjectNameLength)),
                         &node_descriptor);
  ++pos.descriptor.index_count;
  pos.descriptor.name_block_size += name.getDataSize();
  fmm.updateNodeDataPart(node_index, 0, ByteArray(&pos.descriptor, sizeof (ObjectDescriptor)), &node_descriptor);
}

bool FileNodeVisitor::ObjectElementFinder::remove(BufferArray name) {
  ScopedRWGuard rwg(rw_gurad, LockType::unique_lock);
  ui64 name_size = name.getDataSize();
  if(!findElement(std::move(name))) {
    dropPosition();
    return false;
  }
  if(getElementCount() == 1) {
    fmm.updateNode(node_index, VarType::end, ByteArray());
    dropPosition();
    return true;
  }
  NodeDescriptor node_descriptor = fmm.getNodeDescriptor(node_index);
  ByteArray remove_indexes = fmm.getNodeDataPart(
        node_index,
        sizeof (ObjectDescriptor) +
        sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
        pos.descriptor.name_block_size +
        sizeof (virtual_index)*pos.index,
        sizeof (virtual_index));

  remove_indexes += FileNodeVisitor(fmm).getContainedNodeIndexes(remove_indexes.get<virtual_index>(0));

  fmm.removeNodeDataParts(node_index,
                          (!--pos.name_block_descriptor.name_count)
                          ?std::list<RMemoryBlock>{
                          {
                           sizeof (ObjectDescriptor) +
                           sizeof (ObjectNameLength)*pos.name_block_index,
                           sizeof (ObjectNameLength)
                          },
                          {
                           sizeof (ObjectDescriptor) +
                           sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                           pos.name_shift,
                           name_size
                          },
                          {
                           sizeof (ObjectDescriptor) +
                           sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                           pos.descriptor.name_block_size +
                           sizeof (virtual_index)*pos.index,
                           sizeof (virtual_index)
                          }
                         }
                         :std::list<RMemoryBlock>{
                          {
                           sizeof (ObjectDescriptor) +
                           sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                           pos.name_shift,
                           name_size
                          },
                          {
                           sizeof (ObjectDescriptor) +
                           sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                           pos.descriptor.name_block_size +
                           sizeof (virtual_index)*pos.index,
                           sizeof (virtual_index)
                          }
                         }, &node_descriptor);

  pos.descriptor.name_block_size -= name_size;
  --pos.descriptor.index_count;
  if(pos.name_block_descriptor.name_count) {
    fmm.updateNodeDataPart(node_index,
                           sizeof (ObjectDescriptor) +
                           sizeof (ObjectNameLength)*pos.name_block_index,
                           ByteArray(&pos.name_block_descriptor, sizeof (ObjectNameLength)),
                           &node_descriptor);
  } else --pos.descriptor.length_element_count;
  fmm.updateNodeDataPart(node_index, 0, ByteArray(&pos.descriptor, sizeof (ObjectDescriptor)), &node_descriptor);

  for(virtual_index* index_it = remove_indexes.begin<virtual_index>(),
      * index_end = remove_indexes.end<virtual_index>();
      index_it != index_end; ++index_it)
    fmm.removeNode(*index_it);

  dropPosition();
  return true;
}

void FileNodeVisitor::ObjectElementFinder::foreach(std::function<void (FileNodeVisitor::ObjectElementFinder::ObjectElement)> handler) {
  ScopedRWGuard rwg(rw_gurad, LockType::shared_lock);
  ObjectElementPosition pos{this->pos.descriptor, getNameBlockDescriptor(0)};
  ByteArray name_blocks = fmm.getNodeDataPart(node_index, sizeof (ObjectDescriptor), sizeof (ObjectNameLength) * pos.descriptor.length_element_count);
  ui64 name_index = 0;
  ui64 index_pos = 0;
  for(auto it = name_blocks.begin<ObjectNameLength>(), end = name_blocks.end<ObjectNameLength>(); it != end; ++it) {
    ui64 name_count = it->name_count;
    while (name_count) {
      ByteArray name = fmm.getNodeDataPart(node_index,
                                           sizeof (ObjectDescriptor) +
                                           sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                                           name_index, it->name_length*toSize(it->char_type));
      virtual_index index = fmm.getNodeDataPart(node_index,
                                                sizeof (ObjectDescriptor) +
                                                sizeof (ObjectNameLength)*pos.descriptor.length_element_count +
                                                pos.descriptor.name_block_size +
                                                sizeof (virtual_index)*index_pos,
                                                sizeof (virtual_index)).get<virtual_index>(0);
      handler({it->char_type, it->name_length, name.begin(), index});
      name_index += it->name_length*toSize(it->char_type);
      ++index_pos;
      --name_count;
    }
  }
}
