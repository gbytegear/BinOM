#ifndef TESTER_HPP
#define TESTER_HPP

#include <iostream>
#include <iomanip>
#include <thread>

#ifndef __FUNCTION_NAME__
    #ifdef WIN32   //WINDOWS
        #define __FUNCTION_NAME__   __FUNCTION__
    #else          //*NIX
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

thread_local size_t log_depth = 0;

#define GRP_PUSH ++log_depth;
#define GRP_POP std::cout << std::string(log_depth - 1, '|') << "+---\n"; --log_depth;
#define GRP(expression) GRP_PUSH expression GRP_POP

#define SEPARATOR std::cout << "\x1B[97m=============================================================================\033[0m\n";

#define PRINT_RUN(expression) \
  std::cout << "\x1B[33m" << std::string(log_depth, '|') INFO << "\x1B[93m[r]\x1B[33m: " << #expression << "\033[0m\n"; \
  expression

#define TEST_ANNOUNCE(MSG) \
  std::cout << "\033[107;30m" << std::string(log_depth, '|') INFO << "[T]: " #MSG << "\033[0m\n";

#define LOG(INF) \
  std::cout << "\x1B[34m" << std::string(log_depth, '|') INFO << "\x1B[94m[i]\x1B[34m: " << INF << "\033[0m\n";

#define TEST_LEGEND \
  std::cout << "Test legend:\n" \
               "[r] - running line of code\n" \
               "[T] - test announce\n" \
               "[i] - log\n" \
               "[✓] - passed test\n" \
               "[✗] - failed test\n";

inline struct __TestInit {
  bool is_success = true;
  __TestInit() {
    SEPARATOR
    std::cout << "OS: " << OS_TYPE << '\n';
    SEPARATOR
    TEST_LEGEND
  }
  ~__TestInit() {
    SEPARATOR
    if(is_success)
      std::cout << "\033[102;30m+---------------------------------------------------------------------------+\033[0m\n"
                   "\033[102;30m|                            Test ended success!                            |\033[0m\n"
                   "\033[102;30m+---------------------------------------------------------------------------+\033[0m\n";
    else
      std::cout << "\033[101;31m+---------------------------------------------------------------------------+\033[0m\n"
                   "\033[101;31m|                           Test ended with error!                          |\033[0m\n"
                   "\033[101;31m+---------------------------------------------------------------------------+\033[0m\n";
  }
} __test_init;

#define TEST(expression) \
  if(static_cast <bool> (expression)) \
    std::cout << "\x1B[32m" << std::string(log_depth, '|') INFO << "\x1B[92m[✓]\x1B[32m: " << #expression << "\033[0m\n"; \
  else { \
    std::cout << "\x1B[97;101m" << std::string(log_depth, '|') INFO << "\x1B[97;41m[✗]\x1B[97;101m: " << #expression << "\033[0m\n"; \
    __test_init.is_success = false; \
    std::cout.flush(); \
    std::exit(-1); \
  }

class RAIIPerfomanceTest {
  clock_t start_time;
  const char* msg;
public:
  RAIIPerfomanceTest(const char* msg = "") : start_time(clock()), msg(msg) {}
  RAIIPerfomanceTest(const RAIIPerfomanceTest&) = delete;
  RAIIPerfomanceTest(RAIIPerfomanceTest&& other) : start_time(other.start_time), msg(other.msg) {}
  ~RAIIPerfomanceTest() {std::cout << msg << double( clock() - start_time ) / (double)CLOCKS_PER_SEC << " second(s)." << std::endl;}
};

#endif // TESTER_HPP
