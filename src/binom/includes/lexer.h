#ifndef LEXER_H
#define LEXER_H

#include "variables/variable.h"

namespace binom {

/*
# BinOM human readbale representation rules:
* File extensions: *.binomhr
* Complex types branches: {}, [], <> or ()
* Name assigment operator: : or =
* Value separators: any unused character
* Value literals:
  * 5 - number (unsigned by default in array or object)
  * -5 - number (signed by default in array or object)
  * 5_ui8, 5_i8, 0x05 - unsigned byte
  * 5_si8 - signed byte
  * 5_ui16, 5_i16, 0x0005, 0x05_i16 - usingned word
  * 5_si16 - signed word
  * 5_ui32, 5_i32, 0x00000005, 0x05_i32 - usingned dword
  * 5_si32 - signed dword
  * 5_ui64, 5_i64, 0x0000000000000005, 0x05_i64 - usingned 1word
  * 5_si64 - signed qword
  * "Any string" - byte array
  * i8(...), ui8(...) - unsigned by default byte array
  * si8(...) - signed by default byte array
  * i16(...), ui16(...) - unsigned by default word array
  * si16(...) - signed by default word array
  * i32(...), ui32(...) - unsigned by default dword array
  * si32(...) - signed by default dword array
  * i64(...), ui64(...) - unsigned by default qword array
  * si64(...) - signed by default qword array
  * arr(...) - array
  * obj(...) - object

## Example:
obj{
  usr: arr[
    obj{
      id: 0_ui64
      login: "admin"
      password: "admin"
      access_lvl: 0xFF_ui8
    }

    obj{
      id: *usr[0]<last*id + 1
      login: "user"
      password: "user"
      access_lvl: 0x7F_ui8
    }

    obj{
      id: *usr[0]<last*id + 1
      login: "guest"
      password: "guest"
      access_lvl: 0x00_ui8
    }
  ]

  grp: arr[

    obj{

    }

  ]



}


*/


const class Lexer {
  enum Token {
    expected_any_expression,
    end_brach,
    // Generic literals
    integer_literal,
    float_literal,
    hex_literal,
    // Exact literals
    unsigned_byte_literal,
    signed_byte_litersal,
    unsigned_word_literal,
    signed_word_literal,
    unsigned_dword_literal,
    signed_dword_literal,
    float_dword_liteal,
    unsigned_qword_literal,
    signed_qword_literal,
    array_liteal,
    object_literal
  };

  struct ValueWaiter {

  };


public:

  Variable operator << (std::string struct_representation) {
    Variable data;

    return data;
  }

} lexer;

}

#endif // LEXER_H
