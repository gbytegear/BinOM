#ifndef PRINT_VARIABLE_HXX
#define PRINT_VARIABLE_HXX

#include "../variables/variable.hxx"
#include "../variables/number.hxx"
#include "../variables/bit_array.hxx"
#include "../variables/buffer_array.hxx"
#include "../variables/array.hxx"
#include "../variables/singly_linked_list.hxx"
#include "../variables/doubly_linked_list.hxx"

#include <iostream>


namespace binom::utils {

class {
  using Variable = binom::Variable;
  void print(const Variable& variable, size_t shift = 0) {
    switch (variable.getType()) {
    case binom::VarType::null: std::cout << std::string(shift, '|') << "null\n\r"; break;
    case binom::VarType::boolean: std::cout << std::string(shift, '|') << "bool: " << std::boolalpha << bool(variable.toNumber()) << std::noboolalpha << std::endl; break;
    case binom::VarType::ui8: std::cout << std::string(shift, '|') << "ui8: " << ui16(variable.toNumber()) << std::endl; break;
    case binom::VarType::si8: std::cout << std::string(shift, '|') << "si8: " << i16(variable.toNumber()) << std::endl; break;
    case binom::VarType::ui16: std::cout << std::string(shift, '|') << "ui16: " << ui16(variable.toNumber()) << std::endl; break;
    case binom::VarType::si16: std::cout << std::string(shift, '|') << "si16: " << i16(variable.toNumber()) << std::endl; break;
    case binom::VarType::ui32: std::cout << std::string(shift, '|') << "ui32: " << ui32(variable.toNumber()) << std::endl; break;
    case binom::VarType::si32: std::cout << std::string(shift, '|') << "si32: " << i32(variable.toNumber()) << std::endl; break;
    case binom::VarType::f32: std::cout << std::string(shift, '|') << "f32: " << f32(variable.toNumber()) << std::endl; break;
    case binom::VarType::ui64: std::cout << std::string(shift, '|') << "ui64: " << ui64(variable.toNumber()) << std::endl; break;
    case binom::VarType::si64: std::cout << std::string(shift, '|') << "si64: " << i64(variable.toNumber()) << std::endl; break;
    case binom::VarType::f64: std::cout << std::string(shift, '|') << "f64: " << f64(variable.toNumber()) << std::endl; break;
    case binom::VarType::bit_array:
      std::cout << std::string(shift, '|') << "bit_array: " << std::boolalpha;
      for(auto value : variable.toBitArray()) std::cout << bool(value) << ' ';
      std::cout << std::noboolalpha << std::endl;
    break;
    case binom::VarType::ui8_array:
      std::cout << std::string(shift, '|') << "ui8_array: ";
      for(auto value : variable.toBufferArray()) std::cout << ui16(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::si8_array:
      std::cout << std::string(shift, '|') << "si8_array: ";
      for(auto value : variable.toBufferArray()) std::cout << i16(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::ui16_array:
      std::cout << std::string(shift, '|') << "ui16_array: ";
      for(auto value : variable.toBufferArray()) std::cout << ui16(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::si16_array:
      std::cout << std::string(shift, '|') << "si16_array: ";
      for(auto value : variable.toBufferArray()) std::cout << i16(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::ui32_array:
      std::cout << std::string(shift, '|') << "ui32_array: ";
      for(auto value : variable.toBufferArray()) std::cout << ui32(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::si32_array:
      std::cout << std::string(shift, '|') << "si32_array: ";
      for(auto value : variable.toBufferArray()) std::cout << i32(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::f32_array:
      std::cout << std::string(shift, '|') << "f32_array: ";
      for(auto value : variable.toBufferArray()) std::cout << f32(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::ui64_array:
      std::cout << std::string(shift, '|') << "ui64_array: ";
      for(auto value : variable.toBufferArray()) std::cout << ui64(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::si64_array:
      std::cout << std::string(shift, '|') << "si64_array: ";
      for(auto value : variable.toBufferArray()) std::cout << i64(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::f64_array:
      std::cout << std::string(shift, '|') << "f64_array: ";
      for(auto value : variable.toBufferArray()) std::cout << f64(value) << ' ';
      std::cout << std::endl;
    break;
    case binom::VarType::array:
      std::cout << std::string(shift, '|') << "array:\n\r";
      if(shift >= 100) {
        std::cout << std::string(shift, '|') << "Error: The maximum stack size for the printVariable function has been reached!\n\r";
        return;
      }
      for(const auto& var : variable.toArray()) print(var, shift + 1);
    break;
    case binom::VarType::singly_linked_list:
      std::cout << std::string(shift, '|') << "singly_linked_list:\n\r";
      if(shift >= 100) {
        std::cout << std::string(shift, '|') << "Error: The maximum stack size for the printVariable function has been reached!\n\r";
        return;
      }
      for(const auto& var : variable.toSinglyLinkedList()) print(var, shift + 1);
    break;
    case binom::VarType::doubly_linked_list:
      std::cout << std::string(shift, '|') << "doubly_linked_list:\n\r";
      if(shift >= 100) {
        std::cout << std::string(shift, '|') << "Error: The maximum stack size for the printVariable function has been reached!\n\r";
        return;
      }
      for(const auto& var : variable.toDoublyLinkedList()) print(var, shift + 1);
    break;
    default:
    case binom::VarType::invalid_type: std::cout << "unexpected type\n\r"; break;
    }
  }
public:
  void operator()(const Variable& var) {print(var);}
  void operator()(Variable&& var) {print(var);}
} printVariable;

}

#endif // PRINT_VARIABLE_HXX
