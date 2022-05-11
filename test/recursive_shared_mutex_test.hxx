#ifndef RECURSIVE_SHARED_MUTEX_TEST_HXX
#define RECURSIVE_SHARED_MUTEX_TEST_HXX

#include "libbinom/include/utils/shared_recursive_mutex_wrapper.hxx"
#include "test/tester.hxx"
#include <list>
#include <thread>

std::shared_mutex shared_mtx;

void subwriter();

void subreader() {
  binom::priv::SharedRecursiveLock lock(&shared_mtx, binom::priv::MtxLockType::shared_locked);
  static thread_local int recursion_depth = 0;

  std::cout << "0+===RRRRRRRRRRRRRRR===+0\n"
            << "1|rrrrrrrrrrrrrrrrrrrrr|1\n"
            << "2|rrrrrrrrrrrrrrrrrrrrr|2\n"
            << "3|rrrrrrrrrrrrrrrrrrrrr|3\n"
            << "4|rrrrrrrrrrrrrrrrrrrrr|4\n"
            << "5|rrrrrrrrrrrrrrrrrrrrr|5\n"
            << "6|rrrrrrrrrrrrrrrrrrrrr|6\n"
            << "7|rrrrrrrrrrrrrrrrrrrrr|7\n"
            << "8|rrrrrrrrrrrrrrrrrrrrr|8\n"
            << "9+===RRRRRRRRRRRRRRR===+9\n";

  if (++recursion_depth <= 5) subreader();
  else subwriter();
}

void subwriter() {
  binom::priv::SharedRecursiveLock lock(&shared_mtx, binom::priv::MtxLockType::unique_locked);
  static thread_local int recursion_depth = 0;

  std::cout << "0+===WWWWWWWWWWWWWWW===+0\n"
            << "1|wwwwwwwwwwwwwwwwwwwww|1\n"
            << "2|wwwwwwwwwwwwwwwwwwwww|2\n"
            << "3|wwwwwwwwwwwwwwwwwwwww|3\n"
            << "4|wwwwwwwwwwwwwwwwwwwww|4\n"
            << "5|wwwwwwwwwwwwwwwwwwwww|5\n"
            << "6|wwwwwwwwwwwwwwwwwwwww|6\n"
            << "7|wwwwwwwwwwwwwwwwwwwww|7\n"
            << "8|wwwwwwwwwwwwwwwwwwwww|8\n"
            << "9+===WWWWWWWWWWWWWWW===+9\n";

  if (++recursion_depth <= 5) subwriter();
}

void reader() {
  subreader();
}


void testRecursiveSharedMutex() {
  RAIIPerfomanceTest test_perf("Recursive shared mutex test: ");
  shared_mtx.lock();
  TEST_ANNOUNCE(RecursiveSharedMutex test);
  std::list<std::thread> threads;
  for(unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
    threads.emplace_back(reader);
  shared_mtx.unlock();
  for(auto& thread : threads) thread.join();
}

#endif // RECURSIVE_SHARED_MUTEX_TEST_HXX
