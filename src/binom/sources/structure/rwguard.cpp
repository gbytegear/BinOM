#include "binom/includes/structure/rwguard.h"

using namespace binom;


RWSyncMap::RWGuardAutoDelete::RWGuardAutoDelete(RWSyncMap* map)
  : map(map) {}
