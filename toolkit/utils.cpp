#include "utils.h"
#include <climits>

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

ValType toValType(std::string str) {
  if(str == "byte") return ValType::byte;
  elif(str == "word") return ValType::word;
  elif(str == "dword") return ValType::dword;
  elif(str == "qword") return ValType::qword;
  else return ValType::invalid_type;;
}

Primitive inputPrimitive(ValType type) {
  do{
    std::string input_type;
    std::clog << "Input as(signed/unsigned): ";
    std::cin >> input_type;
    if(input_type == "signed") switch (type) {
      case binom::ValType::byte: return Primitive(static_cast<i8>(inputInt("Enter value", 0x7f, 0x80)));
      case binom::ValType::word: return Primitive(static_cast<i16>(inputInt("Enter value", 0x7fff, 0x8000)));
      case binom::ValType::dword: return Primitive(static_cast<i32>(inputInt("Enter value", 0x7fffffff, 0x80000000)));
      case binom::ValType::qword: return Primitive(inputInt("Enter value", 0x7fffffffffffffff, 0x8000000000000000));
      case binom::ValType::invalid_type: throw Exception(ErrCode::binom_invalid_type);
    } elif(input_type == "unsigned") switch (type) {
      case binom::ValType::byte: return Primitive(static_cast<ui8>(inputUInt("Enter value", 0, 0xff)));
      case binom::ValType::word: return Primitive(static_cast<ui16>(inputUInt("Enter value", 0, 0xffff)));
      case binom::ValType::dword: return Primitive(static_cast<ui32>(inputUInt("Enter value", 0, 0xffffffff)));
      case binom::ValType::qword: return Primitive(inputUInt("Enter value", 0, 0xffffffffffffffff));
      case binom::ValType::invalid_type: throw Exception(ErrCode::binom_invalid_type);
    }
  } while (true);
}

BufferArray inputBufferArray(ValType type) {

  while (type == ValType::invalid_type) {
    std::clog <<
      "BinOM value types:\n"
      "| 1. byte\n"
      "| 2. word\n"
      "| 3. dword\n"
      "| 4. qword\n";
    int code = inputUInt("Enter value type", 1, 4);
    type = ValType(code - 1);
  }

  if(type == ValType::byte) {
    char answer;
    do {
      std::clog << "Input as string(y/n)? ";
      std::cin >> answer;
      if(answer == 'y') {
        std::string str;
        std::clog << "Enter string: ";
        std::cin >> str;
        return BufferArray(str);
      }
    } while (answer != 'n');
  }

  size_t element_count = inputUInt("Enter element count: ", 0, UINT_MAX);
  std::string input_type;
  BufferArray buffer(toVarType(type));

  do{
    std::clog << "Input as(signed/unsigned): ";
    std::cin >> input_type;
  } while (input_type != "signed" && input_type != "unsigned");


  if(input_type == "signed")
    for(size_t i = 0; i < element_count; ++i) {
      std::clog << "Buffer: [" << buffer << "]\n";
      switch (type) {
        case binom::ValType::byte:
          buffer.pushBack(inputInt("Enter value", 0x7f, 0x80));
        break;
        case binom::ValType::word:
          buffer.pushBack(inputInt("Enter value", 0x7fff, 0x8000));
        break;
        case binom::ValType::dword:
          buffer.pushBack(inputInt("Enter value", 0x7fffffff, 0x80000000));
        break;
        case binom::ValType::qword:
          buffer.pushBack(inputInt("Enter value", 0x7fffffffffffffff, 0x8000000000000000));
        break;
        case binom::ValType::invalid_type: throw Exception(ErrCode::binom_invalid_type);
    }
  } elif(input_type == "unsigned")
    for(size_t i = 0; i < element_count; ++i) {
      std::clog << "Buffer: [" << buffer << "]\n";
      switch (type) {
        case binom::ValType::byte:
          buffer.pushBack(inputUInt("Enter value", 0, 0xff));
        break;
        case binom::ValType::word:
          buffer.pushBack(inputUInt("Enter value", 0, 0xffff));
        break;
        case binom::ValType::dword:
          buffer.pushBack(inputUInt("Enter value", 0, 0xffffffff));
        break;
        case binom::ValType::qword:
          buffer.pushBack(inputUInt("Enter value", 0, 0xffffffffffffffff));
        break;
        case binom::ValType::invalid_type: throw Exception(ErrCode::binom_invalid_type);
      }
  }

  return buffer;
}

Variable inputVariable(bool optional, binom::ValType constraint) {
  VarType type = VarType::invalid_type;
  if(constraint != ValType::invalid_type)
    type = toVarType(constraint);
  else while (type == VarType::invalid_type) {
    std::clog <<
    "BinOM types:\n"
    "| 1. byte\n"
    "| 2. word\n"
    "| 3. dword\n"
    "| 4. qword\n"
    "| 5. byte_array\n"
    "| 6. word_array\n"
    "| 7. dword_array\n"
    "| 8. qword_array\n"
    "| 9. array\n"
    "| 10. object\n"
    << (optional? "| 11. exit - exit from input\n" : "") <<
    "";
    int code = inputUInt("Enter variable type code: ", optional? 0 : 1, 10);
    if(!code) return nullptr;
    type = VarType(code);
  }

  switch (toTypeClass(type)) {
    case binom::VarTypeClass::primitive: return inputPrimitive(toValueType(type));
    case binom::VarTypeClass::buffer_array: return inputBufferArray(toValueType(type));
    case binom::VarTypeClass::array: {
      size_t element_count = 0;
      std::clog << "Enter element count: ";
      std::cin >> element_count;
      Array array;
      for(size_t i = 0; i < element_count; ++i) {
        std::clog << array << '\n';
        array.pushBack(inputVariable());
      }
      return array.asVar();
    }
    case binom::VarTypeClass::object: {
      size_t element_count = 0;
      std::clog << "Enter element count: ";
      std::cin >> element_count;
      Object object;
      for(size_t i = 0; i < element_count; ++i) {
        std::clog << object << '\n';
        object.insert(inputBufferArray(), inputVariable());
      }
      return object.asVar();
    }
    case binom::VarTypeClass::invalid_type: throw Exception(ErrCode::binom_invalid_type);
  }

}

bool isUInt(const char* str) {
  for(const char* it = str; *it; ++it) {
    if(*it >= '0' && *it <= '9') continue;
    else return false;
  }
  return true;
}

bool isInt(const char* str) {
  for(const char* it = str; *it; ++it) {
    if(*it >= '0' && *it <= '9') continue;
    elif(it == str && *it == '-') continue;
    else return false;
  }
  return true;
}

ui64 inputUInt(const char* msg, ui64 left_limit, ui64 right_limit) {
  std::string str;
  do {
    std::clog << msg << " (from " << left_limit << " to " << right_limit << ") ";
    std::cin >> str;
    if(isUInt(str.c_str())){
      ui64 result = std::stoull(str.c_str());
      if(result >= left_limit && result <= right_limit) return result;
      else std::cerr << "Value out of range!\n";
    } else std::cerr << "Entered value isn't integer!\n";
  } while (true);
}

i64 inputInt(const char* msg, i64 left_limit, i64 right_limit) {
  std::string str;
  do {
    std::clog << msg << " (from " << left_limit << " to " << right_limit << ") ";
    std::cin >> str;
    if(isInt(str.c_str())){
      i64 result = std::stoll(str.c_str());
      if(result >= left_limit && result <= right_limit) return result;
      else std::cerr << "Value out of range!\n";
    } else std::cerr << "Entered value isn't integer!\n";
  } while (true);
}

std::string inputString(const char* msg, std::function<bool (std::string&)> condition) {
  std::string str;
  while (true) {
    std::clog << msg << ':';
    std::cin >> str;
    if(condition(str)) return str;
  }
}

void editContainer(UnionNodeVisitor root) {
  while (1) try{
    int command = 0;
    std::clog << "Curren structure:\n"
    << root.getVariable() << "\n"
    "Commands:\n"
    "| 0. Exit\n"
    "| 1. Add variable\n"
    "| 2. Change variable\n"
    "| 3. Remove variable\n";
    command = inputUInt("Enter command code", 0, 3);
    switch (command) {
      case 0: return;



      case 1: {
        UnionNodeVisitor node;
        do {
          try {
            std::string str;
            std::clog << "Enter path: ";
            std::cin >> str;
            if(str == ".") {
              node = root;
            } else
              node = root[Path::fromString(str)];

            if(node.isPrimitive()) {
              std::clog << "Variable is primitive\n";
              continue;
            }
          }  catch (Exception& except) {
            std::cerr << except.full() << '\n';
            continue;
          }
        } while(false);

        std::clog << "Selected variable:\n" << node.getVariable() << '\n';

        switch (node.getTypeClass()) {
          case VarTypeClass::buffer_array: {
            std::clog << "Positions:\n"
            "| 1. Front\n"
            "| 2. By index\n"
            "| 3. Back\n";
            int code = inputUInt("Enter position", 1, 3);
            switch (code) {
              case 1:
                node.pushFront(inputBufferArray(toValueType(node.getType())));
              continue;
              case 2:
                node.insert(inputUInt("Enter index", 0, node.getElementCount() - 1),
                            inputBufferArray(toValueType(node.getType())));
              continue;
              case 3:
                node.pushBack(inputBufferArray(toValueType(node.getType())));
              default: continue;
            }
          } continue;
          case VarTypeClass::array: {
            std::clog << "Positions:\n"
            "| 1. Front\n"
            "| 2. By index\n"
            "| 3. Back\n";
            int code = inputUInt("Enter position", 1, 3);
            switch (code) {
              case 1:
                node.pushFront(inputVariable());
              continue;
              case 2:
                node.insert(inputUInt("Enter index", 0, node.getElementCount() - 1),
                            inputVariable());
              continue;
              case 3:
                node.pushBack(inputVariable());
              continue;
              default: continue;
            }
          } continue;
          case VarTypeClass::object: {
            Array names = node.getVariable().toObject().getNameArray();
            node.insert(inputString("Enter name", [&](std::string& str)->bool{
                          BufferArray name = str;
                          for(Variable in_obj_name : names)
                            if(name == in_obj_name.toBufferArray()) {
                            std::cerr << "Object has entered name!\n";
                            return false;
                          }
                          return true;
                        }), inputVariable());
          } continue;
          default: throw Exception(ErrCode::binom_invalid_type);
        }
      }



      case 2: {
        UnionNodeVisitor node;
        do {
          try {
            std::string str;
            std::clog << "Enter path: ";
            std::cin >> str;
            if(str == ".") {
              node = root;
            } else node = root[Path::fromString(str)];
          }  catch (Exception& except) {
            std::cerr << except.full() << '\n';
            continue;
          }
        } while(false);

        node.setVariable(inputVariable());

      } continue;

      case 3: {
        do {
          try {
            std::string str;
            std::clog << "Enter path: ";
            std::cin >> str;
            root.remove(Path::fromString(str));
          }  catch (Exception& except) {
            std::cerr << except.full() << '\n';
            continue;
          }
        } while(false);
      }
    }
  } catch(Exception& except) {
    std::cerr << except.full() << '\n';
    continue;
  }
}
