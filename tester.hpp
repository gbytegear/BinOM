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

#ifdef TEST_FULL_INFO
#define INFO << " (@path: " << __FILE__ << ':' << __LINE__ << " @func: " << __FUNCTION_NAME__ << ")"
#else
#define INFO
#endif

#define GRP_PUSH ++log_depth;
#define GRP_POP std::clog << std::string(log_depth - 1, '|') << "+---\n"; --log_depth;

#define SEPARATOR std::clog << "-----------------------------------------------------------------------------\n";

#define PRINT_RUN(expression) \
  std::clog << std::string(log_depth, '|') << "[r] - " << #expression INFO << '\n'; \
  expression

#define TEST_ANOUNCE(MSG) \
  std::clog << std::string(log_depth, '|') << "[TEST STARTED] - " #MSG INFO << '\n';

#define TEST(expression) \
  if(static_cast <bool> (expression)) \
    std::clog << std::string(log_depth, '|') << "[✓] - " << #expression INFO << '\n'; \
  else { \
    std::cerr << std::string(log_depth, '|') << "[✗] - " << #expression INFO << '\n'; \
    std::terminate(); \
  }

#define LOG(INF) \
  std::clog << std::string(log_depth, '|') << "[i] - " << INF INFO << '\n';

#endif // TESTER_HPP
