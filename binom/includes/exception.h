#include "ctypes.h"
#include <string>

namespace binom {

enum ErrCode : ui8 {

    // General exceptions
    any,
    memory_allocation_error,
    memory_free_error,
    out_of_range,
    file_open_error,

    // BinOM Exceptions
    binom_invalid_type,
    binom_out_of_range,
    binom_object_key_error,
    binom_query_invalid_field,

    // BinOM DataBase Exceptions
    binomdb_invalid_file,
    binomdb_invalid_storage_version,
    binomdb_memory_management_error,
    binomdb_page_isnt_exist,
    binomdb_block_isnt_exist,
};

class Exception {
  ErrCode _code;
  const char* error_string = nullptr;

  static const char* ectos(ErrCode code) {
    switch (code) {
      case ErrCode::memory_allocation_error: return   "Memory allocation error";
      case ErrCode::memory_free_error: return         "Memory free error";
      case ErrCode::out_of_range: return              "Out of range";
      case ErrCode::file_open_error: return           "File open error";

      case ErrCode::binom_invalid_type: return        "Invalid BinOM type";
      case ErrCode::binom_out_of_range: return        "Out of BinOM container range";
      case ErrCode::binom_object_key_error: return    "Invalide object key";

      case ErrCode::binomdb_invalid_file: return      "BinOM DB invalid file";
      case ErrCode::binomdb_invalid_storage_version:
      return                                          "BinOM DB invalid file storage version";
      case ErrCode::binomdb_memory_management_error:
      return                                          "BinOM DB memory management error";
      case ErrCode::binomdb_page_isnt_exist:
      return                                          "BinOM DB page isn't exist";
      case ErrCode::binomdb_block_isnt_exist:
      return                                          "BinOM DB block isn't exist";
      case ErrCode::binom_query_invalid_field:
      return                                          "BinOM invalid Query field";

      case ErrCode::any: return                       "Unknown exception";
      default: return                                 "INVALID ErrCode!";
    }
  }

public:
  constexpr Exception(const ErrCode code, const char* error_string) : _code(code), error_string(error_string) {}
  constexpr Exception(const ErrCode code) : _code(code), error_string(nullptr) {}
  ErrCode code() const {return _code;}
  std::string what() const {
    return error_string
        ? std::string(ectos(_code)) + ": " + error_string
        : std::string(ectos(_code));
  }
  [[noreturn]] inline void throwUp() {throw *this;}

};

}
