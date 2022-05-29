#ifndef ERR_HXX
#define ERR_HXX

#include "type_aliases.hxx"

namespace binom::err {

using namespace type_alias;

enum class ErrorType : ui8 {
  no_error = 0,

  // General exceptions
  any,
  out_of_range,
  file_open_error,
  invalid_data,
  null_ponter_dereference,

  // BinOM Exceptions
  binom_invalid_type,
  binom_resource_not_available,
  binom_out_of_range,
  binom_unique_error,
  binom_query_invalid_field,
  binom_invalid_visitor,

  // BinOM FileStorage Exceptions
  binomfs_invalid_file,
  binomfs_invalid_storage_version,
  binomfs_memory_management_error,
  binomfs_page_isnt_exist,
  binomfs_block_isnt_exist,

  // BinOM Lexer
  unexpected_expression
};

class Error {
  ErrorType err_type;
public:
  constexpr Error(ErrorType code = ErrorType::no_error) : err_type(code) {}
  const char* what() const noexcept;
  inline ErrorType code() const noexcept {return err_type;}
  inline operator ErrorType() const noexcept {return err_type;}
  inline operator bool () const noexcept {return err_type != ErrorType::no_error;}
};

inline const char* Error::what() const noexcept {
  switch (err_type) {
  case ErrorType::no_error: return                  "\n\r";
  case ErrorType::out_of_range: return              "Out of range\n\r";
  case ErrorType::file_open_error: return           "File open error\n\r";
  case ErrorType::invalid_data: return              "Invalid data\n\r";
  case ErrorType::null_ponter_dereference: return   "Trying to dereference null-pointer\n\r";

  case ErrorType::binom_invalid_type: return        "BinOM: Invalid BinOM type\n\r";
  case ErrorType::binom_resource_not_available:
  return                                            "BinOM: Variable resource isn't available\n\r";
  case ErrorType::binom_out_of_range: return        "BinOM: Out of BinOM container range\n\r";
  case ErrorType::binom_unique_error: return        "BinOM: Non-unique key\n\r";
  case ErrorType::binom_invalid_visitor: return     "BinOM: Invalide visitor\n\r";

  case ErrorType::binomfs_invalid_file: return      "BinOM FileStorage: Invalid file\n\r";
  case ErrorType::binomfs_invalid_storage_version:
  return                                            "BinOM FileStorage: Invalid file storage version\n\r";
  case ErrorType::binomfs_memory_management_error:
  return                                            "BinOM FileStorage: Memory management error\n\r";
  case ErrorType::binomfs_page_isnt_exist:
  return                                            "BinOM FileStorage: Page isn't exist\n\r";
  case ErrorType::binomfs_block_isnt_exist:
  return                                            "BinOM FileStorage: Block isn't exist\n\r";
  case ErrorType::binom_query_invalid_field:
  return                                            "BinOM: Invalid Query field\n\r";

  case ErrorType::unexpected_expression: return     "Unexpected expression\n\r";

  case ErrorType::any: return                       "Unknown exception\n\r";
  default: return                                   "Invalid error codes\n\r";
  }
}

}

#endif // ERR_HXX
