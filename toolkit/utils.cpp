#include "utils.h"

using namespace binom;

NumberType getNumberType(const char* str) {
  bool is_negative = false;
  bool has_point = false;
  for(const char* it = str; *it; ++it) {
    if(*it >= '0' && *it <= '9') continue;
    elif(!has_point && *it == '.') has_point = true;
    elif(it == str && *it == '-') is_negative = true;
    else return NumberType::not_a_number;
  }
  return has_point? NumberType::float_number
                  :is_negative? NumberType::signed_integer_number
                              : NumberType::unsigned_integer_number;
}

Variable parseArg(int argc, char* argv[], const char* default_arg) {
  --argc; ++argv; // Skip program name
  Object arguments = vobj{};
  Array* default_conatiner = nullptr;

  Array* current_arg = nullptr;
  for(int i = 0; i < argc; ++i) {
    if(argv[i][0] == '-') {
      current_arg = &arguments.insert(++argv[i], varr{}).toArray();
    } elif(current_arg) {
      current_arg->pushBack(argv[i]);
    } else {
      if(!default_conatiner) default_conatiner = &arguments.insert(default_arg, varr{}).toArray();
      arguments[default_arg].toArray().pushBack(argv[i]);
    }
  }

  return arguments.asVar();
}

void openFile(std::string file_name, std::function<void (BinOMDataBase)> db_callback, std::function<void (BinOMFile)> file_callback, std::function<void (Exception&)> err_callback) {
  try {
    if(!FileIO::isExist(file_name)) throw Exception(ErrCode::file_open_error, "File isn't exist");
    if(BinOMDataBase::isValid(file_name))
      db_callback(file_name);
    else
      file_callback(file_name);
  } catch (Exception& except) {
    err_callback(except);
  } catch(std::exception& except) {
    throw except;
  }
}

FileType toFileType(std::string str) {
  if(str == "serialized") return FileType::serialized_data;
  elif(str == "data_base") return FileType::data_base;
  else return FileType::invalid;
}

VarType toType(std::string str) {
  if(str == "byte") return VarType::byte;
  elif(str == "word") return VarType::word;
  elif(str == "dword") return VarType::dword;
  elif(str == "qword") return VarType::qword;
  elif(str == "byte_array") return VarType::byte_array;
  elif(str == "word_array") return VarType::word_array;
  elif(str == "dword_array") return VarType::dword_array;
  elif(str == "qword_array") return VarType::qword_array;
  elif(str == "array") return VarType::array;
  elif(str == "object") return VarType::object;
  else return VarType::invalid_type;;
}

i64 inputSigned(i64 rlimit, i64 llimit) {
  i64 number;
  do {
    std::clog << "Enter value(from " << llimit << " to " << rlimit << "): ";
    std::cin >> number;
    if(number > rlimit || number < llimit) {
      std::clog << "Warning: Out of limit\n";
      continue;
    }
  } while (false);
  return number;
}

ui64 inputUnsigned(ui64 limit) {
  ui64 number;
  do {
    std::clog << "Enter value(from 0 to " << limit << "): ";
    std::cin >> number;
    if(number > limit) {
      std::clog << "Warning: Out of limit\n";
      continue;
    }
  } while (false);
  return number;
}

Primitive inputPrimitive(ValType type) {
  do{
    std::string input_type;
    std::clog << "Input as(signed/unsigned): ";
    std::cin >> input_type;
    if(input_type == "signed") switch (type) {
      case binom::ValType::byte: return Primitive(static_cast<i8>(inputSigned(0x7f, 0x80)));
      case binom::ValType::word: return Primitive(static_cast<i16>(inputSigned(0x7fff, 0x8000)));
      case binom::ValType::dword: return Primitive(static_cast<i32>(inputSigned(0x7fffffff, 0x80000000)));
      case binom::ValType::qword: return Primitive(inputSigned(0x7fffffffffffffff, 0x8000000000000000));
      case binom::ValType::invalid_type: throw Exception(ErrCode::binom_invalid_type);
    } elif(input_type == "unsigned") switch (type) {
      case binom::ValType::byte: return Primitive(static_cast<ui8>(inputUnsigned(0xff)));
      case binom::ValType::word: return Primitive(static_cast<ui16>(inputUnsigned(0xffff)));
      case binom::ValType::dword: return Primitive(static_cast<ui32>(inputUnsigned(0xffffffff)));
      case binom::ValType::qword: return Primitive(inputUnsigned(0xffffffffffffffff));
      case binom::ValType::invalid_type: throw Exception(ErrCode::binom_invalid_type);
    }
  } while (true);
}

Variable inputVariable(bool optional, binom::ValType constraint) {
  VarType type = VarType::invalid_type;
  if(constraint != ValType::invalid_type)
    type = toVarType(constraint);
  else while (type == VarType::invalid_type) {
    std::string str;
    std::clog <<
    "BinOM types:\n"
    "| byte\n"
    "| word\n"
    "| dword\n"
    "| qword\n"
    "| byte_array\n"
    "| word_array\n"
    "| dword_array\n"
    "| qword_array\n"
    "| array\n"
    "| object\n"
    << (optional? "| exit - exit from input\n" : "") <<
    "Enter variable type: ";
    std::cin >> str;
    if(optional && str == "exit") return nullptr;
    type = toType(str);
  }

  switch (type) {
    case binom::VarType::byte:
    case binom::VarType::word:
    case binom::VarType::dword:
    case binom::VarType::qword: return inputPrimitive(toValueType(type));
    case binom::VarType::byte_array: // TODO: all...
    break;
    case binom::VarType::word_array:
    break;
    case binom::VarType::dword_array:
    break;
    case binom::VarType::qword_array:
    break;
    case binom::VarType::array:
    break;
    case binom::VarType::object:
    break;
    default: return nullptr;
  }

}
