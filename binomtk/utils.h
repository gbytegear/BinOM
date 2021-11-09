#ifndef UTILS_H
#define UTILS_H

#include "../libbinom/include/binom.h"

static const char VAR_TYPES[] =
    "| 1 - byte\n"
    "| 2 - word\n"
    "| 3 - dword\n"
    "| 4 - qword\n"
    "| 5 - byte array\n"
    "| 6 - word array\n"
    "| 7 - dword array\n"
    "| 8 - qword array\n"
    "| 9 - array\n"
    "| 10 - object\n";

static const char VAL_TYPES[] =
    "| 1 - byte\n"
    "| 2 - word\n"
    "| 3 - dword\n"
    "| 4 - qword\n";;

void clearConsole();

bool isSigned(std::string str);
bool isUnsigned(std::string str);

binom::i64 inputSigned(const char* msg, std::function<bool(binom::i64)> check_callbak = [](binom::i64)->bool{return true;});
binom::ui64 inputUnsigned(const char* msg, std::function<bool(binom::ui64)> check_callbak = [](binom::ui64)->bool{return true;});
bool inputBool(const char* msg);

#endif // UTILS_H
