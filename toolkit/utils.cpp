#include "utils.h"
#include "cli.h"

#ifdef _WIN32
#include <conio.h>
void clearConsole() {clrscr();}
#else // POSIX
#include <iostream>
void clearConsole() {std::clog << "\e[1;1H\e[2J";}
#endif

using namespace binom;

UnionNodeVisitor inputPath(UnionNodeVisitor& root_node) {
  while (true) {
    std::string str;
    std::cout << root_node.getVariable() << "\n\n"
    "Enter path: ";
    std::cin >> str;
    try {
      return root_node[Path::fromString(str)];
    }  catch (const Exception& except) {
      std::cerr << except.full() << '\n';
      continue;
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

  while (true) {
    ui16 command;
    clearConsole();
    std::clog << root_node.getVariable() << "\n\n"
    "Commands:\n"
    "| 0 - exit\n"
    "| 1 - add variable\n"
    "| 2 - change variable\n"
    "| 3 - remove variable\n"
    << ((root_node.getVisitorType() == VisitorType::DB)? "| 4 - print node index\n" : "") <<
    "Enter command: ";
    std::cin >> command;
    if(command >= ((root_node.getVisitorType() == VisitorType::DB)? 4 : 3)) continue;


    switch (static_cast<Command>(command)) {
      case Command::exit: return;
      case Command::add: {
        UnionNodeVisitor node = inputPath(root_node);
        std::clog << "Selected: " << node.getVariable() << "\n\n";
        VarType type;
        std::clog << "Variable types:\n" << VAR_TYPES
                  << "Enter add type: ";
        std::cin >> type;
      } break;
      case Command::change:
      break;
      case Command::remove:
      break;
      case Command::print_node_index:
      break;

    }


  }
}
