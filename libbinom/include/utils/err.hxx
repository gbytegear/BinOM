#ifndef ERR_HXX
#define ERR_HXX

#include "type_aliases.hxx"
#include <exception>

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
  binom_invalid_key_type,
  binom_resource_not_available,
  binom_out_of_range,
  binom_key_unique_error,
  binom_row_has_no_fields_for_indexing,
  binom_invalid_column_name,
  binom_constraint_check_failed,

  // BinOM Critical errors
  binom_data_corrupted
};

class Error : public std::exception {
  ErrorType err_type;
public:
  Error(ErrorType code = ErrorType::no_error) : err_type(code) {}
  Error(const Error& other) : err_type(other.err_type) {}
  Error(Error&& other) : err_type(other.err_type) {}

  Error& operator=(const Error& other) noexcept {err_type = other.err_type; return self;}
  Error& operator=(Error&& other) noexcept {err_type = other.err_type; return self;}

  const char* what() const noexcept override;
  inline ErrorType code() const noexcept {return err_type;}
  inline operator ErrorType() const noexcept {return err_type;}
  inline operator bool () const noexcept {return err_type != ErrorType::no_error;}
};

inline const char* Error::what() const noexcept {
  switch (err_type) {
  case ErrorType::no_error: return                              "Not a error";
  case ErrorType::out_of_range: return                          "Out of range";
  case ErrorType::file_open_error: return                       "File open error";
  case ErrorType::invalid_data: return                          "Invalid data";
  case ErrorType::null_ponter_dereference: return               "Trying to dereference null-pointer";

  case ErrorType::binom_invalid_type: return                    "BinOM: Invalid BinOM type";
  case ErrorType::binom_resource_not_available:
  return                                                        "BinOM: Variable resource isn't available";
  case ErrorType::binom_out_of_range: return                    "BinOM: Out of BinOM container range";
  case ErrorType::binom_key_unique_error: return                "BinOM: Non-unique key";
  case ErrorType::binom_invalid_column_name: return             "BinOM: Column with given name does not exist";
  case ErrorType::binom_row_has_no_fields_for_indexing: return  "BinOM: Row has no fields for indexing";
  case ErrorType::binom_constraint_check_failed: return         "BinOM: Constraint check failed";

  case ErrorType::binom_data_corrupted: return                  "BinOM critical error: Data corrupted";

  case ErrorType::any: return                                   "Unknown exception";
  default: return                                               "Invalid error codes";
  }
}

template<typename T>
class ProgressReport {
  Error error;
  byte data[sizeof(T)] = {};
  T* answer = nullptr;

  T* getData() noexcept {return reinterpret_cast<T*>(data);}
  const T* getData() const noexcept {return reinterpret_cast<const T*>(data);}

public:
  typedef ProgressReport<T> Self;
  typedef ProgressReport<T>& SelfRef;
  typedef const ProgressReport<T> ConstSelf;
  typedef const ProgressReport<T>& ConstSelfRef;
  typedef T Value;
  typedef T& ValueRef;
  typedef T* ValuePtr;
  typedef const T ConstValue;
  typedef const T& ConstValueRef;
  typedef const T* ConstValuePtr;

  ProgressReport(ValuePtr answer_ptr) : error(ErrorType::no_error), answer(answer_ptr) {}
  ProgressReport(ConstValueRef answer) : error(ErrorType::no_error), answer(getData()) {new(getData()) T(answer);}
  ProgressReport(T&& answer) : error(ErrorType::no_error), answer(getData()) {new(getData()) T(std::move(answer));}
  ProgressReport(ErrorType error_type, T* answer_ptr = nullptr) : error(error_type), answer(answer_ptr) {}
  ProgressReport(ErrorType error_type, const T& answer) : error(error_type), answer(getData()) {new(getData()) T(answer);}
  ProgressReport(ErrorType error_type, T&& answer) : error(error_type), answer(getData()) {new(getData()) T(std::move(answer));}

  ProgressReport(const ProgressReport<T>& other) : error(other.error) {
    if(other.isInThisLocated()) {
      new(data.instance) T(other.data.instance);
      answer = &data.instance;
    } else {
      answer = other.answer;
    }
  }

  ProgressReport(ProgressReport<T>&& other) : error(other.error) {
    if(other.isInThisLocated()) {
      new(data.instance) T(std::move(other.data.instance));
      answer = &data.instance;
    } else {
      answer = other.answer;
    }
  }

  template<typename F>
  requires std::is_invocable_v<F, ValueRef>
  SelfRef ifNoError(F callback) {
    if(hasAnswer()) callback(*self);
    return self;
  }

  template<typename F>
  requires std::is_invocable_v<F, ConstValueRef>
  ConstSelfRef ifNoError(F callback) const {
    if(hasAnswer()) callback(*self);
    return self;
  }

  template<typename F>
  requires std::is_invocable_v<F, Error>
  SelfRef ifError(F callback) {
    if(!self) F(error);
    return self;
  }

  template<typename F>
  requires std::is_invocable_v<F, Error>
  ConstSelfRef ifError(F callback) const {
    if(!self) F(self);
    return self;
  }

  ~ProgressReport() { if(isInThisInstanceAnswerLocated()) answer->~T(); }

  SelfRef operator=(const ProgressReport<T>& other) noexcept { new(this) ProgressReport<T>(other); return self; }
  SelfRef operator=(ProgressReport<T>&& other) noexcept { new(this) ProgressReport<T>(other); return self; }

  inline bool isInThisInstanceAnswerLocated() const noexcept { return answer == getData(); }
  inline bool hasAnswer() const noexcept { return answer; }
  inline operator bool () const noexcept { return !error; }
  inline operator ErrorType () const noexcept { return error; }
  inline operator Error () const noexcept { return error; }
  inline ErrorType getErrorCode() const noexcept { return error; }
  const char* what() const noexcept { return error.what(); }

  ValuePtr getAnswer() noexcept {return answer;}
  ValueRef operator*() {return *answer;}
  ValuePtr operator->() noexcept {return answer;}

  ConstValuePtr getAnswer() const noexcept {return answer;}
  ConstValueRef operator*() const {return *answer;}
  ConstValuePtr operator->() const noexcept {return answer;}
};

}

#endif // ERR_HXX
