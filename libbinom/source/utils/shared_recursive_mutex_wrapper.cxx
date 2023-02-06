#include "libbinom/include/utils/shared_recursive_mutex_wrapper.hxx"

using namespace shared_recursive_mtx;

thread_local std::map<std::shared_mutex*, Counters> SharedRecursiveMutexWrapper::counter_storage;