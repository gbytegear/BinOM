#ifndef LEXER_H
#define LEXER_H

#include "variables/variable.h"
#include "utils/file.h"

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
      login: "admin"
      password: "admin"
      access_lvl: 0xFF_ui8
    }

    obj{
      login: "user"
      password: "user"
      access_lvl: 0x7F_ui8
    }

    obj{
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


extern inline class Lexer {

  enum class ContainerType : ui8 {
    unsigned_byte_array,    // ui8[...] or ui8{...} (by default for i8)
    signed_byte_array,      // si8[...] or si8{...}
    unsigned_word_array,    // ui16[...] or ui16{...} (by default for i16)
    signed_word_array,      // si16[...] or si16{...}
    unsigned_dword_array,   // ui32[...] or ui32{...} (by default for i32)
    signed_dword_array,     // si32[...] or si32{...}
    float_dword_array,      // f32[...] or f32{...}
    unsigned_qword_array,   // ui64[...] or ui64{...} (by default for i64)
    signed_qword_array,     // si64[...] or si64{...}
    float_qword_array,      // f64[...] or f64{...}
    heterogeneous_container // Array or Object
  };

  enum class NameLiteralType : ui8 {
    string,                   // string: or "string":
    unsigned_byte_array,      // ui8[1,2,3]:
    signed_byte_array,        // si8[1,2,3]:
    unsigned_word_array,      // ui16[1,2,3]:
    signed_word_array,        // si16[1,2,3]:
    unsigned_dword_array,     // ui32[1,2,3]:
    signed_dword_array,       // si32[1,2,3]:
    float_dword_array,        // f32[1,2,3]:
    unsigned_qword_array,     // ui64[1,2,3]:
    signed_qword_array,       // si64[1,2,3]:
    float_qword_array,        // f64[1,2,3]:
  };

  enum class LiteralType : ui8 {
    number,           // 5
    signed_number,    // -5
    float_number,     // 5.1
    unsigned_byte,    // 5_ui8
    signed_byte,      // 5_si8
    unsigned_word,    // 5_ui16
    signed_word,      // 5_si16
    unsigned_dword,   // 5_ui32
    signed_dword,     // 5_si32
    float_dword,      // 5.1_f32
    unsigned_qword,   // 5_ui64
    signed_qword,     // 5_si64
    float_qword,      // 5.1_f64
  };

  enum class ValueRecordType {
    decimal,      // 5
    hexadecimal,  // 0x05
    octal         // 05
  };

  LiteralType clarifyLiteralType(ContainerType container_type, LiteralType currtent_type);

  struct ParseContext {
    const std::string& text;
    ui64 index;
    bool isNotEnd() {return index != text.length();}
    inline char operator*() const {return text[index];}
    inline ui64 operator++() {return isNotEnd()? ++index : index;}
    inline ui64 operator++(int) {return isNotEnd()? index++ : index;}
    inline ui64 operator--() {return index? --index : index;}
    inline ui64 operator--(int) {return index? index-- : index;}
    inline char operator+(i64 shift) {return text[index + shift];}
    inline char operator-(i64 shift) {return text[index - shift];}
    std::string getSubStr(ui64 n) {return isNotEnd()? text.substr((index += n)-n, n) : "";}
  };

  void skipWhiteSpace(ParseContext& ctx);
  void skipSeparator(ParseContext& ctx);

  bool isOpenParenthesis(ParseContext& ctx);
  bool isCloseParenthesis(ParseContext& ctx);

  BufferArray parseName(ParseContext& ctx);
  Primitive parsePrimitive(ParseContext& ctx, ContainerType container_type);
  Variable parseContainer(ParseContext& ctx);
  Variable parseLiteral(ParseContext& ctx);


public:

  Variable operator << (std::string text);
  Variable parse(std::string text) {return *this << std::move(text);}
  Variable fromFile(std::string_view url) {
    FileIO file(url);
    if(!file.isExist()) return nullptr;
    return *this << file.read(0, file.getSize()).toStdString();
  }

} lexer;

}

#endif // LEXER_H
