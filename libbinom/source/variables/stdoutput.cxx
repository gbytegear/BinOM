#include "libbinom/include/variables/stdoutput.hxx"

using namespace binom;

const char* getTypeString(VarType type) {
  if(!OutCFG::type_info) return "";
  switch(type) {
  case binom::VarType::separator: return "!SEP! ";
  default:
  case binom::VarType::invalid_type: return "!TINV! ";
  case binom::VarType::null: return "null ";
  case binom::VarType::boolean: return "bool ";
  case binom::VarType::ui8: return "ui8 ";
  case binom::VarType::si8: return "i8 ";
  case binom::VarType::ui16: return "ui16 ";
  case binom::VarType::si16: return "i16 ";
  case binom::VarType::ui32: return "ui32 ";
  case binom::VarType::si32: return "i32 ";
  case binom::VarType::f32: return "f32 ";
  case binom::VarType::ui64: return "ui64 ";
  case binom::VarType::si64: return "i64 ";
  case binom::VarType::f64: return "f64 ";
  case binom::VarType::bit_array: return "bitarr ";
  case binom::VarType::ui8_array: return "ui8arr ";
  case binom::VarType::si8_array: return "i8arr ";
  case binom::VarType::ui16_array: return "ui16arr ";
  case binom::VarType::si16_array: return "i16arr ";
  case binom::VarType::ui32_array: return "ui32arr ";
  case binom::VarType::si32_array: return "i32arr ";
  case binom::VarType::f32_array: return "f32arr ";
  case binom::VarType::ui64_array: return "ui64arr ";
  case binom::VarType::si64_array: return "i64arr ";
  case binom::VarType::f64_array: return "f64arr ";
  case binom::VarType::array: return "array ";
  case binom::VarType::list: return "list ";
  case binom::VarType::map: return "map ";
  case binom::VarType::multimap: return "multimap ";
  case binom::VarType::table: return "table ";
  }
}

std::ostream& operator << (std::ostream& os, binom::Variable value) {
  switch(value.getTypeClass()) {
  case binom::VarTypeClass::null: return os << "null";
  case binom::VarTypeClass::number:
    return os << std::move(value.toNumber());
  case binom::VarTypeClass::bit_array:
    return os << std::move(value.toBitArray());
  case binom::VarTypeClass::buffer_array:
    return os << std::move(value.toBufferArray());
  case binom::VarTypeClass::array:
    return os << std::move(value.toArray());
  case binom::VarTypeClass::list:
    return os << std::move(value.toList());
  case binom::VarTypeClass::map:
    return os << std::move(value.toMap());
  case binom::VarTypeClass::multimap:
    return os << std::move(value.toMultiMap());
  case binom::VarTypeClass::table:
//    return os << std::move(value.toTable()); // TODO

  default:
  case binom::VarTypeClass::invalid_type:
    return os << "TINVAL!";
  }
}

std::ostream& operator << (std::ostream& os, binom::Number value) {
  switch(value.getValType()) {
  case ValType::boolean:
    return os << getTypeString(value.getType()) << std::boolalpha << bool(value) << std::noboolalpha;
  case ValType::ui8: return os << getTypeString(value.getType()) << ui16(value);
  case ValType::si8: return os << getTypeString(value.getType()) << i16(value);
  case ValType::ui16: return os << getTypeString(value.getType()) << ui16(value);
  case ValType::si16: return os << getTypeString(value.getType()) << i16(value);
  case ValType::ui32: return os << getTypeString(value.getType()) << ui32(value);
  case ValType::si32: return os << getTypeString(value.getType()) << i32(value);
  case ValType::f32: return os << getTypeString(value.getType()) << f32(value);
  case ValType::ui64: return os << getTypeString(value.getType()) << ui64(value);
  case ValType::si64: return os << getTypeString(value.getType()) << i64(value);
  case ValType::f64: return os << getTypeString(value.getType()) << f64(value);
  default: return os;
  }
}

std::ostream& operator << (std::ostream& os, binom::BitArray value) {
  os << getTypeString(VarType::bit_array) << "[ " << std::boolalpha;
  bool is_first = true;
  for(auto element : value) {
    os << (is_first?(is_first = false, "") : ", ") << bool(element);
  }
  return os << std::noboolalpha << ']';
}

std::ostream& operator << (std::ostream& os, binom::BufferArray value) {
  os << getTypeString(value.getType());
  if(value.isPrintable()) return os << ' ' << std::string(value);
  os << "[";
  bool is_first = true;
  switch (value.getValType()) {
  default: break;
  case binom::ValType::ui8:
    for(auto element : value) os << (is_first?(is_first = false, "") : ", ") << ui16(element);
  break;
  case binom::ValType::si8:
    for(auto element : value) os << (is_first?(is_first = false, "") : ", ") << i16(element);
  break;
  case binom::ValType::ui16:
    for(auto element : value) os << (is_first?(is_first = false, "") : ", ") << ui16(element);
  break;
  case binom::ValType::si16:
    for(auto element : value) os << (is_first?(is_first = false, "") : ", ") << i16(element);
  break;
  case binom::ValType::ui32:
    for(auto element : value) os << (is_first?(is_first = false, "") : ", ") << ui32(element);
  break;
  case binom::ValType::si32:
    for(auto element : value) os << (is_first?(is_first = false, "") : ", ") << i32(element);
  break;
  case binom::ValType::f32:
    for(auto element : value) os << (is_first?(is_first = false, "") : ", ") << f32(element);
  break;
  case binom::ValType::ui64:
    for(auto element : value) os << (is_first?(is_first = false, "") : ", ") << ui64(element);
  break;
  case binom::ValType::si64:
    for(auto element : value) os << (is_first?(is_first = false, "") : ", ") << i64(element);
  break;
  case binom::ValType::f64:
    for(auto element : value) os << (is_first?(is_first = false, "") : ", ") << f64(element);
  break;
  }
  return os << ']';
}

std::ostream& operator << (std::ostream& os, binom::Array value) {
  os << getTypeString(VarType::array) << "[";
  bool is_first = true;
  for(auto element : value) os << (is_first?(is_first = false, "") : ", ") << element.move();
  return os << ']';
}

std::ostream& operator << (std::ostream& os, binom::List value) {
  os << getTypeString(VarType::list) << "<";
  bool is_first = true;
  for(auto element : value)
    os << (is_first?(is_first = false, "") : ", ") << element.move();
  return os << '>';
}

std::ostream& operator << (std::ostream& os, binom::Map value) {
  os << getTypeString(VarType::map) << "{";
  bool is_first = true;
  for(auto element : value)
    os << (is_first?(is_first = false, "") : ", ") << element.getKey() << " : " << element.getValue();
  return os << '}';
}

std::ostream& operator << (std::ostream& os, binom::MultiMap value) {
  os << getTypeString(VarType::multimap) << "{";
  bool is_first = true;
  for(auto element : value)
    os << (is_first?(is_first = false, "") : ", ") << element.getKey() << " : " << element.getValue();
  return os << '}';
}

std::ostream& operator << (std::ostream& os, binom::Table value) {
  return os; // TODO
}
