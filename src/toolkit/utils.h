#ifndef UTILS_H
#define UTILS_H

#include "../binom/includes/binom.h"

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

void clearConsole();
void editValue(binom::UnionNodeVisitor root_node);

#endif // UTILS_H
