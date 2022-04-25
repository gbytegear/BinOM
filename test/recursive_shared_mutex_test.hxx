#ifndef RECURSIVE_SHARED_MUTEX_TEST_HXX
#define RECURSIVE_SHARED_MUTEX_TEST_HXX

#include "libbinom/include/utils/shared_recursive_mutex_wrapper.hxx"
#include "test/tester.hxx"
#include <thread>

std::shared_mutex shared_mtx;

void subwriter();

void subreader() {
  binom::priv::SharedRecursiveLock lock(&shared_mtx, binom::priv::MtxLockType::shared_locked);
  static thread_local int recursion_depth = 0;
  if (++recursion_depth <= 20) subreader();
  else subwriter();
}

void subwriter() {
  binom::priv::SharedRecursiveLock lock(&shared_mtx, binom::priv::MtxLockType::unique_locked);
  static thread_local int recursion_depth = 0;
  if (++recursion_depth <= 20) subwriter();
}

void reader() {
  subreader();
}


void testRecursiveSharedMutex() {
  RAIIPerfomanceTest test_perf("Recursive shared mutex test: ");
  shared_mtx.lock();
  std::thread thr_1(reader);
  std::thread thr_2(reader);
  shared_mtx.unlock();
  thr_1.join();
  thr_2.join();
}

#endif // RECURSIVE_SHARED_MUTEX_TEST_HXX
