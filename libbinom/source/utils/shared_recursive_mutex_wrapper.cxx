#include "libbinom/include/utils/shared_recursive_mutex_wrapper.hxx"

using namespace shared_recursive_mtx;

thread_local std::map<std::shared_mutex*, Counters> SharedRecursiveMutexWrapper::counter_storage;

std::map<std::shared_mutex*, Counters>::iterator SharedRecursiveMutexWrapper::createCountersRef(const std::shared_mutex *mtx) noexcept {
  if(!mtx) return counter_storage.end();
  auto it = counter_storage.find(const_cast<std::shared_mutex*>(mtx));
  if(it == counter_storage.cend())
    return counter_storage.emplace(std::make_pair(const_cast<std::shared_mutex*>(mtx), Counters{})).first;
  ++it->second.wrapper_count;
  return it;
}

void SharedRecursiveMutexWrapper::deleteCounterRef(std::map<std::shared_mutex*, Counters>::iterator it) noexcept {
  if(it != counter_storage.cend()) {
    if(!--it->second.wrapper_count) {
      if(it->second.unique_lock_counter) it->first->unlock();
      if(it->second.shared_lock_counter) it->first->unlock_shared();
      counter_storage.erase(it);
    }
  }
  it = counter_storage.end();
}

SharedRecursiveMutexWrapper::SharedRecursiveMutexWrapper(const std::shared_mutex *mtx, MtxLockType lock_type) noexcept
  : mtx_data(createCountersRef(mtx)) {
  if(!mtx) std::terminate();
  switch (lock_type) {
  case MtxLockType::unlocked: return;
  case MtxLockType::shared_locked: lockShared(); return;
  case MtxLockType::unique_locked: lock(); return;
  }
}

SharedRecursiveMutexWrapper::SharedRecursiveMutexWrapper(const SharedRecursiveMutexWrapper &other, MtxLockType lock_type) noexcept
  : SharedRecursiveMutexWrapper(other.mtx_data->first) {
  switch (lock_type) {
  case MtxLockType::unlocked: return;
  case MtxLockType::shared_locked: lockShared(); return;
  case MtxLockType::unique_locked: lock(); return;
  }
}

SharedRecursiveMutexWrapper::SharedRecursiveMutexWrapper(SharedRecursiveMutexWrapper &&other, MtxLockType lock_type) noexcept
  : mtx_data(other.mtx_data) {
  other.mtx_data = counter_storage.end();
  switch (lock_type) {
  case MtxLockType::unlocked: return;
  case MtxLockType::shared_locked: lockShared(); return;
  case MtxLockType::unique_locked: lock(); return;
  }
}

SharedRecursiveMutexWrapper::~SharedRecursiveMutexWrapper() { deleteCounterRef(mtx_data); mtx_data = counter_storage.end(); }

ui64 SharedRecursiveMutexWrapper::getWrappedMutexCount() noexcept {return counter_storage.size();}

ui64 SharedRecursiveMutexWrapper::getUniqueLockCount() const noexcept {return mtx_data->second.unique_lock_counter;}

ui64 SharedRecursiveMutexWrapper::getSheredLockCount() const noexcept {return mtx_data->second.shared_lock_counter;}

ui64 SharedRecursiveMutexWrapper::getThisMutexWrapperCount() const noexcept {return mtx_data->second.wrapper_count;}

std::shared_mutex &SharedRecursiveMutexWrapper::getSharedMutex() const noexcept {return *mtx_data->first;}

MtxLockType SharedRecursiveMutexWrapper::getLockType() const noexcept {
  if(mtx_data->second.unique_lock_counter) return MtxLockType::unique_locked;
  if(mtx_data->second.shared_lock_counter) return MtxLockType::shared_locked;
  return MtxLockType::unlocked;
}

void SharedRecursiveMutexWrapper::lock() noexcept {
  switch (getLockType()) {
  case MtxLockType::unlocked:
    mtx_data->first->lock();
    ++mtx_data->second.unique_lock_counter;
    return;
  case MtxLockType::shared_locked:
    mtx_data->first->unlock_shared();
    mtx_data->first->lock();
    ++mtx_data->second.unique_lock_counter;
    return;
  case MtxLockType::unique_locked:
    ++mtx_data->second.unique_lock_counter;
    return;
  }
}

void SharedRecursiveMutexWrapper::lockShared() noexcept {
  switch (getLockType()) {
  case MtxLockType::unlocked:
    mtx_data->first->lock_shared();
    ++mtx_data->second.shared_lock_counter;
    return;
  case MtxLockType::shared_locked:
    ++mtx_data->second.shared_lock_counter;
    return;
  case MtxLockType::unique_locked:
    ++mtx_data->second.shared_lock_counter;
    return;
  }
}

void SharedRecursiveMutexWrapper::unlock() noexcept {
  switch (getLockType()) {
  case MtxLockType::unlocked:
  case MtxLockType::shared_locked: return;
  case MtxLockType::unique_locked:
    if(!--mtx_data->second.unique_lock_counter) {
      if(mtx_data->second.shared_lock_counter) {

        // "A prior unlock() operation on the same mutex synchronizes-with (as defined in std::memory_order) this operation."
        // - (C) [https://en.cppreference.com/w/cpp/thread/shared_mutex/lock_shared] 3.04.2022
        mtx_data->first->unlock(); mtx_data->first->lock_shared();

      } else mtx_data->first->unlock();
    }
    return;
  }
}

void SharedRecursiveMutexWrapper::unlockShared() noexcept {
  switch (getLockType()) {
  case MtxLockType::unlocked: return;
  case MtxLockType::shared_locked:
    if(!--mtx_data->second.shared_lock_counter) mtx_data->first->unlock_shared();
    return;
  case MtxLockType::unique_locked:
    if(mtx_data->second.shared_lock_counter) --mtx_data->second.shared_lock_counter;
    return;
  }
}

bool SharedRecursiveMutexWrapper::tryLock() noexcept {
  switch (getLockType()) {
  case MtxLockType::unlocked: {
    if(mtx_data->first->try_lock()) {
      ++mtx_data->second.unique_lock_counter;
      return true;
    } else return false;
  }
  case MtxLockType::shared_locked:
    mtx_data->first->unlock_shared();
    mtx_data->first->lock();
    ++mtx_data->second.unique_lock_counter;
    return true;
  case MtxLockType::unique_locked:
    ++mtx_data->second.unique_lock_counter;
    return true;
  default: return false;
  }
}

bool SharedRecursiveMutexWrapper::tryLockShared() noexcept {
  switch (getLockType()) {
  case MtxLockType::unlocked:
    if(mtx_data->first->try_lock_shared()) {
      ++mtx_data->second.shared_lock_counter;
      return true;
    } else return false;
  case MtxLockType::shared_locked:
    ++mtx_data->second.shared_lock_counter;
    return true;
  case MtxLockType::unique_locked:
    ++mtx_data->second.shared_lock_counter;
    return true;
  default: return false;
  }
}

SharedRecursiveMutexWrapper &SharedRecursiveMutexWrapper::operator =(const SharedRecursiveMutexWrapper &other) noexcept {
  this->~SharedRecursiveMutexWrapper();
  return *new(this) SharedRecursiveMutexWrapper(other);
}

SharedRecursiveMutexWrapper &SharedRecursiveMutexWrapper::operator =(SharedRecursiveMutexWrapper &&other) noexcept {
  this->~SharedRecursiveMutexWrapper();
  return *new(this) SharedRecursiveMutexWrapper(std::move(other));
}

SharedRecursiveMutexWrapper &SharedRecursiveMutexWrapper::operator =(const SharedRecursiveLock &other) noexcept {
  this->~SharedRecursiveMutexWrapper();
  return *new(this) SharedRecursiveMutexWrapper(other);
}

SharedRecursiveMutexWrapper &SharedRecursiveMutexWrapper::operator =(SharedRecursiveLock &&other) noexcept {
  this->~SharedRecursiveMutexWrapper();
  return *new(this) SharedRecursiveMutexWrapper(std::move(other));
}








SharedRecursiveLock::SharedRecursiveLock(const std::shared_mutex *mtx, MtxLockType lock_type)
  : SharedRecursiveMutexWrapper(const_cast<std::shared_mutex*>(mtx), lock_type), lock_type(lock_type) {}

SharedRecursiveLock::SharedRecursiveLock(const SharedRecursiveMutexWrapper &other, MtxLockType lock_type)
  : SharedRecursiveMutexWrapper(other, lock_type),
    lock_type(lock_type) {}

SharedRecursiveLock::SharedRecursiveLock(SharedRecursiveMutexWrapper &&other, MtxLockType lock_type)
  : SharedRecursiveMutexWrapper(std::move(other), lock_type),
    lock_type(lock_type) {}

SharedRecursiveLock::SharedRecursiveLock(const SharedRecursiveLock &other, MtxLockType lock_type)
  : SharedRecursiveMutexWrapper(other, lock_type),
    lock_type(lock_type) {}

SharedRecursiveLock::SharedRecursiveLock(SharedRecursiveLock &&other)
  : SharedRecursiveMutexWrapper(std::move(other)),
    lock_type(other.lock_type) {
  other.lock_type = MtxLockType::unlocked;
}

SharedRecursiveLock::~SharedRecursiveLock() {
  switch (lock_type) {
  case MtxLockType::unlocked: return;
  case MtxLockType::shared_locked: unlockShared(); return;
  case MtxLockType::unique_locked: unlock(); return;
  }
}







TransactionLock::TransactionLock(std::set<std::shared_mutex *> mtx_set, MtxLockType lock_type)
  : lock_type(lock_type) {

  if(mtx_set.empty()) {
    this->lock_type = MtxLockType::unlocked;
    return;
  }

  switch (lock_type) {
  case shared_recursive_mtx::MtxLockType::unlocked: return;
  case shared_recursive_mtx::MtxLockType::shared_locked:
    forever {
      bool is_own_lock = true;

      auto it = mtx_set.begin();
      mtx_stack.emplace(*it).lockShared();
      ++it;

      for(auto end = mtx_set.cend(); it != end; ++it) {
        if(mtx_stack.emplace(*it).tryLockShared()) continue;

        is_own_lock = false;
        mtx_stack.pop();
        while(!mtx_stack.empty()) {
          mtx_stack.top().unlockShared();
          mtx_stack.pop();
        }
        break;
      }

      if(is_own_lock) break;
    }
    return;

  case shared_recursive_mtx::MtxLockType::unique_locked:
    forever {
      bool is_own_lock = true;

      auto it = mtx_set.begin();
      mtx_stack.emplace(*it).lock();
      ++it;

      for(auto end = mtx_set.cend(); it != end; ++it) {
        if(mtx_stack.emplace(*it).tryLock()) continue;

        is_own_lock = false;
        mtx_stack.pop();
        while(!mtx_stack.empty()) {
          mtx_stack.top().unlock();
          mtx_stack.pop();
        }
        break;
      }

      if(is_own_lock) break;
    }
    return;
  }
}

TransactionLock::TransactionLock(TransactionLock &&other)
  : mtx_stack(std::move(other.mtx_stack)), lock_type(other.lock_type) {
  other.lock_type = MtxLockType::unlocked;
}

TransactionLock::~TransactionLock(){
  switch (lock_type) {
  case shared_recursive_mtx::MtxLockType::unlocked: return;
  case shared_recursive_mtx::MtxLockType::shared_locked:
    while (!mtx_stack.empty()) { mtx_stack.top().unlockShared(); mtx_stack.pop(); }
    return;
  case shared_recursive_mtx::MtxLockType::unique_locked:
    while (!mtx_stack.empty()) { mtx_stack.top().unlock(); mtx_stack.pop(); }
    return;
  }
}
