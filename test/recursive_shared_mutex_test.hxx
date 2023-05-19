#ifndef RECURSIVE_SHARED_MUTEX_TEST_HXX
#define RECURSIVE_SHARED_MUTEX_TEST_HXX

#include "libbinom/include/utils/shared_recursive_mutex_wrapper.hxx"
#include "test/tester.hxx"
#include <list>
#include <thread>

std::shared_mutex shared_mtx;

void subwriter();

bool is_valid_data = true;

void subreader() {
  GRP_PUSH
  shared_recursive_mtx::SharedRecursiveLock lock(&shared_mtx, shared_recursive_mtx::MtxLockType::shared_locked);
  static thread_local int recursion_depth = 0;

  TEST(is_valid_data);

  if (++recursion_depth <= 20) subreader();
  else subwriter();

  TEST(is_valid_data);
  GRP_POP
}

void subwriter() {
  GRP_PUSH
  shared_recursive_mtx::SharedRecursiveLock lock(&shared_mtx, shared_recursive_mtx::MtxLockType::unique_locked);
  static thread_local int recursion_depth = 0;

  PRINT_RUN(is_valid_data = false);

  if (++recursion_depth <= 10) subwriter();

  PRINT_RUN(is_valid_data = true);
  GRP_POP
}

void testRecursiveSharedMutex() {
  RAIIPerfomanceTest test_perf("Recursive shared mutex test: ");
  SEPARATOR
  TEST_ANNOUNCE(RecursiveSharedMutex test);
  shared_mtx.lock();
  std::list<std::thread> threads;
  for(unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
    threads.emplace_back(subreader);
  shared_mtx.unlock();
  for(auto& thread : threads) thread.join();
}

#endif // RECURSIVE_SHARED_MUTEX_TEST_HXX
