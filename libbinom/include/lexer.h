#ifndef LEXER_H
#define LEXER_H

#include "variables/variable.h"
#include "utils/file.h"

namespace binom {

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
