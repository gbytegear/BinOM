#include "tester.hxx"

thread_local size_t log_depth = 0;

RAIIPerfomanceTest __test_perf("Test perfomance: ");

void __signal_handler(int signum) {
  std::cout << std::string(log_depth, '|') INFO << "\x1B[97;41m[✗]\x1B[97;101m: ";
  std::cout.flush();
  switch (signum) {
  case SIGILL:  std::cout << "Recived signal SIGILL - Illegal instruction"; std::cout.flush(); break;
  case SIGABRT: std::cout << "Recived signal SIGABRT - Abnormal termination"; std::cout.flush(); break;
  case SIGFPE:  std::cout << "Recived signal SIGFPE - Erroneous arithmetic operation"; std::cout.flush(); break;
  case SIGSEGV: std::cout << "Recived signal SIGSEGV - Invalid access to storage"; std::cout.flush(); break;
  case SIGTERM: std::cout << "Recived signal SIGTERM - Termination request"; std::cout.flush(); break;
  }
  std::cout << "\033[0m\n\r";
  std::cout.flush();
  __test_init.is_success = false;
  std::exit(signum);
}
