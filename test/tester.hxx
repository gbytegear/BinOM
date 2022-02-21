#ifndef TESTER_HPP
#define TESTER_HPP

#include <iostream>

size_t log_depth = 0;

#ifndef __FUNCTION_NAME__
    #ifdef WIN32   //WINDOWS
        #define __FUNCTION_NAME__   __FUNCTION__
    #else          //*NIX
        #define __FUNCTION_NAME__   __func__
    #endif
#endif

#if defined (_WIN32) || defined (_WIN64) || defined (__CYGWIN__)

#define IF_WINDWOS(expression) expression
#define IF_LINUX(expression)
#define IF_UNIX(expression)
#define IF_MAC_OS_X(expression)
#define IF_FREEBSD(expression)
#define IF_ANDROID(expression)
#define OS_TYPE "Windows"

#elif defined (__linux__) || defined (linux) || defined (__linux)

#define IF_WINDWOS(expression)
#define IF_LINUX(expression) expression
#define IF_UNIX(expression) expression
#define IF_MAC_OS_X(expression)
#define IF_FREEBSD(expression)
#define IF_ANDROID(expression)
#define OS_TYPE "Linux"

#elif defined (unix) || defined (__unix) || defined (__unix__)

#define IF_WINDWOS(expression)
#define IF_LINUX(expression)
#define IF_UNIX(expression) expression
#define IF_MAC_OS_X(expression)
#define IF_FREEBSD(expression)
#define IF_ANDROID(expression)
#define OS_TYPE "Unix"

#elif defined(__APPLE__) || defined(__MACH__)

#define IF_WINDWOS(expression)
#define IF_LINUX(expression)
#define IF_UNIX(expression) expression
#define IF_MAC_OS_X(expression) expression
#define IF_FREEBSD(expression)
#define IF_ANDROID(expression)
#define OS_TYPE "Max OS X"

#elif defined(__FreeBSD__)

#define IF_WINDWOS(expression)
#define IF_LINUX(expression)
#define IF_UNIX(expression) expression
#define IF_MAC_OS_X(expression)
#define IF_FREEBSD(expression) expression
#define IF_ANDROID(expression)
#define OS_TYPE "FreeBSD"

#elif defined(__ANDROID__)

#define IF_WINDWOS(expression)
#define IF_LINUX(expression)
#define IF_UNIX(expression) expression
#define IF_MAC_OS_X(expression)
#define IF_FREEBSD(expression)
#define IF_ANDROID(expression) expression
#define OS_TYPE "Android"

#else

#define IF_WINDWOS(expression)
#define IF_LINUX(expression)
#define IF_UNIX(expression)
#define IF_MAC_OS_X(expression)
#define IF_FREEBSD(expression)
#define IF_ANDROID(expression)
#define OS_TYPE "Unknown"

#endif


#ifdef TEST_FULL_INFO
#define INFO << "(@path: " __FILE__ ":" << __LINE__ << " @func: " << __FUNCTION_NAME__ << ") "
#else
#define INFO
#endif

#define GRP_PUSH ++log_depth;
#define GRP_POP std::cout << std::string(log_depth - 1, '|') << "+---\n"; --log_depth;
#define GRP(expression) GRP_PUSH expression GRP_POP

#define SEPARATOR std::cout << "-----------------------------------------------------------------------------\n";

#define PRINT_RUN(expression) \
  std::cout << std::string(log_depth, '|') INFO << "[r]: " << #expression << '\n'; \
  expression

#define TEST_ANOUNCE(MSG) \
  std::cout << std::string(log_depth, '|') INFO << "[T]: " #MSG << '\n';

#define TEST(expression) \
  if(static_cast <bool> (expression)) \
    std::cout << std::string(log_depth, '|') INFO << "[✓]: " << #expression << '\n'; \
  else { \
    std::cerr << std::string(log_depth, '|') INFO << "[✗]: " << #expression << '\n'; \
    std::exit(-1); \
  }

#define LOG(INF) \
  std::cout << std::string(log_depth, '|') INFO << "[i]: " << INF << '\n';

#endif // TESTER_HPP
