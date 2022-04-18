#ifndef RECURSIVE_SHARED_MUTEX_TEST_HXX
#define RECURSIVE_SHARED_MUTEX_TEST_HXX

#include "libbinom/include/utils/shared_recursive_mutex_wrapper.hxx"
#include "test/tester.hxx"
#include <thread>

std::shared_mutex shared_mtx;

binom::priv::SharedRecursiveLock reader(binom::priv::SharedRecursiveLock* outer_lock = nullptr) {
  binom::priv::SharedRecursiveLock lock = outer_lock
      ? std::move(*outer_lock)
      : binom::priv::SharedRecursiveLock(&shared_mtx, binom::priv::MtxLockType::shared_locked);
  static thread_local int recursion_depth = 0;

}

void testRecursiveSharedMutex() {
  std::thread thr_1(reader);
  std::thread thr_2(reader);
  thr_1.join();
  thr_2.join();
}

#endif // RECURSIVE_SHARED_MUTEX_TEST_HXX
