#ifndef RECURSIVE_SHARED_MUTEX_TEST_HXX
#define RECURSIVE_SHARED_MUTEX_TEST_HXX

#include "libbinom/include/utils/resource_control.hxx"
#include "test/tester.hxx"
#include <thread>

std::shared_mutex shared_mtx;

void testRecursiveSharedMutexJob() {
  using namespace binom::priv;

  shared_mtx.lock();
  TEST_ANNOUNCE(Thread started)
  shared_mtx.unlock();

  PRINT_RUN(SharedRecursiveMutexWrapper wrapper_1(&shared_mtx, MtxLockType::shared_locked);)
  LOG("Thread #" << std::this_thread::get_id() << " owns first shared lock")
  LOG("Shared locks count: " << wrapper_1.getSheredLockCount())
  LOG("Unique locks count: " << wrapper_1.getUniqueLockCount())
  PRINT_RUN(wrapper_1.lockShared();)
  LOG("Thread #" << std::this_thread::get_id() << " owns second shared lock")
  LOG("Shared locks count: " << wrapper_1.getSheredLockCount())
  LOG("Unique locks count: " << wrapper_1.getUniqueLockCount())
  PRINT_RUN(wrapper_1.lock();)
  LOG("Thread #" << std::this_thread::get_id() << " owns first unique lock")
  LOG("Shared locks count: " << wrapper_1.getSheredLockCount())
  LOG("Unique locks count: " << wrapper_1.getUniqueLockCount())
  PRINT_RUN(wrapper_1.unlock();)
  LOG("Thread #" << std::this_thread::get_id() << " release first unique lock")
  LOG("Shared locks count: " << wrapper_1.getSheredLockCount())
  LOG("Unique locks count: " << wrapper_1.getUniqueLockCount())
  PRINT_RUN(wrapper_1.unlockShared();)
  LOG("Thread #" << std::this_thread::get_id() << " release second shared lock")
  LOG("Shared locks count: " << wrapper_1.getSheredLockCount())
  LOG("Unique locks count: " << wrapper_1.getUniqueLockCount())
  PRINT_RUN(wrapper_1.unlockShared();)
  LOG("Thread #" << std::this_thread::get_id() << " release first shared lock")
  LOG("Shared locks count: " << wrapper_1.getSheredLockCount())
  LOG("Unique locks count: " << wrapper_1.getUniqueLockCount())
}

void testRecursiveSharedMutex() {
  std::thread thr_1(testRecursiveSharedMutexJob);
  std::thread thr_2(testRecursiveSharedMutexJob);
  thr_1.join();
  thr_2.join();
}

#endif // RECURSIVE_SHARED_MUTEX_TEST_HXX
