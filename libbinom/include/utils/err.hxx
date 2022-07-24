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
  binom_key_unique_error,
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
  case ErrorType::binom_key_unique_error: return    "BinOM: Non-unique key\n\r";

  case ErrorType::any: return                       "Unknown exception\n\r";
  default: return                                   "Invalid error codes\n\r";
  }
}

template<typename T>
class ProgressReport {
  Error error;
  T* answer = nullptr;
public:
  ProgressReport(T* answer_ptr) : error(ErrorType::no_error), answer(answer_ptr) {}
  ProgressReport(T& answer) : error(ErrorType::no_error), answer(&answer) {}
  ProgressReport(ErrorType error_type, T* answer_ptr = nullptr) : error(error_type), answer(answer_ptr) {}
  ProgressReport(ErrorType error_type, T& answer) : error(error_type), answer(&answer) {}
  ProgressReport(const ProgressReport<T>& other) : error(other.error), answer(other.answer) {}
  ProgressReport(ProgressReport<T>&& other) : error(other.error), answer(other.answer) {}

  inline operator bool () const noexcept {return error;}
  inline operator ErrorType () const noexcept {return error;}
  inline ErrorType getErrorCode() const noexcept {return error;}
  const char* what() const noexcept {return error.what();}

  T* getAnswer() noexcept {return answer;}
  T& operator*() {return *answer;}
  T* operator->() noexcept {return answer;}

  const T* getAnswer() const noexcept {return answer;}
  const T& operator*() const {return *answer;}
  const T* operator->() const noexcept {return answer;}
};

}

#endif // ERR_HXX
