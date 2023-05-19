#ifdef ENABLE_DEBUG
#ifndef DEBUG_HXX
#define DEBUG_HXX

extern thread_local size_t dbg_log_depth = 0;

#define DBG_SEPARATOR std::cout << "\x1B[97m=============================================================================\033[0m\n\r";  std::cout.flush();

#define DBG_ANNOUNCE(MSG) \
  std::cout << std::string(dbg_log_depth, '|') << "[DA]: " #MSG << "\033[0m\n\r"; std::cout.flush();

#define DBG_LOG(INF) \
  std::cout << std::string(dbg_log_depth, '|') << "\x1B[94m[Di]\x1B[34m: " << INF << "\033[0m\n\r"; std::cout.flush();

#undef ENABLE_DEBUG
#endif
#else
#ifndef NO_DEBUG_HXX
#define NO_DEBUG_HXX

#define DBG_SEPARATOR
#define DBG_ANNOUNCE(MSG)
#define DBG_LOG(INF)

#endif
#endif