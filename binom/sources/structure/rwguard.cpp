#include "binom/includes/structure/rwguard.h"

using namespace binom;


RWGuard::RWGuard()
  : reader_count(0),
    writer(false),
    reader_wait_count(0),
    writer_wait_count(0),
    mtx(),
    read_condition(),
    write_condition() {}

void RWGuard::readLock() {
  std::unique_lock lk(mtx);
  ++reader_wait_count;
  read_condition.wait(lk, [&](){return !writer_wait_count;});
  --reader_wait_count;
  ++reader_count;
  lk.unlock();
}

void RWGuard::readUnlock() {
  std::unique_lock lk(mtx);
  --reader_count;
  lk.unlock();
  write_condition.notify_one();
}

void RWGuard::writeLock() {
  std::unique_lock lk(mtx);
  ++writer_wait_count;
  write_condition.wait(lk, [&](){return (!reader_count && !writer);});
  --writer_wait_count;
  writer = true;
  lk.unlock();
}

void RWGuard::writeUnlock() {
  std::unique_lock lk(mtx);
  writer = false;
  if(writer_wait_count)
    write_condition.notify_one();
  else
    read_condition.notify_all();
  lk.unlock();
}

void RWGuard::readToWrite() {
  std::unique_lock lk(mtx);
  --reader_count;
  ++writer_wait_count;
  write_condition.wait(lk, [&](){return (!reader_count && !writer);});
  --writer_wait_count;
  writer = true;
  lk.unlock();
}

void RWGuard::writeToRead() {
  std::unique_lock lk(mtx);
  writer = false;
  if(writer_wait_count)
    write_condition.notify_one();
  else
    read_condition.notify_all();

  ++reader_wait_count;
  read_condition.wait(lk, [&](){return !writer_wait_count;});
  --reader_wait_count;
  ++reader_count;
  lk.unlock();
}






RWSyncMap::RWSyncMapNode* RWSyncMap::getIfExist(f_virtual_index node_index) {
  map_mtx.lock();
  i64 left = 0;
  i64 right = length;
  i64 middle;

  // Find node
  do {
    middle = (left + right) / 2;

    if(middle >= static_cast<i64>(length) ||
       left > right ||
       (middle == 0 && map[middle].node_index > node_index))
      break;

    if(map[middle].node_index > node_index) right = middle - 1;
    elif(map[middle].node_index < node_index) left = middle + 1;
    elif(map[middle].node_index == node_index) {
      map_mtx.unlock();
      return map + middle;
    }

  } while(true);
  return nullptr;
}

RWGuard* RWSyncMap::get(f_virtual_index node_index) {
  map_mtx.lock();

  RWGuard* result;

  // If map is empty
  if(!map) {
    length = 1;
    map_mtx.unlock();
    return (map = new RWSyncMapNode(node_index))->guard;
  }

  i64 left = 0;
  i64 right = length;
  i64 middle;

  // Find node
  do {
    middle = (left + right) / 2;

    if(middle >= static_cast<i64>(length) ||
       left > right ||
       (middle == 0 && map[middle].node_index > node_index))
      break;

    if(map[middle].node_index > node_index) right = middle - 1;
    elif(map[middle].node_index < node_index) left = middle + 1;
    elif(map[middle].node_index == node_index) {
      result = map[middle].guard;
      map_mtx.unlock();
      return result;
    }

  } while(true);


  // Insert new node
  ++length;
  map = tryRealloc<RWSyncMapNode>(map, length);

  if(middle == 0) {
    memmove(map + 1, map, (length - 1) * sizeof(RWSyncMapNode));
    RWGuard* result = (new(map) RWSyncMapNode(node_index))->guard;
    map_mtx.unlock();
    return result;
  } elif(middle == static_cast<i64>(length)) {
    result = (new(map + length - 1) RWSyncMapNode(node_index))->guard;
    map_mtx.unlock();
    return result;
  }

  if(map[middle].node_index < node_index) ++middle;
  memmove(map + middle, map + middle + 1, (length - middle - 1) * sizeof(RWSyncMapNode));
  result = (new(map + middle) RWSyncMapNode(node_index))->guard;
  map_mtx.unlock();
  return result;

}

void RWSyncMap::tryRemove(f_virtual_index node_index) {
  RWSyncMapNode* sync_node_ptr;
  if(!(sync_node_ptr = getIfExist(node_index))) return;
  map_mtx.lock();
  if(!sync_node_ptr->guard->isFree()) {
    map_mtx.unlock();
    return;
  }

  if(map == sync_node_ptr) {
    delete map;
    map = nullptr;
    length = 0;
    return;
  }

  memmove(sync_node_ptr, sync_node_ptr + 1, (length - (sync_node_ptr - map)) + 1);
  map = tryRealloc<RWSyncMapNode>(map, --length);

  map_mtx.unlock();
}

RWSyncMap::RWSyncMapNode::RWSyncMapNode(f_virtual_index node_index)
  : node_index(node_index), guard(new RWGuard()) {}

RWSyncMap::RWSyncMapNode::~RWSyncMapNode() {if(guard) delete guard;}
