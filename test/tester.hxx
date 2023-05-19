#ifndef TESTER_HPP
#define TESTER_HPP

#include <iostream>
#include <iomanip>
#include <thread>
#include <csignal>

#ifndef __FUNCTION_NAME__
    #if defined (WIN32) || defined (_WIN32) || defined (_WIN64) || defined (__CYGWIN__) //WINDOWS
        #define __FUNCTION_NAME__   __FUNCTION__
    #else //*NIX
        #define __FUNCTION_NAME__   __func__
    #endif
#endif

#if defined (_WIN32) || defined (_WIN64) || defined (__CYGWIN__)

#define IF_WINDOWS(expression) expression
#define IF_LINUX(expression)
#define IF_UNIX(expression)
#define IF_ONLY_UNIX(expression)
#define IF_MAC_OS_X(expression)
#define IF_FREEBSD(expression)
#define IF_ANDROID(expression)
#define OS_TYPE "Windows"

#elif defined (__linux__) || defined (linux) || defined (__linux)

#define IF_WINDOWS(expression)
#define IF_LINUX(expression) expression
#define IF_UNIX(expression) expression
#define IF_ONLY_UNIX(expression)
#define IF_MAC_OS_X(expression)
#define IF_FREEBSD(expression)
#define IF_ANDROID(expression)
#define OS_TYPE "Linux"

#elif defined (unix) || defined (__unix) || defined (__unix__)

#define IF_WINDOWS(expression)
#define IF_LINUX(expression)
#define IF_UNIX(expression) expression
#define IF_ONLY_UNIX(expression) expression
#define IF_MAC_OS_X(expression)
#define IF_FREEBSD(expression)
#define IF_ANDROID(expression)
#define OS_TYPE "Unix"

#elif defined(__APPLE__) || defined(__MACH__)

#define IF_WINDOWS(expression)
#define IF_LINUX(expression)
#define IF_UNIX(expression) expression
#define IF_ONLY_UNIX(expression)
#define IF_MAC_OS_X(expression) expression
#define IF_FREEBSD(expression)
#define IF_ANDROID(expression)
#define OS_TYPE "Max OS X"

#elif defined(__FreeBSD__)

#define IF_WINDOWS(expression)
#define IF_LINUX(expression)
#define IF_UNIX(expression) expression
#define IF_ONLY_UNIX(expression)
#define IF_MAC_OS_X(expression)
#define IF_FREEBSD(expression) expression
#define IF_ANDROID(expression)
#define OS_TYPE "FreeBSD"

#elif defined(__ANDROID__)

#define IF_WINDOWS(expression)
#define IF_LINUX(expression)
#define IF_UNIX(expression) expression
#define IF_ONLY_UNIX(expression)
#define IF_MAC_OS_X(expression)
#define IF_FREEBSD(expression)
#define IF_ANDROID(expression) expression
#define OS_TYPE "Android"

#else

#define IF_WINDOWS(expression)
#define IF_LINUX(expression)
#define IF_UNIX(expression)
#define IF_ONLY_UNIX(expression)
#define IF_MAC_OS_X(expression)
#define IF_FREEBSD(expression)
#define IF_ANDROID(expression)
#define OS_TYPE "Unknown"

#endif

#ifdef TEST_FULL_INFO
#define INFO << "(@path: " __FILE__ ":" << __LINE__ << " @func: " << __FUNCTION_NAME__ << " @thread: #" << std::hex << std::this_thread::get_id() << std::dec << ") "
#else
#define INFO
#endif

extern thread_local size_t log_depth;

#define GRP_PUSH ++log_depth;
#define GRP_POP if(log_depth) std::cout << std::string(log_depth - 1, '|') << "+---\n\r"; std::cout.flush(); --log_depth;
#define GRP(expression) GRP_PUSH expression GRP_POP


// Test utilits

#define SEPARATOR std::cout << "\x1B[97m=============================================================================\033[0m\n\r";  std::cout.flush();


#define PRINT_RUN(expression) \
  std::cout << std::string(log_depth, '|') INFO << "\x1B[93m[r]\x1B[33m: " << #expression << "\033[0m\n\r"; std::cout.flush(); \
  expression


#define TEST_ANNOUNCE(MSG) \
  std::cout << std::string(log_depth, '|') INFO << "[T]: " #MSG << "\033[0m\n\r"; std::cout.flush();


#define NORMAL_TXT "\033[0m"
#define WHITE_TXT "\x1B[97m"
#define RED_TXT "\x1B[97;101m"
#define GREEN_TXT "\x1B[32m"

#define LOG(INF) \
  std::cout << std::string(log_depth, '|') INFO << "\x1B[94m[i]\x1B[34m: " << INF << "\033[0m\n\r"; std::cout.flush();


#ifdef EXIT_ON_FAILURE
#define FAILURE_TEST_EXIT std::exit(-1);
#else
#define FAILURE_TEST_EXIT
#endif

#define TEST(expression) \
  if(static_cast<bool>(expression)) { \
    std::cout << std::string(log_depth, '|') INFO << "\x1B[92m[✓]\x1B[32m: " << #expression << "\033[0m\n\r"; std::cout.flush(); \
  } else { \
    std::cout << std::string(log_depth, '|') INFO << "\x1B[97;41m[✗]\x1B[97;101m: " << #expression << "\033[0m\n\r"; std::cout.flush(); \
    __test_init.is_success = false; \
    std::cout.flush(); \
    FAILURE_TEST_EXIT \
  }


#define TEST_LEGEND \
  std::cout << "Test legend:\n\r" \
               "\x1B[93m[r]\033[0m - running line of code\n\r" \
               "\033[107;30m[T]\033[0m - test announce\n\r" \
               "\x1B[94m[i]\033[0m - log\n\r" \
               "\x1B[92m[✓]\033[0m - passed test\n\r" \
               "\x1B[97;41m[✗]\033[0m - failed test\n\r"; \
  std::cout.flush();


[[noreturn]] void __signal_handler(int signum);

inline struct __TestInit {
  bool is_success = true;

  __TestInit() {
    signal(SIGILL, __signal_handler);
    signal(SIGFPE, __signal_handler);
    signal(SIGSEGV, __signal_handler);
    signal(SIGTERM, __signal_handler);
    signal(SIGABRT, __signal_handler);

    std::cout << "\033[102;30m+---------------------------------------------------------------------------+\033[0m\n\r"
                 "\033[102;30m|                                Test started                               |\033[0m\n\r"
                 "\033[102;30m+---------------------------------------------------------------------------+\033[0m\n\r";
    SEPARATOR
    std::cout << "OS: " << OS_TYPE << "\n\r"; std::cout.flush();
    SEPARATOR
    TEST_LEGEND
  }
  ~__TestInit() {
    SEPARATOR
    std::cout << "OS: " << OS_TYPE << "\n\r"; std::cout.flush();
    SEPARATOR
    TEST_LEGEND
    SEPARATOR
    if(is_success)
      std::cout << "\033[102;30m+---------------------------------------------------------------------------+\033[0m\n\r"
                   "\033[102;30m|                            Test ended success!                            |\033[0m\n\r"
                   "\033[102;30m+---------------------------------------------------------------------------+\033[0m\n\r";
    else
      std::cout << "\033[101;31m+---------------------------------------------------------------------------+\033[0m\n\r"
                   "\033[101;31m|                           Test ended with error!                          |\033[0m\n\r"
                   "\033[101;31m+---------------------------------------------------------------------------+\033[0m\n\r";
       std::cout.flush();
  }
} __test_init;



extern class RAIIPerfomanceTest {
  clock_t start_time;
  const char* msg;
public:
  RAIIPerfomanceTest(const char* msg = "") : start_time(clock()), msg(msg) {}
  RAIIPerfomanceTest(const RAIIPerfomanceTest&) = delete;
  RAIIPerfomanceTest(RAIIPerfomanceTest&& other) : start_time(other.start_time), msg(other.msg) {}
  ~RAIIPerfomanceTest() {
    double time = double( clock() - start_time ) / (double)CLOCKS_PER_SEC;
    if(!start_time) return;
    std::cout << std::string(log_depth, '|') << msg << time << " second(s).\033[0m" << std::endl;
    std::cout.flush();
  }
} __test_perf;

#endif // TESTER_HPP
