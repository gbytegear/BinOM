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

bool isNumber(std::string str) {return !str.empty() && std::find_if(str.begin(), str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();}

