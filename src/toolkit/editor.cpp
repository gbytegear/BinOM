#include "cli.h"

using namespace binom;

typedef std::unique_ptr<NodeVisitorBase> UnionNodeVisitor;

void changeVariable(NodeVisitorBase* node);

void changePrimitive(NodeVisitorBase* node) {
  enum InputType : int {
    sym     = 1,
    sign    = 2,
    unsign  = 3
  };

  InputType input_type;
  std::string input;
  ValType value_type = toValueType(node->getType());
  switch (value_type) {
    case binom::ValType::byte:
      input_type = InputType(inputUnsigned("Enter byte input type:\n"
        "| 1 - symbol\n"
        "| 2 - signed number (-128...127)\n"
        "| 3 - unsigned number (0...255)\n"
        ":",
                                           [](ui64 number){
                               if(number < int(sym) || number > int(unsign)) {
                                 std::cerr << "Invalid value!";
                                 return false;
                               }
                               return true;
                             }));

      switch (input_type) {
        case sym:{
          char symbol;
          std::clog << "Input value(symbol): ";
          std::cin >> symbol;
          node->setVariable(symbol);
        }return;
        default:
        case sign:
          node->setVariable(i8(inputSigned("Input value(-128...127): ", [](i64 number) {return number >= -128 && number <= 127;})));
        return;
        case unsign:
          node->setVariable(ui8(inputUnsigned("Input value(0...255): ", [](ui64 number) {return number <= 255;})));
        return;
      }

    break;
    case binom::ValType::word:
      input_type = InputType(inputUnsigned("Enter byte input type:\n"
        "| 1 - signed number (-32768...32767)\n"
        "| 2 - unsigned number (0...65535)\n"
        ":",
                                           [](ui64 number){
                               if(number < int(sign) + 1 || number > int(unsign) + 1) {
                                 std::cerr << "Invalid value!";
                                 return false;
                               }
                               return true;
                             }) - 1);

      switch (input_type) {
        default:
        case sign:
          node->setVariable(i16(inputSigned("Input value(-32768...32767): ", [](i64 number) {return number >= -32768 && number <= 32767;})));
        return;
        case unsign:
          node->setVariable(ui16(inputUnsigned("Input value(0...65535): ", [](ui64 number) {return number <= 65535;})));
        return;
      }

    break;
    case binom::ValType::dword:
      input_type = InputType(inputUnsigned("Enter byte input type:\n"
        "| 1 - signed number (-2147483648...2147483647)\n"
        "| 2 - unsigned number (0...4294967295)\n"
        ":",
                                           [](ui64 number){
                               if(number < int(sign) + 1 || number > int(unsign) + 1) {
                                 std::cerr << "Invalid value!";
                                 return false;
                               }
                               return true;
                             }) - 1);

      switch (input_type) {
        default:
        case sign:
          node->setVariable(i32(inputSigned("Input value(-32768...32767): ", [](i64 number) {return number >= -32768 && number <= 32767;})));
        return;
        case unsign:
          node->setVariable(ui32(inputUnsigned("Input value(0...65535): ", [](ui64 number) {return number <= 65535;})));
        return;
      }

    break;
    case binom::ValType::qword:
      input_type = InputType(inputUnsigned("Enter byte input type:\n"
        "| 1 - signed number (-9223372036854775808...9223372036854775807)\n"
        "| 2 - unsigned number (0...18446744073709551615)\n"
        ":",
                                           [](ui64 number){
                               if(number < int(sign) + 1 || number > int(unsign) + 1) {
                                 std::cerr << "Invalid value!";
                                 return false;
                               }
                               return true;
                             }) - 1);

      switch (input_type) {
        default:
        case sign:
          node->setVariable(i64(inputSigned("Input value(-9223372036854775808...9223372036854775807): ", [](i64 number) {return number >= -9223372036854775808_i64 && number <= 9223372036854775807_i64;})));
        return;
        case unsign:
          node->setVariable(ui64(inputUnsigned("Input value(0...18446744073709551615): ", [](ui64 number) {return number <= 18446744073709551615_ui64;})));
        return;
      }

    break;
    case binom::ValType::invalid_type: throw Exception(ErrCode::binom_invalid_type);
  }



}


enum class InsertMode : int {
  back = 1,
  front = 2,
  by_index = 3
};

InsertMode inputInsertMode() {
  return InsertMode(inputUnsigned("Enter insert mode\n"
    "| 1 - push element back\n"
    "| 2 - push element fron\n"
    "| 3 - insert by index\n"
    ":", [](ui64 number) {
      if(number < int(InsertMode::back) || number > int(InsertMode::by_index)) {
        std::cerr << "Invalid input\n";
        return false;
      }
      return true;
    }));
}

void changeBufferArray(NodeVisitorBase* node) {
  if(node->getType() == VarType::byte_array && node->getElementCount() == 0)
    if(inputBool("Input as string? (y/n):")) {
      std::string value;
      std::clog << "Enter value: ";
      std::cin >> value;
      node->setVariable(value);
      return;
    }

  VarType type = node->getType();
  Primitive input(type);
  NodeVisitor input_node = input.asVar();


  while (true) {
    std::clog << "Node: " << node->getVariableClone() << '\n';
    if(!inputBool("Add element in buffer array? (y/n):"))
      break;
    switch (inputInsertMode()) {
      case InsertMode::back:
        changePrimitive(&input_node);
        node->pushBack(input);
      break;


      case InsertMode::front:
        changePrimitive(&input_node);
        node->pushFront(input);
      break;


      case InsertMode::by_index: {
        ui64 index = inputUnsigned("Input index: ", [&node](ui64 index) {
            ui64 element_count = node->getElementCount();
            if(index > element_count) {
              std::clog << "Insert index highter then element count (" << element_count << ")\n";
              return false;
            }
            return true;
        });
        changePrimitive(&input_node);
        node->insert(index, input);
      } break;
    }
  }
}

void changeArray(NodeVisitorBase* node) {
  while (true) {
    std::clog << "Node: " << node->getVariableClone() << '\n';
    if(!inputBool("Add element in array? (y/n):"))
      break;

    switch (inputInsertMode()) {
      case InsertMode::back: {
        VarType type;
        std::clog << "Enter element type:\n"
        "| 1 - byte\n"
        "| 2 - word\n"
        "| 3 - dword\n"
        "| 4 - qword\n"
        "| 5 - byte array\n"
        "| 6 - word array\n"
        "| 7 - dword array\n"
        "| 8 - qword array\n"
        "| 9 - array\n"
        "| 10 - object\n"
        ":";
        std::cin >> type;
        Variable element = type;
        NodeVisitor input_node = element;
        changeVariable(&input_node);
        node->pushBack(std::move(element));
      }break;


      case InsertMode::front: {
        VarType type;
        std::clog << "Enter element type:\n"
        "| 1 - byte\n"
        "| 2 - word\n"
        "| 3 - dword\n"
        "| 4 - qword\n"
        "| 5 - byte array\n"
        "| 6 - word array\n"
        "| 7 - dword array\n"
        "| 8 - qword array\n"
        "| 9 - array\n"
        "| 10 - object\n"
        ":";
        std::cin >> type;
        Variable element = type;
        NodeVisitor input_node = element;
        changeVariable(&input_node);
        node->pushFront(std::move(element));
      }break;


      case InsertMode::by_index: {
        ui64 index = inputUnsigned("Input index: ", [&node](ui64 index) {
            ui64 element_count = node->getElementCount();
            if(index > element_count) {
              std::clog << "Insert index highter then element count (" << element_count << ")\n";
              return false;
            }
            return true;
        });
        VarType type;
        std::clog << "Enter element type:\n"
        "| 1 - byte\n"
        "| 2 - word\n"
        "| 3 - dword\n"
        "| 4 - qword\n"
        "| 5 - byte array\n"
        "| 6 - word array\n"
        "| 7 - dword array\n"
        "| 8 - qword array\n"
        "| 9 - array\n"
        "| 10 - object\n"
        ":";
        std::cin >> type;
        Variable element = type;
        NodeVisitor input_node = element;
        changeVariable(&input_node);
        node->insert(index, std::move(element));
      }break;


    }
  }
}

void changeObject(NodeVisitorBase* node) {
  while (true) {
    std::clog << "Node: " << node->getVariableClone() << '\n';
    if(!inputBool("Add element in object? (y/n):"))
      break;

    ValType name_type;
    std::clog << "Enter name type:\n"
    "| 1 - byte\n"
    "| 2 - word\n"
    "| 3 - dword\n"
    "| 4 - qword\n"
    ":";
    std::cin >> name_type;
    BufferArray name(name_type);
    NodeVisitor input_node = name.asVar();
    std::clog << "Enter name:\n";
    changeVariable(&input_node);

    VarType type;
    std::clog << "Enter element type:\n"
    "| 1 - byte\n"
    "| 2 - word\n"
    "| 3 - dword\n"
    "| 4 - qword\n"
    "| 5 - byte array\n"
    "| 6 - word array\n"
    "| 7 - dword array\n"
    "| 8 - qword array\n"
    "| 9 - array\n"
    "| 10 - object\n"
    ":";
    std::cin >> type;
    Variable element = type;
    input_node = element;
    changeVariable(&input_node);

    node->insert(std::move(name), std::move(element));

  }
}


void changeVariable(NodeVisitorBase* node) {
  switch (node->getTypeClass()) {
    case binom::VarTypeClass::primitive: return changePrimitive(node);
    case binom::VarTypeClass::buffer_array: return changeBufferArray(node);
    case binom::VarTypeClass::array: return changeArray(node);
    case binom::VarTypeClass::object: return changeObject(node);
    case binom::VarTypeClass::invalid_type: throw Exception(ErrCode::binom_invalid_type);
  }
}

void editor(std::unique_ptr<NodeVisitorBase> root, bool edit_root) {
  enum Command : int {
    exit   = 0,
    change = 1,
    set    = 2,
    remove = 3,
  };

  if(edit_root) {
    edit_root = false;
    changeVariable(&*root);
  }

  while(true) try {
    std::clog << "File content:\n"
    << root->getVariableClone();

    Command command = Command(inputUnsigned("Enter command:\n"
    "| 1 - change node\n"
    "| 2 - set node\n"
    "| 3 - remove node\n"
    "| 0 - exit\n"
    ": ", [](ui64 command_num){
        if(command_num > int(Command::remove)) {
          std::cerr << "Invalid command number\n";
          return false;
        }
        return true;
    }));

    Path path;

    while (true) try {
      std::string input;
      std::clog << "Enter path: ";
      std::cin >> input;
      path = Path::fromString(input);
      break;
    }  catch (Exception& except) {
      continue;
    }

    switch (command) {
      case exit: return;

      case change: {
        UnionNodeVisitor node = (root->getVisitorType() == VisitorType::file_storage_visitor)
            ? UnionNodeVisitor(new FileNodeVisitor(root->toFileVisitor()))
            : UnionNodeVisitor(new NodeVisitor(root->toRAMVisitor()));
        node->stepInside(path);
        changeVariable(&*node);
      }break;

      case set:{
        UnionNodeVisitor node = (root->getVisitorType() == VisitorType::file_storage_visitor)
            ? UnionNodeVisitor(new FileNodeVisitor(root->toFileVisitor()))
            : UnionNodeVisitor(new NodeVisitor(root->toRAMVisitor()));
        node->stepInside(path);
        VarType type;
        std::clog << "Enter node type:\n"
        "| 1 - byte\n"
        "| 2 - word\n"
        "| 3 - dword\n"
        "| 4 - qword\n"
        "| 5 - byte array\n"
        "| 6 - word array\n"
        "| 7 - dword array\n"
        "| 8 - qword array\n"
        "| 9 - array\n"
        "| 10 - object\n"
        ":";
        std::cin >> type;
        Variable element = type;
        NodeVisitor input_node = element;
        changeVariable(&input_node);
        node->setVariable(element);
      }break;

      case remove:
        root->remove(path);
      break;

    }

  } catch(Exception& except) {
    std::cerr << except.full() << '\n';
    continue;
  }

}
