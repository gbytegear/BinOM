#ifndef STDOUTPUT_HXX
#define STDOUTPUT_HXX

#include "libbinom/include/variables/variable.hxx"
#include "libbinom/include/variables/number.hxx"
#include "libbinom/include/variables/bit_array.hxx"
#include "libbinom/include/variables/buffer_array.hxx"
#include "libbinom/include/variables/array.hxx"
#include "libbinom/include/variables/list.hxx"
#include "libbinom/include/variables/map.hxx"
#include "libbinom/include/variables/multi_map.hxx"
#include "libbinom/include/variables/table.hxx"
#include <iostream>

namespace binom {

struct OutCFG {
  static inline bool type_info = true;
  static inline bool row_tail = false;
};

}

std::ostream& operator << (std::ostream&, binom::Variable);
std::ostream& operator << (std::ostream&, binom::KeyValue);
std::ostream& operator << (std::ostream&, binom::Number);
std::ostream& operator << (std::ostream&, binom::BitArray);
std::ostream& operator << (std::ostream&, binom::BufferArray);
std::ostream& operator << (std::ostream&, binom::Array);
std::ostream& operator << (std::ostream&, binom::List);
std::ostream& operator << (std::ostream&, binom::Map);
std::ostream& operator << (std::ostream&, binom::MultiMap);
std::ostream& operator << (std::ostream&, binom::Table);







#endif // STDOUTPUT_HXX
