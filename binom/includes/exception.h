#include "ctypes.h"
#include <string>
#include <fstream>
#include <iostream>

namespace binom {

enum ErrCode : ui8 {
  memory_allocation_error,
  memory_free_error,
  binom_invalid_type,
  binom_invalid_initer,
};

class SException {
  ErrCode _code;
  const char* error_string;
  friend class Exception;
public:
  constexpr SException(const ErrCode code, const char* error_string) : _code(code), error_string(error_string) {}
  constexpr SException(const ErrCode code) : _code(code), error_string(nullptr) {}
  ErrCode code() {return _code;}
  const char* what() {return error_string;}
  [[noreturn]] inline void throwUp() {throw *this;}
};

class Exception {
  ErrCode _code;
  std::string error_string;
public:
  Exception(ErrCode code, std::string error_string) : _code(code), error_string(error_string) {}
  Exception(SException except) : _code(except._code), error_string(except.error_string) {}
  Exception(SException except, std::string add_error_string) : _code(except._code), error_string(std::string(except.error_string) + add_error_string) {}
  ErrCode code() {return _code;}
  std::string what() {return error_string;}
  [[noreturn]] inline void throwUp() {throw *this;}
};

class ErrLogger {
  std::ofstream logfile;
  std::ostream& console = std::cerr;

  ErrLogger& out(std::string str) {
    logfile << str;
    console << str;
    return *this;
  }

  ErrLogger& out(char* str) {
    logfile << str;
    console << str;
    return *this;
  }

  const char* ectos(ErrCode code) {
    switch (code) {
      case ErrCode::memory_allocation_error: return   "Memory allocation error";
      case ErrCode::memory_free_error: return         "Memory free error";
      case ErrCode::binom_invalid_type: return        "Invalid BinOM type";
    }
  }

public:

  ErrLogger& operator<<(std::string str) {return out(str);}
  ErrLogger& operator<<(const char* str) {return out(str);}

  ErrLogger& operator<<(SException except) {
    return
         out(ectos(except.code()))
        .out(except.what()?": ":" ")
        .out(except.what()?except.what():"");
  }

  ErrLogger& operator<<(Exception except) {
    out(ectos(except.code()));
    std::string msg = except.what();
    if(msg.empty()) return *this;
    else return out(":").out(msg);
  }
};

extern ErrLogger erlog;

}
