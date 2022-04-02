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

#define SEPARATOR std::cout << "=============================================================================\n";

#define PRINT_RUN(expression) \
  std::cout << std::string(log_depth, '|') INFO << "[r]: " << #expression << '\n'; \
  expression

#define TEST_ANNOUNCE(MSG) \
  std::cout << std::string(log_depth, '|') INFO << "[T]: " #MSG << '\n';

#define LOG(INF) \
  std::cout << std::string(log_depth, '|') INFO << "[i]: " << INF << '\n';

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
      std::cout << "+---------------------------------------------------------------------------+\n"
                   "|                            Test ended success!                            |\n"
                   "+---------------------------------------------------------------------------+\n";
    else
      std::cout << "+---------------------------------------------------------------------------+\n"
                   "|                           Test ended with error!                          |\n"
                   "+---------------------------------------------------------------------------+\n";
  }
} __test_init;

#define TEST(expression) \
  if(static_cast <bool> (expression)) \
    std::cout << std::string(log_depth, '|') INFO << "[✓]: " << #expression << '\n'; \
  else { \
    std::cerr << std::string(log_depth, '|') INFO << "[✗]: " << #expression << '\n'; \
    __test_init.is_success = false; \
    std::exit(-1); \
  }

#endif // TESTER_HPP
