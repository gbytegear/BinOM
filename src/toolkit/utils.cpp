#include "utils.h"
#include "cli.h"

#ifdef _WIN32
#include <windows.h>
void clearConsole() {
  COORD topLeft  = { 0, 0 };
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO screen;
  DWORD written;

  GetConsoleScreenBufferInfo(console, &screen);
  FillConsoleOutputCharacterA( console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written );
  FillConsoleOutputAttribute( console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
                              screen.dwSize.X * screen.dwSize.Y, topLeft, &written );
  SetConsoleCursorPosition(console, topLeft);
}
#else // POSIX
#include <iostream>
void clearConsole() {std::clog << "\e[1;1H\e[2J";}
#endif

using namespace binom;

UnionNodeVisitor inputPath(UnionNodeVisitor& root_node) {
  while (true) {
    std::string str;
    std::cout << root_node.getVariable() << "\n\n"
    "Enter path (Type '.' - for select root node): ";
    std::cin >> str;
    try {
      return root_node[Path::fromString(str)];
    } catch (const Exception& except) {
      std::cerr << except.full() << '\n';
      continue;
    }
  }
}



bool isSignedInt(const std::string& str) {
  for(std::string::const_iterator c_it = str.cbegin(),
                                  c_end = str.cend();
                                  c_it != c_end;
                                  ++c_it) {
    if(*c_it >= '0' && *c_it <= '9') continue;
    elif(c_it == str.cbegin() && *c_it == '-')continue;
    else return false;
  }
  return true;
}

bool isUnsignedInt(const std::string& str) {
  for(std::string::const_iterator c_it = str.cbegin(),
                                  c_end = str.cend();
                                  c_it != c_end;
                                  ++c_it) {
    if(*c_it >= '0' && *c_it <= '9') continue;
    else return false;
  }
  return true;
}

ui64 inputInt() {
  while (true) {
    std::string str;
    std::cin >> str;
    if(isSignedInt(str)) return std::stoull(str);
    std::cerr << "Isn't integer!\nTry again: ";
  }
}



Variable inputVariable(VarType type) {
  std::string input;
  i64 value;
  int as;
  switch (type) {
  default:
  case binom::VarType::invalid_type:
  case binom::VarType::end: return nullptr;

  case binom::VarType::byte:
    while (true) {
      std::clog << "Input as:\n"
                 "| 1 - unsigned\n"
                 "| 2 - signed\n"
                 ": ";
      as = inputInt();
      if(as == 1 || as == 2) break;
    }

    switch (as) {
    default:
    case 1:
      while (true) {
        std::clog << "Enter ui8 value (0...255): ";
        std::cin >> input;
        if(isUnsignedInt(input)) {
          value = std::stoll(input);
          if(value >= 0 && value <= 255)
            return ui8(value);
        }
        std::clog << "Invalid input!";
      }

    case 2:
      while (true) {
        std::clog << "Enter i8 value (-128...127): ";
        std::cin >> input;
        if(isSignedInt(input)) {
          value = std::stoll(input);
          if(value >= -128 && value <= 127)
            return i8(value);
        }
        std::clog << "Invalid input!";
      }
    }

  case binom::VarType::word:
    while (true) {
      std::clog << "Input as:\n"
                 "| 1 - unsigned\n"
                 "| 2 - signed\n"
                 ": ";
      as = inputInt();
      if(as == 1 || as == 2) break;
    }

    switch (as) {
    default:
    case 1:
      while (true) {
        std::clog << "Enter ui16 value (0...65535): ";
        std::cin >> input;
        if(isUnsignedInt(input)) {
          value = std::stoll(input);
          if(value >= 0 && value <= 65535)
            return ui16(value);
        }
        std::clog << "Invalid input!";
      }

    case 2:
      while (true) {
        std::clog << "Enter i16 value (-32768...32767): ";
        std::cin >> input;
        if(isSignedInt(input)) {
          value = std::stoll(input);
          if(value >= -32768 && value <= 32767)
            return i16(value);
        }
        std::clog << "Invalid input!";
      }
    }

  case binom::VarType::dword:
    while (true) {
      std::clog << "Input as:\n"
                 "| 1 - unsigned\n"
                 "| 2 - signed\n"
                 ": ";
      as = inputInt();
      if(as == 1 || as == 2) break;
    }

    switch (as) {
    default:
    case 1:
      while (true) {
        std::clog << "Enter ui32 value (0...4294967295): ";
        std::cin >> input;
        if(isUnsignedInt(input)) {
          value = std::stoll(input);
          if(value >= 0 && value <= 4294967295)
            return ui32(value);
        }
        std::clog << "Invalid input!";
      }

    case 2:
      while (true) {
        std::clog << "Enter i32 value (-2147483648 ...2147483647): ";
        std::cin >> input;
        if(isSignedInt(input)) {
          value = std::stoll(input);
          if(value >= -2147483648 && value <= 2147483647)
            return i32(value);
        }
        std::clog << "Invalid input!";
      }
    }

  case binom::VarType::qword:
    while (true) {
      std::clog << "Input as:\n"
                 "| 1 - unsigned\n"
                 "| 2 - signed\n"
                 ": ";
      as = inputInt();
      if(as == 1 || as == 2) break;
    }

    switch (as) {
    default:
    case 1:
      while (true) {
        std::clog << "Enter ui64 value (0...18446744073709551615): ";
        std::cin >> input;
        if(isUnsignedInt(input)) {
          ui64 value = std::stoull(input);
          if(value >= 0 && value <= 18446744073709551615_ui64)
            return ui64(value);
        }
        std::clog << "Invalid input!";
      }

    case 2:
      while (true) {
        std::clog << "Enter i64 value (-9223372036854775808...9223372036854775807): ";
        std::cin >> input;
        if(isSignedInt(input)) {
          value = std::stoll(input);
          if(value >= -9223372036854775808_i64 && value <= 9223372036854775807_i64)
            return i64(value);
        }
        std::clog << "Invalid input!";
      }
    }

  case binom::VarType::byte_array:
    while (true) {
      std::clog << "Input as:\n"
                 "| 1 - unsigned\n"
                 "| 2 - signed\n"
                 "| 3 - string\n"
                 ": ";
      as = inputInt();
      if(as >= 1 && as <= 3) break;
    }

    switch (as) {
    default:
    case 1: {
      ui64 count = 0;
      while (true) {
        std::clog << "Enter element count: ";
        std::cin >> input;
        if(isUnsignedInt(input)) {
          count = std::stoull(input);
          if(count >= 0 && count <= 18446744073709551615_ui64)
            break;
        }
        std::clog << "Invalid input!";
      }

      BufferArray buffer(VarType::byte_array);
      for(ui64 i = 0; i < count; ++i) {
        clearConsole();
        std::clog << buffer << '\n';
        while (true) {
          std::clog << "Enter ui8 value (0...255): ";
          std::cin >> input;
          if(isUnsignedInt(input)) {
            value = std::stoll(input);
            if(value >= 0 && value <= 255) {
              buffer.popBack(value);
              break;
            }
          }
          std::clog << "Invalid input!";
        }
      }

      return buffer;
    }

    case 2: {
      ui64 count = 0;
      while (true) {
        std::clog << "Enter element count: ";
        std::cin >> input;
        if(isUnsignedInt(input)) {
          count = std::stoull(input);
          if(count >= 0 && count <= 18446744073709551615_ui64)
            break;
        }
        std::clog << "Invalid input!";
      }

      BufferArray buffer(VarType::byte_array);
      for(ui64 i = 0; i < count; ++i) {
        clearConsole();
        std::clog << buffer << '\n';
        while (true) {
          std::clog << "Enter i8 value (-128...127): ";
          std::cin >> input;
          if(isSignedInt(input)) {
            value = std::stoll(input);
            if(value >= -128 && value <= 127) {
              buffer.popBack(value);
              break;
            }
          }
          std::clog << "Invalid input!";
        }
      }

      return buffer;
    }

    case 3:
      std::clog << "Enter string: ";
      std::cin.ignore();
      std::getline(std::cin, input);
    return input;
    }

  case binom::VarType::word_array: {
    while (true) {
      std::clog << "Input as:\n"
                 "| 1 - unsigned\n"
                 "| 2 - signed\n"
                 ": ";
      as = inputInt();
      if(as == 1 || as == 2) break;
    }
    ui64 count = 0;
    while (true) {
      std::clog << "Enter element count: ";
      std::cin >> input;
      if(isUnsignedInt(input)) {
        count = std::stoull(input);
        if(count >= 0 && count <= 18446744073709551615_ui64)
          break;
      }
      std::clog << "Invalid input!";
    }
    BufferArray buffer(VarType::word_array);

    switch (as) {
    default:
    case 1:
      for(ui64 i = 0; i < count; ++i) {
        clearConsole();
        std::clog << buffer << '\n';
        while (true) {
          std::clog << "Enter ui16 value (0...65535): ";
          std::cin >> input;
          if(isUnsignedInt(input)) {
            value = std::stoll(input);
            if(value >= 0 && value <= 65535) {
              buffer.popBack(value);
              break;
            }
          }
          std::clog << "Invalid input!";
        }
      }

    return buffer;

    case 2:
      for(ui64 i = 0; i < count; ++i) {
        clearConsole();
        std::clog << buffer << '\n';
        while (true) {
          std::clog << "Enter i16 value (-32768...32767): ";
          std::cin >> input;
          if(isSignedInt(input)) {
            value = std::stoll(input);
            if(value >= -32768 && value <= 32767) {
              buffer.popBack(value);
              break;
            }
          }
          std::clog << "Invalid input!";
        }
      }

      return buffer;
    }
  }

  case binom::VarType::dword_array: {
    while (true) {
      std::clog << "Input as:\n"
                 "| 1 - unsigned\n"
                 "| 2 - signed\n"
                 ": ";
      as = inputInt();
      if(as == 1 || as == 2) break;
    }
    ui64 count = 0;
    while (true) {
      std::clog << "Enter element count: ";
      std::cin >> input;
      if(isUnsignedInt(input)) {
        count = std::stoull(input);
        if(count >= 0 && count <= 18446744073709551615_ui64)
          break;
      }
      std::clog << "Invalid input!";
    }
    BufferArray buffer(VarType::dword_array);

    switch (as) {
    default:
    case 1:
      for(ui64 i = 0; i < count; ++i) {
        clearConsole();
        std::clog << buffer << '\n';
        while (true) {
          std::clog << "Enter ui32 value (0...4294967295): ";
          std::cin >> input;
          if(isUnsignedInt(input)) {
            value = std::stoll(input);
            if(value >= 0 && value <= 4294967295) {
              buffer.popBack(value);
              break;
            }
          }
          std::clog << "Invalid input!";
        }
      }

    return buffer;

    case 2:
      for(ui64 i = 0; i < count; ++i) {
        clearConsole();
        std::clog << buffer << '\n';
        while (true) {
          std::clog << "Enter i32 value (-2147483648 ...2147483647): ";
          std::cin >> input;
          if(isSignedInt(input)) {
            value = std::stoll(input);
            if(value >= -2147483648 && value <= 2147483647) {
              buffer.popBack(value);
              break;
            }
          }
          std::clog << "Invalid input!";
        }
      }

      return buffer;
    }
  }

  case binom::VarType::qword_array: {
    while (true) {
      std::clog << "Input as:\n"
                 "| 1 - unsigned\n"
                 "| 2 - signed\n"
                 ": ";
      as = inputInt();
      if(as == 1 || as == 2) break;
    }
    ui64 count = 0;
    while (true) {
      std::clog << "Enter element count: ";
      std::cin >> input;
      if(isUnsignedInt(input)) {
        count = std::stoull(input);
        if(count >= 0 && count <= 18446744073709551615_ui64)
          break;
      }
      std::clog << "Invalid input!";
    }

    BufferArray buffer(VarType::qword_array);

    switch (as) {
    default:
    case 1:

      for(ui64 i = 0; i < count; ++i) {
        clearConsole();
        std::clog << buffer << '\n';
        while (true) {
          std::clog << "Enter ui64 value (0...18446744073709551615): ";
          std::cin >> input;
          if(isUnsignedInt(input)) {
            ui64 value = std::stoull(input);
            if(value >= 0 && value <= 18446744073709551615_ui64) {
              buffer.popBack(value);
              break;
            }
          }
          std::clog << "Invalid input!";
        }
      }

    return buffer;

    case 2:

      for(ui64 i = 0; i < count; ++i) {
        clearConsole();
        std::clog << buffer << '\n';
        while (true) {
          std::clog << "Enter i64 value (-9223372036854775808...9223372036854775807): ";
          std::cin >> input;
          if(isSignedInt(input)) {
            value = std::stoll(input);
            if(value >= -9223372036854775808_i64 && value <= 9223372036854775807_i64) {
              buffer.popBack(value);
              break;
            }
          }
          std::clog << "Invalid input!";
        }
      }

      return buffer;
    }
  }

  case binom::VarType::array: {
    Array array;
    ui64 count = 0;
    while (true) {
      std::clog << "Enter element count: ";
      std::cin >> input;
      if(isUnsignedInt(input)) {
        count = std::stoull(input);
        if(count >= 0 && count <= 18446744073709551615_ui64)
          break;
      }
      std::clog << "Invalid input!";
    }

    for(ui64 i = 0; i < count; ++i) {
      clearConsole();
      std::clog << array << '\n';


      VarType type;
      std::clog << "Variable types:\n" << VAR_TYPES
                << "Enter add type: ";
      std::cin >> type;
      array.pushBack(inputVariable(type));
    }
    return array;
  }

  case binom::VarType::object: {
    Object object;
    ui64 count = 0;
    while (true) {
      std::clog << "Enter element count: ";
      std::cin >> input;
      if(isUnsignedInt(input)) {
        count = std::stoull(input);
        if(count >= 0 && count <= 18446744073709551615_ui64)
          break;
      }
      std::clog << "Invalid input!";
    }

    for(ui64 i = 0; i < count; ++i) {
      clearConsole();
      std::clog << object << '\n';

      ValType name_type;
      std::clog << "Value types:\n" << VAL_TYPES
                << "Enter name values type: ";
      std::cin >> name_type;

      BufferArray name = inputVariable(toVarBufferType(name_type)).toBufferArray();

      VarType type;
      std::clog << "Variable types:\n" << VAR_TYPES
                << "Enter variable type: ";
      std::cin >> type;

      object.insert(name, inputVariable(type));
    }
    return object;
  }
  }

}




void editValue(UnionNodeVisitor root_node) {
  enum class Command {
    exit = 0,
    add = 1,
    change = 2,
    remove = 3,
    print_node_index = 4,
  };

  clearConsole();
  while (true) {
    ui16 command;
    std::clog << root_node.getVariable() << "\n\n"
    "Commands:\n"
    "| 0 - exit\n"
    "| 1 - add variable\n"
    "| 2 - change variable\n"
    "| 3 - remove variable\n"
    << ((root_node.getVisitorType() == VisitorType::DB)? "| 4 - print node index\n" : "") <<
    "Enter command: ";
    std::cin >> command;

    if(command > ((root_node.getVisitorType() == VisitorType::DB)? 4 : 3)) continue;


    switch (static_cast<Command>(command)) {
    case Command::exit: return;


    case Command::add: {
      clearConsole();
      UnionNodeVisitor node = root_node.isEmpty()? root_node : inputPath(root_node);
      clearConsole();

      std::clog << "Selected: " << node.getVariable() << "\n\n";
      switch (node.getTypeClass()) {

      case binom::VarTypeClass::primitive: std::cerr << "Variable is primitive\n"; continue;

      case binom::VarTypeClass::buffer_array: {

        int as = 0;
        int to = 0;
        ui64 index = 0;
        while (true) {
          std::clog << "Input as:\n"
                       "| 1 - primitive\n"
                       "| 2 - buffer array\n"
                       ": ";
          as = inputInt();
          if(as == 1 || as == 2) break;
        }

        ValType type;
        std::clog << "Value types:\n" << VAL_TYPES
                  << "Enter add value type: ";
        std::cin >> type;

        if(!node.isEmpty()) {
          while (true) {
            std::clog << "Input to:\n"
                         "| 1 - front\n"
                         "| 2 - by index\n"
                         "| 3 - back\n"
                         ": ";
            to = inputInt();
            if(to >= 1 && to <= 3) break;
          }
          if(to == 2) {
            std::clog << "Enter index: ";
            std::cin >> index;
          }
        } else to = 3;

        if(as == 1) {
          switch (to) {
          case 1: node.pushFront(inputVariable(toVarType(type))); clearConsole(); continue;
          case 2: node.insert(index, inputVariable(toVarType(type))); clearConsole(); continue;
          case 3: node.pushBack(inputVariable(toVarType(type))); clearConsole(); continue;
          }
        } elif(as == 2) {
          switch (to) {
          case 1: node.pushFront(inputVariable(toVarBufferType(type))); clearConsole(); continue;
          case 2: node.insert(index, inputVariable(toVarBufferType(type))); clearConsole(); continue;
          case 3: node.pushBack(inputVariable(toVarBufferType(type))); clearConsole(); continue;
          }
        }

      } continue;


      case binom::VarTypeClass::array: {
        int to = 0;
        ui64 index = 0;
        VarType type;

        std::clog << "Variable types:\n" << VAR_TYPES
                  << "Enter add type: ";
        std::cin >> type;

        if(!node.isEmpty()) {
          while (true) {
            std::clog << "Input to:\n"
                         "| 1 - front\n"
                         "| 2 - by index\n"
                         "| 3 - back\n"
                         ": ";
            to = inputInt();
            if(to >= 1 && to <= 3) break;
          }
        } else to = 3;

        switch (to) {
        case 1: node.pushFront(inputVariable(type)); clearConsole(); continue;
        case 2:
          std::clog << "Enter index: ";
          std::cin >> index;
          node.insert(index, inputVariable(type));
          clearConsole();
        continue;
        case 3: node.pushBack(inputVariable(type)); clearConsole(); continue;
        }

      } continue;


      case binom::VarTypeClass::object:{
        ValType name_type;
        std::clog << "Value types:\n" << VAL_TYPES
                  << "Enter name values type: ";
        std::cin >> name_type;

        BufferArray name = inputVariable(toVarBufferType(name_type)).toBufferArray();

        VarType type;
        std::clog << "Variable types:\n" << VAR_TYPES
                  << "Enter variable type: ";
        std::cin >> type;

        node.insert(name, inputVariable(type));
        clearConsole();
      } continue;


      case binom::VarTypeClass::invalid_type: std::cerr << "Invalid type\n"; continue;
      }


    } break;

    case Command::change: {
      clearConsole();
      UnionNodeVisitor node = root_node.isEmpty()? root_node : inputPath(root_node);
      clearConsole();
      VarType type;
      std::clog << "Variable types:\n" << VAR_TYPES
                << "Enter variable type: ";
      std::cin >> type;
      node.setVariable(inputVariable(type));
      clearConsole();
    } continue;

    case Command::remove:
      if(root_node.isEmpty()) {
        std::clog << "Root is empty!";
        continue;
      }
      while (true) {
        std::string str;
        std::cout << root_node.getVariable() << "\n\n"
        "Enter path: ";
        std::cin >> str;
        try {
          root_node.remove(Path::fromString(str));
          break;
        } catch (const Exception& except) {
          std::cerr << except.full() << '\n';
          continue;
        }
        clearConsole();
      }
    continue;

    case Command::print_node_index:
      clearConsole();
      DBNodeVisitor node = inputPath(root_node).toDBVisitor();
      std::clog << "Node index: " << node.getNodeIndex() << "\n\n";
    continue;

    }


  }
}
