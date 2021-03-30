#ifndef BINOM_H
#define BINOM_H

#include "structure/variables/node_visitor.h"
#include "structure/file_storage/data_base_container.h"

namespace binom {

ui64 saveToFile(std::string filename, Variable var);
Variable loadFromFile(std::string filename);

}

#endif // BINOM_H
