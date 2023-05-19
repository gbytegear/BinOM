#ifndef PRINT_VARIABLE_HXX
#define PRINT_VARIABLE_HXX

#include "libbinom/include/variables/key_value.hxx"

#include "libbinom/include/binom.hxx"

#include "tester.hxx"

namespace binom::utils {

class {
  using Variable = binom::Variable;

  void printKey(const KeyValue value) {
    switch (value.getType()) {
    case binom::VarKeyType::null: std::cout << "[null] null"; break;
    case binom::VarKeyType::boolean: std::cout << "[bool] " << std::boolalpha << bool(value.toNumber()) << std::noboolalpha; break;
    case binom::VarKeyType::ui8: std::cout << "[ui8] " << ui16(value.toNumber()); break;
    case binom::VarKeyType::si8: std::cout << "[si8] " << i16(value.toNumber()); break;
    case binom::VarKeyType::ui16: std::cout << "[ui16] " << ui16(value.toNumber()); break;
    case binom::VarKeyType::si16: std::cout << "[si16] " << i16(value.toNumber()); break;
    case binom::VarKeyType::ui32: std::cout << "[ui32] " << ui32(value.toNumber()); break;
    case binom::VarKeyType::si32: std::cout << "[si32] " << i32(value.toNumber()); break;
    case binom::VarKeyType::f32: std::cout << "[f32] " << f32(value.toNumber()); break;
    case binom::VarKeyType::ui64: std::cout << "[ui64] " << ui64(value.toNumber()); break;
    case binom::VarKeyType::si64: std::cout << "[si64] " << i64(value.toNumber()); break;
    case binom::VarKeyType::f64: std::cout << "[f64] " << f64(value.toNumber()); break;
    case binom::VarKeyType::bit_array:
      std::cout << "[bit_array] " << std::boolalpha;
      for(auto value : value.toBitArray()) std::cout << bool(value) << ' ';
      std::cout << std::noboolalpha;
    break;
    case binom::VarKeyType::ui8_array:
      std::cout << "[ui8_array] ";
      if(value.toBufferArray().isPrintable()) std::cout << "[string] " << std::string(value.toBufferArray());
      else for(auto value : value.toBufferArray()) std::cout << ui16(value) << ' ';
    break;
    case binom::VarKeyType::si8_array:
      std::cout << "[si8_array] ";
      if(value.toBufferArray().isPrintable()) std::cout << "[string] " << std::string(value.toBufferArray());
      else for(auto value : value.toBufferArray()) std::cout << i16(value) << ' ';
    break;
    case binom::VarKeyType::ui16_array:
      std::cout << "[ui16_array] ";
      if(value.toBufferArray().isPrintable()) std::wcout << "[wstring] " << std::wstring(value.toBufferArray());
      else for(auto value : value.toBufferArray()) std::cout << ui16(value) << ' ';
    break;
    case binom::VarKeyType::si16_array:
      std::cout << "[si16_array] ";
      if(value.toBufferArray().isPrintable()) std::wcout << "[wstring] " << std::wstring(value.toBufferArray());
      else for(auto value : value.toBufferArray()) std::cout << i16(value) << ' ';
    break;
    case binom::VarKeyType::ui32_array:
      std::cout << "[ui32_array] ";
      if(value.toBufferArray().isPrintable()) std::wcout << "[wstring] " << std::wstring(value.toBufferArray());
      else for(auto value : value.toBufferArray()) std::cout << ui32(value) << ' ';
    break;
    case binom::VarKeyType::si32_array:
      std::cout << "[si32_array] ";
      if(value.toBufferArray().isPrintable()) std::wcout << "[wstring] " << std::wstring(value.toBufferArray());
      else for(auto value : value.toBufferArray()) std::cout << i32(value) << ' ';
    break;
    case binom::VarKeyType::f32_array:
      std::cout << "[f32_array] ";
      if(value.toBufferArray().isPrintable()) std::wcout << "[wstring] " << std::wstring(value.toBufferArray());
      else for(auto value : value.toBufferArray()) std::cout << f32(value) << ' ';
    break;
    case binom::VarKeyType::ui64_array:
      std::cout << "[ui64_array] ";
      for(auto value : value.toBufferArray()) std::cout << ui64(value) << ' ';
    break;
    case binom::VarKeyType::si64_array:
      std::cout << "[si64_array] ";
      for(auto value : value.toBufferArray()) std::cout << i64(value) << ' ';
    break;
    case binom::VarKeyType::f64_array:
      std::cout << "[f64_array] ";
      for(auto value : value.toBufferArray()) std::cout << f64(value) << ' ';
    break;
    default: std::cout << "<unexpected type>"; break;
    }
  }


  void print(const Variable& variable, size_t shift = 0, const char* msg = "") {
    switch (variable.getType()) {
    case binom::VarType::null: std::cout << std::string(shift, '|') << msg << "[null]\n\r"; break;
    case binom::VarType::boolean: std::cout << std::string(shift, '|') << msg << "[bool] " << std::boolalpha << bool(variable.toNumber()) << std::noboolalpha << std::endl; break;
    case binom::VarType::ui8: std::cout << std::string(shift, '|') << msg << "[ui8] " << ui16(variable.toNumber()) << std::endl; break;
    case binom::VarType::si8: std::cout << std::string(shift, '|') << msg << "[si8] " << i16(variable.toNumber()) << std::endl; break;
    case binom::VarType::ui16: std::cout << std::string(shift, '|') << msg << "[ui16] " << ui16(variable.toNumber()) << std::endl; break;
    case binom::VarType::si16: std::cout << std::string(shift, '|') << msg << "[si16] " << i16(variable.toNumber()) << std::endl; break;
    case binom::VarType::ui32: std::cout << std::string(shift, '|') << msg << "[ui32] " << ui32(variable.toNumber()) << std::endl; break;
    case binom::VarType::si32: std::cout << std::string(shift, '|') << msg << "[si32] " << i32(variable.toNumber()) << std::endl; break;
    case binom::VarType::f32: std::cout << std::string(shift, '|') << msg << "[f32] " << f32(variable.toNumber()) << std::endl; break;
    case binom::VarType::ui64: std::cout << std::string(shift, '|') << msg << "[ui64] " << ui64(variable.toNumber()) << std::endl; break;
    case binom::VarType::si64: std::cout << std::string(shift, '|') << msg << "[si64] " << i64(variable.toNumber()) << std::endl; break;
    case binom::VarType::f64: std::cout << std::string(shift, '|') << msg << "[f64] " << f64(variable.toNumber()) << std::endl; break;
    case binom::VarType::bit_array:
      std::cout << std::string(shift, '|') << msg << "[bit_array] " << std::boolalpha;
      for(auto value : variable.toBitArray()) std::cout << bool(value) << ' ';
      std::cout << std::noboolalpha << std::endl;
    break;
    case binom::VarType::ui8_array:
      std::cout << std::string(shift, '|') << msg << "[ui8_array] ";
      if(variable.toBufferArray().isPrintable()) std::cout << "[string] " << std::string(variable.toBufferArray());
      else for(auto value : variable.toBufferArray()) std::cout << ui16(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::si8_array:
      std::cout << std::string(shift, '|') << msg << "[si8_array] ";
      if(variable.toBufferArray().isPrintable()) std::cout << "[string] " << std::string(variable.toBufferArray());
      else for(auto value : variable.toBufferArray()) std::cout << i16(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::ui16_array:
      std::cout << std::string(shift, '|') << msg << "[ui16_array] ";
      if(variable.toBufferArray().isPrintable()) std::wcout << "[wstring] " << std::wstring(variable.toBufferArray());
      else for(auto value : variable.toBufferArray()) std::cout << ui16(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::si16_array:
      std::cout << std::string(shift, '|') << msg << "[si16_array] ";
      if(variable.toBufferArray().isPrintable()) std::wcout << "[wstring] " << std::wstring(variable.toBufferArray());
      else for(auto value : variable.toBufferArray()) std::cout << i16(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::ui32_array:
      std::cout << std::string(shift, '|') << msg << "[ui32_array] ";
      if(variable.toBufferArray().isPrintable()) std::wcout << "[wstring] " << std::wstring(variable.toBufferArray());
      else for(auto value : variable.toBufferArray()) std::cout << ui32(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::si32_array:
      std::cout << std::string(shift, '|') << msg << "[si32_array] ";
      if(variable.toBufferArray().isPrintable()) std::wcout << "[wstring] " << std::wstring(variable.toBufferArray());
      else for(auto value : variable.toBufferArray()) std::cout << i32(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::f32_array:
      std::cout << std::string(shift, '|') << msg << "[f32_array] ";
      if(variable.toBufferArray().isPrintable()) std::wcout << "[wstring] " << std::wstring(variable.toBufferArray());
      else for(auto value : variable.toBufferArray()) std::cout << f32(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::ui64_array:
      std::cout << std::string(shift, '|') << msg << "[ui64_array] ";
      for(auto value : variable.toBufferArray()) std::cout << ui64(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::si64_array:
      std::cout << std::string(shift, '|') << msg << "[si64_array] ";
      for(auto value : variable.toBufferArray()) std::cout << i64(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::f64_array:
      std::cout << std::string(shift, '|') << msg << "[f64_array] ";
      for(auto value : variable.toBufferArray()) std::cout << f64(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::array:
      std::cout << std::string(shift, '|') << msg << "[array]\n\r";
      if(shift >= 100) {
        std::cout << std::string(shift, '|') << "Error: The maximum stack size for the printVariable function has been reached!\n\r";
        return;
      }
      for(const auto& var : variable.toArray()) print(var, shift + 1);
    break;
    case binom::VarType::list:
      std::cout << std::string(shift, '|') << msg << "[list]\n\r";
      if(shift >= 100) {
        std::cout << std::string(shift, '|') << "Error: The maximum stack size for the printVariable function has been reached!\n\r";
        return;
      }
      for(const auto& var : variable.toList())
        print(var, shift + 1);
    break;
    case binom::VarType::map:
      std::cout << std::string(shift, '|') << msg << "[map]\n\r";
      if(shift >= 100) {
        std::cout << std::string(shift, '|') << "Error: The maximum stack size for the printVariable function has been reached!\n\r";
        return;
      }
      for(const auto& named_var : variable.toMap()) {
        std::cout << std::string(shift, '|') << "KEY - "; printKey(named_var.getKey()); std::cout << ":" << std::endl;
        print(named_var.getValue(), shift + 1, "VALUE - ");
      }
    break;
    case binom::VarType::multimap:
      std::cout << std::string(shift, '|') << msg << "[multimap]\n\r";
      if(shift >= 100) {
        std::cout << std::string(shift, '|') << "Error: The maximum stack size for the printVariable function has been reached!\n\r";
        return;
      }
      for(const auto& named_var : variable.toMultiMap()) {
        std::cout << std::string(shift, '|') << "KEY - "; printKey(named_var.getKey()); std::cout << ":" << std::endl;
        print(named_var.getValue(), shift + 1, "VALUE - ");
      }
    break;
    default:
    case binom::VarType::invalid_type: std::cout << std::string(shift, '|') << "<unexpected type>\n\r"; break;
    }
  }
public:
  void operator()(const Variable& var) {
    std::cout << std::string(log_depth, '|') INFO << "\x1B[94m[i] printVariable(const Variable&) \x1B[34m:\n\r";
    GRP_PUSH
    print(var, log_depth);
    GRP_POP
    std::cout << "\033[0m"; std::cout.flush();
  }
  void operator()(Variable&& var) {
    std::cout << std::string(log_depth, '|') INFO << "\x1B[94m[i] printVariable(Variable&&) \x1B[34m:\n\r";
    GRP_PUSH
    print(var, log_depth);
    GRP_POP
    std::cout << "\033[0m"; std::cout.flush();
  }
  void operator()(const KeyValue& var) {
    std::cout << std::string(log_depth, '|') INFO << "\x1B[94m[i] printVariable(const KeyValue&) \x1B[34m:\n\r";
    GRP_PUSH
    print(var, log_depth);
    GRP_POP
    std::cout << "\033[0m"; std::cout.flush();
  }
  void operator()(KeyValue&& var) {
    std::cout << std::string(log_depth, '|') INFO << "\x1B[94m[i] printVariable(KeyValue&&) \x1B[34m:\n\r";
    GRP_PUSH
    print(var, log_depth);
    GRP_POP
    std::cout << "\033[0m"; std::cout.flush();
  }
} printVariable;

}

#endif // PRINT_VARIABLE_HXX
