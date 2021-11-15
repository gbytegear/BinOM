#include "libbinom/include/lexer.h"

using namespace binom;

Lexer::LiteralType Lexer::clarifyLiteralType(Lexer::ContainerType container_type, Lexer::LiteralType currtent_type) {
  switch (currtent_type) {
    case LiteralType::number:
      switch (container_type) {
        case ContainerType::unsigned_byte_array:    return LiteralType::unsigned_byte;
        case ContainerType::signed_byte_array:      return LiteralType::signed_byte;
        case ContainerType::unsigned_word_array:    return LiteralType::unsigned_word;
        case ContainerType::signed_word_array:      return LiteralType::signed_word;
        case ContainerType::unsigned_dword_array:   return LiteralType::unsigned_dword;
        case ContainerType::signed_dword_array:     return LiteralType::signed_dword;
        case ContainerType::float_dword_array:      return LiteralType::float_dword;
        case ContainerType::unsigned_qword_array:   return LiteralType::unsigned_qword;
        case ContainerType::signed_qword_array:     return LiteralType::signed_qword;
        case ContainerType::float_qword_array:      return LiteralType::float_qword;
        case ContainerType::heterogeneous_container:return LiteralType::unsigned_dword;
      }
    break;
    case LiteralType::signed_number:
      switch (container_type) {
        case ContainerType::unsigned_byte_array:
        case ContainerType::signed_byte_array:      return LiteralType::signed_byte;
        case ContainerType::unsigned_word_array:
        case ContainerType::signed_word_array:      return LiteralType::signed_word;
        case ContainerType::unsigned_dword_array:
        case ContainerType::signed_dword_array:     return LiteralType::signed_dword;
        case ContainerType::float_dword_array:      return LiteralType::float_dword;
        case ContainerType::unsigned_qword_array:
        case ContainerType::signed_qword_array:     return LiteralType::signed_qword;
        case ContainerType::float_qword_array:      return LiteralType::float_qword;
        case ContainerType::heterogeneous_container:return LiteralType::signed_dword;
      }
    break;
    case LiteralType::float_number:
      switch (container_type) {
        case ContainerType::unsigned_byte_array:
        case ContainerType::signed_byte_array:      return LiteralType::signed_byte;
        case ContainerType::unsigned_word_array:
        case ContainerType::signed_word_array:      return LiteralType::signed_word;
        case ContainerType::unsigned_dword_array:
        case ContainerType::signed_dword_array:
        case ContainerType::float_dword_array:      return LiteralType::float_dword;
        case ContainerType::unsigned_qword_array:
        case ContainerType::signed_qword_array:
        case ContainerType::float_qword_array:      return LiteralType::float_qword;
        case ContainerType::heterogeneous_container:return LiteralType::float_dword;
      }
    default: return currtent_type;
  }
}

void Lexer::skipWhiteSpace(Lexer::ParseContext& ctx) {
  for(;ctx.isNotEnd();++ctx) switch (*ctx) {
    case ' ': case '\t': case '\n': case '\r': case '\x0B': case '\f': continue;
    case '/': // Skip comments
      ++ctx;
      if(*ctx == '/') for(;*ctx != '\n';++ctx);
      elif(*ctx == '*') for(;true;++ctx) {
        if(*ctx == '*') { ++ctx; if(*ctx == '/') {continue;} }
      } else --ctx;
    continue;
    default: return;
  }
}

void Lexer::skipSeparator(Lexer::ParseContext& ctx) {
  for(;ctx.isNotEnd();++ctx) switch (*ctx) {
    case ',': case ';': case '|': case '\\': // Separation symbols
    case ' ': case '\t': case '\n': case '\r': case '\x0B': case '\f': continue;
    case '/': // Skip comments
      ++ctx;
      if(*ctx == '/') for(;*ctx != '\n';++ctx);
      elif(*ctx == '*') for(;true;++ctx) {
        if(*ctx == '*') { ++ctx; if(*ctx == '/') {continue;} }
      } else --ctx;
    continue;
    default: return;
  }
}

bool Lexer::isOpenParenthesis(Lexer::ParseContext& ctx) {
  switch (*ctx) {
    case '{': case '[': case '(': case '<': ++ctx; return true;
    default: return false;
  }
}

bool Lexer::isCloseParenthesis(Lexer::ParseContext& ctx) {
  switch (*ctx) {
    case '}': case ']': case ')': case '>': ++ctx; return true;
    default: return false;
  }
}

BufferArray Lexer::parseName(Lexer::ParseContext& ctx) {
  if(*ctx == '"') {
    ++ctx;
    ui64 start = ctx.index;
    for(;ctx.isNotEnd() && *ctx != '"';++ctx);
    ++ctx;
    return BufferArray(ctx.text.substr(start, ctx.index - start - 1));
  } elif(*ctx == '\'') {
    ++ctx;
    ui64 start = ctx.index;
    for(;ctx.isNotEnd() && *ctx != '\'';++ctx);
    ++ctx;
    return BufferArray(ctx.text.substr(start, ctx.index - start - 1));
  }

  ui64 start = ctx.index;
  switch (*ctx) {
    case 'i':
      ++ctx;
      switch (*ctx) {
        case '8': {
          skipWhiteSpace(ctx);
          if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
          BufferArray data(VarType::byte_array);
          skipWhiteSpace(ctx);
          for(++ctx;ctx.isNotEnd();++ctx) {
            if(isCloseParenthesis(ctx)) break;
            data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_byte_array));
            skipSeparator(ctx);
          }
          return data;
        }
        case '1': if(ctx.getSubStr(2) == "16") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::word_array);
            skipWhiteSpace(ctx);
            for(++ctx;ctx.isNotEnd();++ctx) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_word_array));
              skipSeparator(ctx);
            }
            return data;
          } else {ctx.index = start; break;}
        case '3': if(ctx.getSubStr(2) == "32") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::dword_array);
            skipWhiteSpace(ctx);
            for(++ctx;ctx.isNotEnd();++ctx) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_dword_array));
              skipSeparator(ctx);
            }
            return data;
          } else {ctx.index = start; break;}
        case '6': if(ctx.getSubStr(2) == "64") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::qword_array);
            skipWhiteSpace(ctx);
            for(++ctx;ctx.isNotEnd();++ctx) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_qword_array));
              skipSeparator(ctx);
            }
            return data;
          } else  {ctx.index = start; break;}
        default:  {ctx.index = start; break;}
      } break;


    case 'f':
      ++ctx;
      switch (*ctx) {
        case '3': if(ctx.getSubStr(2) == "32") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::dword_array);
            skipWhiteSpace(ctx);
            for(++ctx;ctx.isNotEnd();++ctx) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::float_dword_array));
              skipSeparator(ctx);
            }
            return data;
          } else  {ctx.index = start; break;}
        case '6': if(ctx.getSubStr(2) == "64") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::qword_array);
            skipWhiteSpace(ctx);
            for(++ctx;ctx.isNotEnd();++ctx) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_qword_array));
              skipSeparator(ctx);
            }
            return data;
          } else  {ctx.index = start; break;}
        default:  {ctx.index = start; break;}
      }

    case 'u':
      ++ctx;
      if(*ctx != 'i') {ctx.index = start; break;}
      ++ctx;
      switch (*ctx) {
        case '8': {
          skipWhiteSpace(ctx);
          if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
          BufferArray data(VarType::byte_array);
          skipWhiteSpace(ctx);
          for(++ctx;ctx.isNotEnd();++ctx) {
            if(isCloseParenthesis(ctx)) break;
            data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_byte_array));
            skipSeparator(ctx);
          }
          return data;
        }
        case '1': if(ctx.getSubStr(2) == "16") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::word_array);
            skipWhiteSpace(ctx);
            for(++ctx;ctx.isNotEnd();++ctx) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_word_array));
              skipSeparator(ctx);
            }
            return data;
          } else {ctx.index = start; break;}
        case '3': if(ctx.getSubStr(2) == "32") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::dword_array);
            skipWhiteSpace(ctx);
            for(++ctx;ctx.isNotEnd();++ctx) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_dword_array));
              skipSeparator(ctx);
            }
            return data;
          } else {ctx.index = start; break;}
        case '6': if(ctx.getSubStr(2) == "64") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::qword_array);
            skipWhiteSpace(ctx);
            for(++ctx;ctx.isNotEnd();++ctx) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_qword_array));
              skipSeparator(ctx);
            }
            return data;
          } else {ctx.index = start; break;}
        default: {ctx.index = start; break;}
      } break;

    case 's':
      ++ctx;
      if(*ctx != 'i') {ctx.index = start; break;}
      ++ctx;
      switch (*ctx) {
        case '8': {
          skipWhiteSpace(ctx);
          if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
          BufferArray data(VarType::byte_array);
          skipWhiteSpace(ctx);
          for(++ctx;ctx.isNotEnd();++ctx) {
            if(isCloseParenthesis(ctx)) break;
            data.pushBack(parsePrimitive(ctx, ContainerType::signed_byte_array));
            skipSeparator(ctx);
          }
          return data;
        }
        case '1': if(ctx.getSubStr(2) == "16") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::word_array);
            skipWhiteSpace(ctx);
            for(++ctx;ctx.isNotEnd();++ctx) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::signed_word_array));
              skipSeparator(ctx);
            }
            return data;
          } else {ctx.index = start; break;}
        case '3': if(ctx.getSubStr(2) == "32") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::dword_array);
            skipWhiteSpace(ctx);
            for(++ctx;ctx.isNotEnd();++ctx) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::signed_dword_array));
              skipSeparator(ctx);
            }
            return data;
          } else {ctx.index = start; break;}
        case '6': if(ctx.getSubStr(2) == "64") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::qword_array);
            skipWhiteSpace(ctx);
            for(++ctx;ctx.isNotEnd();++ctx) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::signed_qword_array));
              skipSeparator(ctx);
            }
            return data;
          } else {ctx.index = start; break;}
        default: {ctx.index = start; break;}
      }
    default: break;
  }
  for(;ctx.isNotEnd() && *ctx != ':' && *ctx != '=';++ctx);
  return BufferArray(ctx.text.substr(start, ctx.index - start));
}

Primitive Lexer::parsePrimitive(Lexer::ParseContext& ctx, Lexer::ContainerType container_type) {
  LiteralType type = LiteralType::number;
  ValueRecordType record_type = ValueRecordType::decimal;
  if(*ctx == 't') {
    if(ctx.getSubStr(4) == "true") return ui8(1);
    else throw Exception(ErrCode::unexpected_expression,
                         "Unexpected char 't' in struct description\n"
                         "Maybe you haven't completed the 'true' literal?");
  }elif(*ctx == 'f') {
    if(ctx.getSubStr(5) == "false") return ui8(0);
    else throw Exception(ErrCode::unexpected_expression,
                         "Unexpected char 'f' in struct description\n"
                         "Maybe you haven't completed the 'false' literal?");
  }

  if(*ctx == '-') {type = LiteralType::signed_number; ++ctx;}
  if(*ctx == '0') {
    if(ctx + 1 == 'x') {record_type = ValueRecordType::hexadecimal; ++ctx; ++ctx;}
    elif(isdigit(ctx + 1)) {record_type = ValueRecordType::octal; ++ctx;}
  }

  std::string number;
  for(ui64 number_start = ctx.index;ctx.isNotEnd();++ctx) {
    switch (record_type) {
      case binom::Lexer::ValueRecordType::decimal:
        if(const char ch = *ctx; ch >= '0' && ch <= '9') continue;
        elif(ch == '.') {
          if(type != LiteralType::float_number) {type = LiteralType::float_number;}
          else throw Exception(ErrCode::unexpected_expression, "Floating literal can only contain one point");
        }
      break;
      case binom::Lexer::ValueRecordType::hexadecimal:
        if(const char ch = *ctx; (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) continue;
      break;
      case binom::Lexer::ValueRecordType::octal:
        if(const char ch = *ctx; ch >= '0' && ch <= '7') continue;
      break;
    }
    number = ctx.text.substr(number_start, ctx.index - number_start);
    break;
  }

  if(*ctx == '_') {
    ++ctx;
    switch (*ctx) {
      case 'i':
        ++ctx;
        switch (*ctx) {
          case '8': {
            switch (type) {
              default:
              case LiteralType::number: type = LiteralType::unsigned_byte; break;
              case LiteralType::signed_byte: type = LiteralType::signed_byte; break;
            }
          }break;
          case '1': if(ctx.getSubStr(2) == "16") {
              switch (type) {
                default:
                case LiteralType::number: type = LiteralType::unsigned_word; break;
                case LiteralType::signed_byte: type = LiteralType::signed_word; break;
              }
              break;
            } else throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 'i' in struct description\n"
                                     "Maybe you haven't completed the 'i16' type?");
          case '3': if(ctx.getSubStr(2) == "32") {
              switch (type) {
                default:
                case LiteralType::number: type = LiteralType::unsigned_word; break;
                case LiteralType::signed_byte: type = LiteralType::signed_word; break;
              }
              break;
            } else throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 'i' in struct description\n"
                                     "Maybe you haven't completed the 'i32' type?");
          case '6': if(ctx.getSubStr(2) == "64") {
              switch (type) {
                default:
                case LiteralType::number: type = LiteralType::unsigned_word; break;
                case LiteralType::signed_byte: type = LiteralType::signed_word; break;
              }
              break;
            } else throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 'i' in struct description\n"
                                     "Maybe you haven't completed the 'i64' type?");
          default: throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 'i' in struct description\n"
                                     "Maybe you haven't completed the 'i8', 'i16', 'i32' or 'i64' type?");
        }break;


      case 'f':
        switch (*ctx) {
          case '3': if(ctx.getSubStr(2) == "32") {
              type = LiteralType::float_dword;
              break;
            } else throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 'f' in struct description\n"
                                     "Maybe you haven't completed the 'f32' type?");
          case '6': if(ctx.getSubStr(2) == "64") {
              type = LiteralType::float_qword;
              break;
            } else throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 'f' in struct description\n"
                                     "Maybe you haven't completed the 'f64' type?");
          default: throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 'f' in struct description\n"
                                     "Maybe you haven't completed the 'f32' or 'f64' type?");
        }break;

      case 'u':
        ++ctx;
        if(*ctx != 'i')
          Exception(ErrCode::unexpected_expression,
                    "Unexpected char 'f' in struct description\n"
                      "Maybe you haven't completed the 'ui8', 'ui16', 'ui32' or 'ui64' type?");
        ++ctx;
        switch (*ctx) {
          case '8': {
            type = LiteralType::unsigned_byte;
          }break;
          case '1': if(ctx.getSubStr(2) == "16") {
              type = LiteralType::unsigned_word;
            } else throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 'u' in struct description\n"
                                     "Maybe you haven't completed the 'ui16' type?");
            break;
          case '3': if(ctx.getSubStr(2) == "32") {
              type = LiteralType::unsigned_dword;
            } else throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 'u' in struct description\n"
                                     "Maybe you haven't completed the 'ui32' type?");
            break;
          case '6': if(ctx.getSubStr(2) == "64") {
              type = LiteralType::unsigned_qword;
            } else throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 'u' in struct description\n"
                                     "Maybe you haven't completed the 'ui64' type?");
            break;
          default: throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 'u' in struct description\n"
                                     "Maybe you haven't completed the 'ui8', 'ui16', 'ui32' or 'ui64' type?");
        }break;

      case 's':
        ++ctx;
        if(*ctx != 'i')
          Exception(ErrCode::unexpected_expression,
                    "Unexpected char 's' in struct description\n"
                      "Maybe you haven't completed the 'si8', 'si16', 'si32' or 'si64' type?");
        ++ctx;
        switch (*ctx) {
          case '8': {
            type = LiteralType::signed_byte;
          }break;
          case '1': if(ctx.getSubStr(2) == "16") {
              type = LiteralType::unsigned_word;
              break;
            } else throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 's' in struct description\n"
                                     "Maybe you haven't completed the 'si16' type?");
          case '3': if(ctx.getSubStr(2) == "32") {
              type = LiteralType::unsigned_dword;
              break;
            } else throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 's' in struct description\n"
                                     "Maybe you haven't completed the 'si32' type?");
          case '6': if(ctx.getSubStr(2) == "64") {
              type = LiteralType::unsigned_qword;
              break;
            } else throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 's' in struct description\n"
                                     "Maybe you haven't completed the 'si64' type?");
          default: throw Exception(ErrCode::unexpected_expression,
                                   "Unexpected char 's' in struct description\n"
                                     "Maybe you haven't completed the 'si8', 'si16', 'si32' or 'si64' type?");
        }break;
      default: throw Exception(ErrCode::unexpected_expression);
    }
    ++ctx;
  } else type = clarifyLiteralType(container_type, type);

  switch (type) {
    case binom::Lexer::LiteralType::unsigned_byte:
      switch (record_type) {
        case binom::Lexer::ValueRecordType::decimal: return ui8(std::strtoul(number.c_str(), nullptr, 10));
        case binom::Lexer::ValueRecordType::hexadecimal: return ui8(std::strtoul(number.c_str(), nullptr, 16));
        case binom::Lexer::ValueRecordType::octal: return ui8(std::strtoul(number.c_str(), nullptr, 8));
      }
    case binom::Lexer::LiteralType::signed_byte:
      switch (record_type) {
        case binom::Lexer::ValueRecordType::decimal: return i8(std::strtol(number.c_str(), nullptr, 10));
        case binom::Lexer::ValueRecordType::hexadecimal: return i8(std::strtol(number.c_str(), nullptr, 16));
        case binom::Lexer::ValueRecordType::octal: return i8(std::strtol(number.c_str(), nullptr, 8));
      }
    case binom::Lexer::LiteralType::unsigned_word:
      switch (record_type) {
        case binom::Lexer::ValueRecordType::decimal: return ui16(std::strtoul(number.c_str(), nullptr, 10));
        case binom::Lexer::ValueRecordType::hexadecimal: return ui16(std::strtoul(number.c_str(), nullptr, 16));
        case binom::Lexer::ValueRecordType::octal: return ui16(std::strtoul(number.c_str(), nullptr, 8));
      }
    case binom::Lexer::LiteralType::signed_word:
      switch (record_type) {
        case binom::Lexer::ValueRecordType::decimal: return i16(std::strtol(number.c_str(), nullptr, 10));
        case binom::Lexer::ValueRecordType::hexadecimal: return i16(std::strtol(number.c_str(), nullptr, 16));
        case binom::Lexer::ValueRecordType::octal: return i16(std::strtol(number.c_str(), nullptr, 8));
      }
    default:
    case binom::Lexer::LiteralType::unsigned_dword:
      switch (record_type) {
        case binom::Lexer::ValueRecordType::decimal: return ui32(std::strtoul(number.c_str(), nullptr, 10));
        case binom::Lexer::ValueRecordType::hexadecimal: return ui32(std::strtoul(number.c_str(), nullptr, 16));
        case binom::Lexer::ValueRecordType::octal: return ui32(std::strtoul(number.c_str(), nullptr, 8));
      }
    case binom::Lexer::LiteralType::signed_dword:
      switch (record_type) {
        case binom::Lexer::ValueRecordType::decimal: return i32(std::strtoul(number.c_str(), nullptr, 10));
        case binom::Lexer::ValueRecordType::hexadecimal: return i32(std::strtoul(number.c_str(), nullptr, 16));
        case binom::Lexer::ValueRecordType::octal: return i32(std::strtoul(number.c_str(), nullptr, 8));
      }
    case binom::Lexer::LiteralType::float_dword:
    return f32(std::strtof(number.c_str(), nullptr));
    case binom::Lexer::LiteralType::unsigned_qword:
      switch (record_type) {
        case binom::Lexer::ValueRecordType::decimal: return ui64(std::strtoull(number.c_str(), nullptr, 10));
        case binom::Lexer::ValueRecordType::hexadecimal: return ui64(std::strtoull(number.c_str(), nullptr, 16));
        case binom::Lexer::ValueRecordType::octal: return ui64(std::strtoull(number.c_str(), nullptr, 8));
      }
    case binom::Lexer::LiteralType::signed_qword:
      switch (record_type) {
        case binom::Lexer::ValueRecordType::decimal: return i64(std::strtoll(number.c_str(), nullptr, 10));
        case binom::Lexer::ValueRecordType::hexadecimal: return i64(std::strtoll(number.c_str(), nullptr, 16));
        case binom::Lexer::ValueRecordType::octal: return i64(std::strtoll(number.c_str(), nullptr, 8));
      }
    case binom::Lexer::LiteralType::float_qword:
    return f64(std::strtof(number.c_str(), nullptr));
  }
}

Variable Lexer::parseContainer(Lexer::ParseContext& ctx) {
  switch (*ctx) {
    case 't':
      if(ctx.getSubStr(4) == "true") return ui8(1);
      else throw Exception(ErrCode::unexpected_expression,
                           "Unexpected char 't' in struct description\n"
                           "Maybe you haven't completed the 'true' literal?");
    case 'i':
      ++ctx;
      switch (*ctx) {
        case '8': {
          skipWhiteSpace(ctx);
          if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
          BufferArray data(VarType::byte_array);
          skipWhiteSpace(ctx);
          for(;ctx.isNotEnd();) {
            if(isCloseParenthesis(ctx)) break;
            data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_byte_array));
            skipSeparator(ctx);
          }
          return data;
        }
        case '1': if(ctx.getSubStr(2) == "16") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::word_array);
            skipWhiteSpace(ctx);
            for(;ctx.isNotEnd();) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_word_array));
              skipSeparator(ctx);
            }
            return data;
          } else throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 'i' in struct description\n"
                                   "Maybe you haven't completed the 'i16' type?");
        case '3': if(ctx.getSubStr(2) == "32") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::dword_array);
            skipWhiteSpace(ctx);
            for(;ctx.isNotEnd();) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_dword_array));
              skipSeparator(ctx);
            }
          } else throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 'i' in struct description\n"
                                 "Maybe you haven't completed the 'i32' type?");
        case '6': if(ctx.getSubStr(2) == "64") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::qword_array);
            skipWhiteSpace(ctx);
            for(;ctx.isNotEnd();) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_qword_array));
              skipSeparator(ctx);
            }
          } else throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 'i' in struct description\n"
                                 "Maybe you haven't completed the 'i64' type?");
        default: throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 'i' in struct description\n"
                                 "Maybe you haven't completed the 'i8', 'i16', 'i32' or 'i64' type?");
      }


    case 'f':
      ++ctx;
      switch (*ctx) {
        case 'a':
          if(ctx.getSubStr(4) == "alse") return ui8(0);
          else throw Exception(ErrCode::unexpected_expression,
                               "Unexpected char 'f' in struct description\n"
                               "Maybe you haven't completed the 'false' literal?");
        case '3': if(ctx.getSubStr(2) == "32") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::dword_array);
            skipWhiteSpace(ctx);
            for(;ctx.isNotEnd();) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::float_dword_array));
              skipSeparator(ctx);
            }
            return data;
          } else throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 'f' in struct description\n"
                                   "Maybe you haven't completed the 'f32' type?");
        case '6': if(ctx.getSubStr(2) == "64") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::qword_array);
            skipWhiteSpace(ctx);
            for(;ctx.isNotEnd();) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_qword_array));
              skipSeparator(ctx);
            }
            return data;
          } else throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 'f' in struct description\n"
                                   "Maybe you haven't completed the 'f64' type?");
        default: throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 'f' in struct description\n"
                                   "Maybe you haven't completed the 'f32' or 'f64' type?");
      }

    case 'u':
      ++ctx;
      if(*ctx != 'i')
        Exception(ErrCode::unexpected_expression,
                  "Unexpected char 'f' in struct description\n"
                    "Maybe you haven't completed the 'ui8', 'ui16', 'ui32' or 'ui64' type?");
      ++ctx;
      switch (*ctx) {
        case '8': {
          skipWhiteSpace(ctx);
          if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
          BufferArray data(VarType::byte_array);
          skipWhiteSpace(ctx);
          for(;ctx.isNotEnd();) {
            if(isCloseParenthesis(ctx)) break;
            data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_byte_array));
            skipSeparator(ctx);
          }
          return data;
        }
        case '1': if(ctx.getSubStr(2) == "16") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::word_array);
            skipWhiteSpace(ctx);
            for(;ctx.isNotEnd();) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_word_array));
              skipSeparator(ctx);
            }
            return data;
          } else throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 'u' in struct description\n"
                                   "Maybe you haven't completed the 'ui16' type?");
        case '3': if(ctx.getSubStr(2) == "32") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::dword_array);
            skipWhiteSpace(ctx);
            for(;ctx.isNotEnd();) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_dword_array));
              skipSeparator(ctx);
            }
            return data;
          } else throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 'u' in struct description\n"
                                   "Maybe you haven't completed the 'ui32' type?");
        case '6': if(ctx.getSubStr(2) == "64") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::qword_array);
            skipWhiteSpace(ctx);
            for(;ctx.isNotEnd();) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::unsigned_qword_array));
              skipSeparator(ctx);
            }
            return data;
          } else throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 'u' in struct description\n"
                                   "Maybe you haven't completed the 'ui64' type?");
        default: throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 'u' in struct description\n"
                                   "Maybe you haven't completed the 'ui8', 'ui16', 'ui32' or 'ui64' type?");
      }

    case 's':
      ++ctx;
      if(*ctx != 'i')
        Exception(ErrCode::unexpected_expression,
                  "Unexpected char 's' in struct description\n"
                    "Maybe you haven't completed the 'si8', 'si16', 'si32' or 'si64' type?");
      ++ctx;
      switch (*ctx) {
        case '8': {
          skipWhiteSpace(ctx);
          if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
          BufferArray data(VarType::byte_array);
          skipWhiteSpace(ctx);
          for(;ctx.isNotEnd();) {
            if(isCloseParenthesis(ctx)) break;
            data.pushBack(parsePrimitive(ctx, ContainerType::signed_byte_array));
            skipSeparator(ctx);
          }
          return data;
        }
        case '1': if(ctx.getSubStr(2) == "16") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::word_array);
            skipWhiteSpace(ctx);
            for(;ctx.isNotEnd();) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::signed_word_array));
              skipSeparator(ctx);
            }
            return data;
          } else throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 's' in struct description\n"
                                   "Maybe you haven't completed the 'si16' type?");
        case '3': if(ctx.getSubStr(2) == "32") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::dword_array);
            skipWhiteSpace(ctx);
            for(;ctx.isNotEnd();) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::signed_dword_array));
              skipSeparator(ctx);
            }
            return data;
          } else throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 's' in struct description\n"
                                   "Maybe you haven't completed the 'si32' type?");
        case '6': if(ctx.getSubStr(2) == "64") {
            skipWhiteSpace(ctx);
            if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
            BufferArray data(VarType::qword_array);
            skipWhiteSpace(ctx);
            for(;ctx.isNotEnd();) {
              if(isCloseParenthesis(ctx)) break;
              data.pushBack(parsePrimitive(ctx, ContainerType::signed_qword_array));
              skipSeparator(ctx);
            }
            return data;
          } else throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 's' in struct description\n"
                                   "Maybe you haven't completed the 'si64' type?");
        default: throw Exception(ErrCode::unexpected_expression,
                                 "Unexpected char 's' in struct description\n"
                                   "Maybe you haven't completed the 'si8', 'si16', 'si32' or 'si64' type?");
      }

    case 'a':
      if(ctx.getSubStr(3) != "arr")
        throw Exception(ErrCode::unexpected_expression,
                                     "Unexpected 'a' in struct description\n"
                                       "Maybe you haven't completed the 'arr' type?");
    case '[':
      {
        skipWhiteSpace(ctx);
        if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
        Array data;
        skipWhiteSpace(ctx);
        for(;ctx.isNotEnd();) {
          if(isCloseParenthesis(ctx)) break;
          data.pushBack(parseLiteral(ctx));
          skipSeparator(ctx);
        }
        return data;
      }
    case 'o':
      if(ctx.getSubStr(3) != "obj")
        throw Exception(ErrCode::unexpected_expression,
                                     "Unexpected 'o' in struct description\n"
                                       "Maybe you haven't completed the 'obj' type?");
    case '{':
      {
        skipWhiteSpace(ctx);
        if(!isOpenParenthesis(ctx)) throw Exception(ErrCode::unexpected_expression, "Open parenthesis expected\n");
        Object data;
        skipWhiteSpace(ctx);
        for(;ctx.isNotEnd();) {
          if(isCloseParenthesis(ctx)) break;
          BufferArray name = parseName(ctx);
          skipWhiteSpace(ctx);
          if(*ctx == ':' || *ctx == '=') ++ctx;
          skipWhiteSpace(ctx);
          data.insert(std::move(name), parseLiteral(ctx));
          skipSeparator(ctx);
        }
        return data;
      }
    default: throw Exception(ErrCode::unexpected_expression);
  }
}

Variable Lexer::parseLiteral(Lexer::ParseContext& ctx) {
  for (;ctx.isNotEnd();++ctx) {
    switch (*ctx) {
      case '/':
        ++ctx;
        if(*ctx == '/') for(;*ctx != '\n';++ctx);
        elif(*ctx == '*') for(;true;++ctx) {
          if(*ctx == '*') { ++ctx; if(*ctx == '/') {break;} }
        } else throw Exception(ErrCode::unexpected_expression,
                               "Unexpected '/' in struct description\n"
                                 "Maybe you haven't completed the comment opening parenthesis?\n"
                                 "'//' or '/*...*/'");
        ++ctx;
      continue;

      case ' ': case '\t': case '\n': case '\r': case '\x0B': case '\f': case '\0':
      continue;

      case 'i': case 'f': case 'u': case 's': case 'a': case 'o': case '[': case '{':
      return parseContainer(ctx);
      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '.': case '-':
      case 't':
      return parsePrimitive(ctx, ContainerType::heterogeneous_container);
      case '"':{
        ++ctx;
        ui64 start = ctx.index;
        for(;ctx.isNotEnd() && *ctx != '"';++ctx);
        BufferArray data(ctx.text.substr(start, ctx.index - start));
        ++ctx;
        return data;
      }
      case '\'':{
        ++ctx;
        ui64 start = ctx.index;
        for(;ctx.isNotEnd() && *ctx != '\'';++ctx);
        BufferArray data(ctx.text.substr(start, ctx.index - start));
        ++ctx;
        return data;
      }

      default: throw Exception(ErrCode::unexpected_expression);
    }
  }
  throw Exception(ErrCode::unexpected_expression);
}

Variable Lexer::operator <<(std::string text) {
  ParseContext ctx{text, 0};
  return parseLiteral(ctx);
}
