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

  // BinOM DataBase Exceptions
  binomdb_invalid_file,
  binomdb_invalid_storage_version,
  binomdb_memory_management_error,
  binomdb_page_isnt_exist,
  binomdb_block_isnt_exist,

  // BinOM Lexer
  unexpected_expression
};

class Error {
  ErrorType err_type;
public:
  Error(ErrorType code = ErrorType::no_error) : err_type(code) {}
  const char* what() const noexcept;
  ErrorType code() const noexcept {return err_type;}
  operator ErrorType() const noexcept {return err_type;}
  operator bool () const noexcept {return err_type != ErrorType::no_error;}
};

inline const char* Error::what() const noexcept {
  switch (err_type) {
  case ErrorType::no_error: return                  "";
  case ErrorType::out_of_range: return              "Out of range";
  case ErrorType::file_open_error: return           "File open error";
  case ErrorType::invalid_data: return              "Invalid data";
  case ErrorType::null_ponter_dereference: return   "Trying to dereference null-pointer";

  case ErrorType::binom_invalid_type: return        "Invalid BinOM type";
  case ErrorType::binom_resource_not_available:
  return                                            "BinOM Variable resource isn't available";
  case ErrorType::binom_out_of_range: return        "Out of BinOM container range";
  case ErrorType::binom_unique_error: return        "Non-unique key";
  case ErrorType::binom_invalid_visitor: return     "Invalide visitor";

  case ErrorType::binomdb_invalid_file: return      "BinOM DB invalid file";
  case ErrorType::binomdb_invalid_storage_version:
  return                                            "BinOM DB invalid file storage version";
  case ErrorType::binomdb_memory_management_error:
  return                                            "BinOM DB memory management error";
  case ErrorType::binomdb_page_isnt_exist:
  return                                            "BinOM DB page isn't exist";
  case ErrorType::binomdb_block_isnt_exist:
  return                                            "BinOM DB block isn't exist";
  case ErrorType::binom_query_invalid_field:
  return                                            "BinOM invalid Query field";

  case ErrorType::unexpected_expression: return     "Unexpected expression";

  case ErrorType::any: return                       "Unknown exception";
  default: return                                   "Invalid error codes";
  }
}

}

#endif // ERR_HXX
