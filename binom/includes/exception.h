#include "ctypes.h"
#include <string>
#include <fstream>
#include <iostream>

namespace binom {

enum ErrCode : ui8 {
  any,
  memory_allocation_error,
  memory_free_error,
  out_of_range,
  file_open_error,

  binom_invalid_type,
  binom_out_of_range,
  binom_object_key_error,

  binomdb_invalid_file,
  binomdb_invalid_storage_version,

  binomdb_memory_management_error,
  binomdb_page_isnt_exist,
  binomdb_block_isnt_exist,
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

  static const char* ectos(ErrCode code) {
    switch (code) {
      case ErrCode::memory_allocation_error: return   "Memory allocation error: ";
      case ErrCode::memory_free_error: return         "Memory free error: ";
      case ErrCode::binom_invalid_type: return        "Invalid BinOM type: ";
      case ErrCode::binom_out_of_range: return        "Out of BinOM container range: ";
      case ErrCode::binom_object_key_error: return    "Invalide object key: ";
      case ErrCode::binomdb_invalid_file: return      "BinOM DB invalid file: ";
      case ErrCode::binomdb_invalid_storage_version:
      return                                          "BinOM DB invalid file storage version: ";
      case ErrCode::binomdb_memory_management_error:
      return                                          "BinOM DB memory management error: ";
      case ErrCode::binomdb_page_isnt_exist:
      return                                          "BinOM DB page isn't exist: ";
      case ErrCode::binomdb_block_isnt_exist:
      return                                          "BinOM DB block isn't exist: ";

      case ErrCode::any:
      default:
         return                                       "TODO: Write this message: ";
    }
  }

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

  static inline const char* ectos(ErrCode code) {return SException::ectos(code);}

};

class ErrLogger {
  std::ofstream logfile;
  std::ostream& console = std::cerr;

  ErrLogger& out(const std::string str) {
    logfile << str;
    console << str;
    return *this;
  }

  ErrLogger& out(const char* str) {
    logfile << str;
    console << str;
    return *this;
  }

public:

  ErrLogger& operator<<(std::string str) {return out(str);}
  ErrLogger& operator<<(const char* str) {return out(str);}

  ErrLogger& operator<<(SException except) {
    return
         out(SException::ectos(except.code()))
        .out(except.what()?": ":" ")
        .out(except.what()?except.what():"");
  }

  ErrLogger& operator<<(Exception except) {
    out(SException::ectos(except.code()));
    std::string msg = except.what();
    if(msg.empty()) return *this;
    else return out(":").out(msg);
  }
};

extern ErrLogger erlog;

}
