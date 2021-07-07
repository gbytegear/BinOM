#ifndef RWGUARD_H
#define RWGUARD_H

#include <shared_mutex>
#include <map>
#include <condition_variable>
#include "types.h"


namespace binom {


typedef ui64 f_virtual_index;

struct RWSyncMap {
  class RWGuard;
private:

  class RWGuardAutoDelete {
    RWSyncMap* map;
    std::weak_ptr<RWGuardAutoDelete> weak_ptr;
    std::shared_mutex mtx;
    std::map<f_virtual_index, std::weak_ptr<RWGuardAutoDelete>>::iterator it;

    RWGuardAutoDelete(RWSyncMap* map);

    friend class RWGuard;
  public:
    ~RWGuardAutoDelete() {map->mtx_map.erase(it);}
  };

  std::map<f_virtual_index, std::weak_ptr<RWGuardAutoDelete>> mtx_map;
public:

  class RWGuard {
    std::shared_ptr<RWGuardAutoDelete> shr_ptr;

    RWGuard(f_virtual_index v_index, RWSyncMap* map) : shr_ptr(std::make_shared<RWGuardAutoDelete>(map)) {
      shr_ptr->weak_ptr = shr_ptr;
      map->mtx_map.insert(v_index, shr_ptr); // TODO: Make this shit worck!

    }

    friend struct RWSyncMap;
  public:


  };

  RWSyncMap() = default;

  RWGuard get(f_virtual_index node_index) {

  }
};


}


#endif // RWGUARD_H
