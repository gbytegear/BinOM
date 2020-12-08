#include "binom/includes/structure/variables/variable.h"

using namespace binom;


void Array::destroy() {
  for(Variable* it = reinterpret_cast<Variable*>(data.bytes + 9),
      *const end = reinterpret_cast<Variable*>(data.bytes + 9 + length()*PTR_SZ);
      it != end;
      ++it) {
//    it->destroy();
  }
}
