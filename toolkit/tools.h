#ifndef TOOLS_H
#define TOOLS_H

#include <map>
#include <functional>
#include <string>
#include "utils.h"

typedef std::function<binom::Variable(binom::Variable)> tool_function_t;

extern std::map<std::string, std::function<binom::Variable(binom::Variable)>> tool_map;

int execute(binom::Variable args);

#endif // TOOLS_H
