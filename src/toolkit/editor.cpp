#include "cli.h"
#include "binom/includes/lexer.h"

#include <limits>

//#define ED_DEBUG
#ifdef ED_DEBUG
#define dbg_out(out) std::clog << out
#else
#define dbg_out(out)
#endif

using namespace binom;

typedef std::unique_ptr<NodeVisitorBase> UnionNodeVisitor;

struct AddPosisition {
  enum class PositionType : int {
    front = 0,
    by_index = 1,
    back = 2
  };
  PositionType type;
  ui64 index;
  AddPosisition(ui64 element_count)
    : type(PositionType(inputUnsigned(
             "Enter add type:\n"
             "| 0 - add to front\n"
             "| 1 - add by index\n"
             "| 2 - add to back\n"
             ": ",[](ui64 value)->bool { return value <= 2; }))) {
    if(type == PositionType::by_index)
      index = inputUnsigned("Enter index: ", [&element_count](ui64 value)->bool {
              if(value > element_count) {
                std::cerr << "Index highter then container element count\n"
                             "index: " << value <<
                             "\nelement_count: " << element_count << '\n';
                return false;
              }
              return true;
      });
  }
  AddPosisition(AddPosisition& other) : type(other.type), index(other.index) {}
  AddPosisition(AddPosisition&& other) : type(other.type), index(other.index) {}
};

Path inputPath() {
  while (true) try {
    std::string input;
    std::clog << "Enter path: ";
//    std::cin.clear();
//    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, input, '\n');
    return Path::fromString(input);
  }  catch (Exception& except) {
    std::cerr << except.full() << '\n';
    continue;
  }
}

Variable inputeVariable(const char* msg) {
  while(true)
    try {
      std::string input;
      std::clog << msg;
//      std::cin.clear();
//      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::getline(std::cin, input, '\n');
      return lexer << input;
    } catch(Exception& except) {
      std::cerr << except.full() << '\n';
      continue;
    }
}

UnionNodeVisitor getByPath(UnionNodeVisitor& root, Path path) {
  switch (root->getVisitorType()) {
    case binom::VisitorType::ram_storage_visitor: return UnionNodeVisitor(new NodeVisitor(root->toRAMVisitor()[path]));
    case binom::VisitorType::file_storage_visitor: return UnionNodeVisitor(new FileNodeVisitor(root->toFileVisitor()[path]));
  }
}

void editor(std::unique_ptr<NodeVisitorBase> root) {
  enum Command : int {
    exit   = 0,
    change = 1,
    add    = 2,
    remove = 3,
  };

  while(true) try {
    clearConsole();
    std::clog << "File content:\n"
    << root->getVariableClone() << '\n';

    Command command = Command(inputUnsigned("Enter command:\n"
    "| 1 - change node\n"
    "| 2 - add node\n"
    "| 3 - remove node\n"
    "| 0 - exit\n"
    ": ", [](ui64 command_num){
        if(command_num > int(Command::remove)) {
          std::cerr << "Invalid command number\n";
          return false;
        }
        return true;
    }));



    switch (command) {
      case exit: return;

      case change: {
        UnionNodeVisitor node;
        while(true) try {
          node = getByPath(root, inputPath());
          break;
        } catch(Exception& except) {
          std::cerr << except.full() << '\n';
          std::clog << "Node struct: " << root->getVariableClone() << '\n';
          continue;
        }

        while(true) try {
          Variable value = inputeVariable("Enter value: ");
          dbg_out("Entered value: " << value);
          node->setVariable(value);
          break;
        } catch(Exception& except) {
          std::cerr << except.full() << '\n';
          continue;
        }

      }break;

      case add:{
        UnionNodeVisitor node;
        while(true) try {
          node = getByPath(root, inputPath());
          if(node->isPrimitive()) {
            std::cerr << "No value can be added to a primitive variable\n";
            continue;
          }
          break;
        } catch(Exception& except) {
          std::cerr << except.full() << '\n';
          std::clog << "Node struct: " << root->getVariableClone() << '\n';
          continue;
        }

        while(true) try {
          if(node->isObject()) {
            Variable name = inputeVariable("Enter name: ");
            dbg_out("Entered name: " << name);
            if(!name.isBufferArray()) {
              std::cerr << "Expected buffer array as name\n";
              continue;
            }
            Variable value = inputeVariable("Enter value: ");
            dbg_out("Entered value: " << value);
            node->insert(name.toBufferArray(), value);
          } elif(node->isValueRef()) {
            Variable value = inputeVariable("Enter primitive value: ");
            dbg_out("Entered value: " << value);
            if(!value.isPrimitive()) {
              std::cerr << "Expected primitive as value of buffer array\n";
              continue;
            }
            if(!node->getElementCount()) {
              node->pushBack(value);
            } else {
              AddPosisition pos(node->getElementCount());
              switch (pos.type) {
                case AddPosisition::PositionType::front:
                  node->pushFront(value);
                break;
                case AddPosisition::PositionType::by_index:
                  node->insert(pos.index, value);
                break;
                case AddPosisition::PositionType::back:
                  node->pushBack(value);
                break;
              }
            }
          } else {
            Variable value = inputeVariable("Enter value: ");
            dbg_out("Entered value: " << value);
            if(!node->getElementCount()) {
              node->pushBack(value);
            } else {
              AddPosisition pos(node->getElementCount());
              switch (pos.type) {
                case AddPosisition::PositionType::front:
                  node->pushFront(value);
                break;
                case AddPosisition::PositionType::by_index:
                  node->insert(pos.index, value);
                break;
                case AddPosisition::PositionType::back:
                  node->pushBack(value);
                break;
              }
            }
          }
          break;
        } catch(Exception& except) {
          std::cerr << except.full() << '\n';
          continue;
        }

      }break;

      case remove:
        while(true) try {
          root->remove(inputPath());
          break;
        } catch(Exception& except) {
          std::cerr << except.full() << '\n';
          continue;
        }
      break;

    }

  } catch(Exception& except) {
    std::cerr << except.full() << '\n';
    continue;
  }

}
