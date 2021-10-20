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

bool isSigned(std::string str) {
  std::string::const_iterator it = str.begin();
  if(*it == '-') ++it;
  while (it != str.end() && std::isdigit(*it)) ++it;
  return !str.empty() && it == str.end();
}

bool isUnsigned(std::string str) {
  std::string::const_iterator it = str.begin();
  while (it != str.end() && std::isdigit(*it)) ++it;
  return !str.empty() && it == str.end();
}


binom::i64 inputSigned(const char* msg, std::function<bool (binom::i64)> check_callbak) {
  std::string input;
  while(true) {
    std::clog << msg;
    std::getline(std::cin, input, '\n');
    if(!isSigned(input)) {
      std::clog << "Entered isn't number value!\n";
      continue;
    }

    binom::i64 number = std::stoll(input);

    if(!check_callbak(number)) continue;

    return number;
  }
}

binom::ui64 inputUnsigned(const char* msg, std::function<bool (binom::ui64)> check_callbak) {
  std::string input;
  while(true) {
    std::clog << msg;
    std::getline(std::cin, input, '\n');
    if(!isUnsigned(input)) {
      std::clog << "Entered isn't unsigned number value!\n";
      continue;
    }

    binom::ui64 number = std::stoull(input);

    if(!check_callbak(number)) continue;

    return number;
  }
}

bool inputBool(const char* msg) {
  std::string input;
  while (true) {
    std::clog << msg;
    std::getline(std::cin, input, '\n');

    if(input == "y") return true;
    elif(input == "n") return false;
    elif(input == "t") return true;
    elif(input == "f") return false;
    elif(input == "yes") return true;
    elif(input == "no") return false;
    elif(input == "true") return true;
    elif(input == "false") return false;
    else continue;
  }
}
