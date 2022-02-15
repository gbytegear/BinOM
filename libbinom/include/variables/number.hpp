#ifndef NUMBER_HPP
#define NUMBER_HPP

#include "../utils/node_manager.hpp"

namespace binom {

class Number {
  using NodeIndex = priv::NodeDescriptorIndex;

  NodeIndex node_index;
};

}


#endif // NUMBER_HPP
