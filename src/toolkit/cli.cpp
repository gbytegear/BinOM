#include "cli.h"

using namespace binom;

static const CLI* cli_obj_ptr = nullptr;

const char HELP_TEXT[] =
"Usage: binomtk <Command>...\n"
"Description: BinOM structure management toolkit.\n\n"
"Commands:\n|\n"
"| * cat - print BinOM-files content\n"
"| $ binomtk cat <file_#1> <file_#2> ... <file_#N>\n|\n"
"| * pnode - print nodes from BinOM-file\n"
"|           path example: name[0]name_1.name_2\n"
"| $ binomtk pnode <file> <path_to_node_#1> <path_to_node_#2> ... <path_to_node_#N>\n|\n"
"| * mk - create BinOM-file\n"
"| $ binomtk mk <file>\n|\n"
"| * edit - edit BinOM-file\n"
"| $ binomtk edit <file>\n|\n"
"| * build - build BinOM-file from BinOM source\n"
"| $ binomtk build <source-file> <output-file>\n"
"+\n\n"
"Optional flags:\n|\n"
"| * help - print this manual\n"
"| $ binomtk -help [or:-h, --help]\n|\n"
"| * license - print license\n"
"| $ binomtk -license [or:-l, --license]\n|\n"
"| * outflag - change BinOM-tree output mode\n"
"|             primitive: 's' - signed int, 'u' - unsigned int, 'h' - hex\n"
"|             buffer array: 'p' - primitive array, 't' - string\n"
"| $ binomtk -outflag hp (primitive - hex, buffer array - primitive array)\n|\n"
"+\n\n";

const char LICENSE_TEXT[] =
"MIT License\n\n"
"Copyright (c) 2020 Max Shemendiuk\n\n"
"Permission is hereby granted, free of charge, to any person obtaining a copy\n"
"of this software and associated documentation files (the \"Software\"), to deal\n"
"in the Software without restriction, including without limitation the rights\n"
"to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
"copies of the Software, and to permit persons to whom the Software is\n"
"furnished to do so, subject to the following conditions:\n\n"
"The above copyright notice and this permission notice shall be included in all\n"
"copies or substantial portions of the Software.\n\n"
"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
"SOFTWARE.\n\n";


const binom::Object CLI::arg_tmpl = vobj {

  {"cmd_args", vobj{
    {"cat", varr{"files"}},
    {"pnode", varr{"file", "paths"}},
    {"mk", varr{"file"}},
    {"edit", varr{"file"}},
    {"build", varr{"files"}}
  }},

  {"arg_types", vobj{
    {"file",ui8(VarType::byte_array)},
    {"files",ui8(VarType::array)},
    {"path",ui8(VarType::byte_array)},
    {"paths",ui8(VarType::array)},
  }},

  {"optional_arg", vobj{
    {"outflag", ui8(VarType::byte_array)}, // s - signed, u - unsigned, h - hex, p - primitive, t - string
    {"showargs", ui8(VarType::end)},
    {"h", ui8(VarType::end)},
    {"help", ui8(VarType::end)},
    {"-help", ui8(VarType::end)},
    {"l", ui8(VarType::end)},
    {"license", ui8(VarType::end)},
    {"-license", ui8(VarType::end)},
  }}

};


const Object& CLI::getArgs() {return cli_obj_ptr->args;}

Object CLI::paresArgs(int argc, char* argv[]) {
  Object args;

  class CurrentArg {
    Object buffer = vobj{};
    bool is_optional = false;
  public:

    BufferArray cmd = VarType::byte_array;
    i64 arg_chain_index = -1;
    BufferArray name = "cmd";
    VarType type = VarType::byte_array;

    void next() {
      ++arg_chain_index;
      if(arg_tmpl["cmd_args"][cmd].toArray().getMemberCount() > ui64(arg_chain_index)) {
        name = arg_tmpl["cmd_args"][cmd][arg_chain_index].toBufferArray();
        type = VarType(arg_tmpl["arg_types"][name].getValue().asUi8());
      } else {
        arg_chain_index = -1;
      }
    }

    bool isOptional() const { return is_optional; }

    void beginOptional() {
      buffer = vobj{
        {"arg_chain_index", arg_chain_index},
        {"name", name},
        {"type", ui8(type)}
      };
      is_optional = true;
    }

    void endOptional() {
      if(is_optional) {
        arg_chain_index = buffer["arg_chain_index"].getValue().asI64();
        name = buffer["name"].toBufferArray();
        type = VarType(buffer["type"].getValue().asUi8());
        buffer = vobj {};
        is_optional = false;
      }
    }

  } current_arg;


  for(int i = 0; i < argc; ++i) {
    if(argv[i][0] == '-') {
      if(arg_tmpl["arg_types"].contains(argv[i] + 1)) { // Input argumment flag
        current_arg.name = argv[i] + 1;
        current_arg.type = VarType(arg_tmpl["arg_types"][argv[i] + 1].getValue().asUi8());
        current_arg.arg_chain_index = -1;
      } elif(arg_tmpl["optional_arg"].contains(argv[i] + 1)) {
        current_arg.beginOptional();
        current_arg.name = argv[i] + 1;
        current_arg.type = VarType(arg_tmpl["optional_arg"][argv[i] + 1].getValue().asUi8());
        current_arg.arg_chain_index = -1;
        if(current_arg.type == VarType::end) {
          args.insert(current_arg.name, argv[i] + 1);
          current_arg.endOptional();
          continue;
        }
      } else {
        std::cerr << "Invalide flag " << argv[i] << " ignored!\n";
        continue;
      }
    } else {

      if(current_arg.name == "cmd") { // Input command
        if(!arg_tmpl["cmd_args"].contains(argv[i])) throw Exception(ErrCode::invalid_data, "Invalid command");
        if(args.contains("cmd")) throw Exception(ErrCode::invalid_data, "Command is already exist");
        args.insert("cmd", argv[i]);

        current_arg.cmd = argv[i];
        current_arg.next();

        continue;


      } else { // Input argumment
        switch (current_arg.type) {
        case binom::VarType::byte_array:
          args.insert(current_arg.name, argv[i]);
          if(current_arg.isOptional()) current_arg.endOptional();
          else current_arg.next();
        continue;
        case binom::VarType::array:
          if(!args.contains(current_arg.name)) args.insert(current_arg.name, varr{});
          args[current_arg.name].toArray().pushBack(argv[i]);
          if(current_arg.isOptional()) current_arg.endOptional();
        continue;
        default: throw Exception(ErrCode::invalid_data, "Invalid argumment type");
        }
      }
    }
  }

  return args;
}

void CLI::requredArg(BufferArray name,
                     VarType requred_type,
                     const char* exist_err,
                     const char* type_err) {
  if(!CLI::getArgs().contains(name)) throw Exception(ErrCode::invalid_data, exist_err);
  if(requred_type != VarType::invalid_type)
    if(CLI::getArgs()[name].type() == requred_type) throw Exception(ErrCode::invalid_data, type_err);
}

void CLI::formatOutput(BufferArray format) {
  for(ValueRef value : format)
    switch (value.asI8()) {
    case 'h': OutputManip::primitive = OutputManip::Primitive::HEX; continue;
    case 'u': OutputManip::primitive = OutputManip::Primitive::UNSIGNED; continue;
    case 's': OutputManip::primitive = OutputManip::Primitive::SIGNED; continue;
    case 'p': OutputManip::buffer_array = OutputManip::BufferArray::PRIMITIVE; continue;
    case 't': OutputManip::buffer_array = OutputManip::BufferArray::STRING; continue;
    default: std::cerr << "Invalid output flag '" << value.asI8() << "'\n";
    }
}

void CLI::printHelp() {
  std::cout << HELP_TEXT;
  if(args.contains("l") || args.contains("license") || args.contains("-license")) printLicense();
  std::exit(0);
}

void CLI::printLicense() {
  std::cout << LICENSE_TEXT;
  std::exit(0);
}

void CLI::exec() {
  if(!args.contains("cmd"))
    printHelp();
  elif(auto it = commands.find(args["cmd"].toBufferArray()); it == commands.cend())
      throw Exception(ErrCode::invalid_data, "Command isn't exist");
  else
  it->second();
}

CLI::CLI(int argc, char* argv[]) : args(paresArgs(--argc, ++argv)) {
  if(!cli_obj_ptr)cli_obj_ptr = this;
  else {
    std::cerr << "CLI is exist!\n";
    std::exit(-1);
  }

  // Pre-excution
  if(args.contains("showargs")) std::clog << "Parsed args: " << args << "\n\n";
  if(args.contains("h") || args.contains("help") || args.contains("-help")) printHelp();
  if(args.contains("l") || args.contains("license") || args.contains("-license")) printLicense();
  if(args.contains("outflag")) formatOutput(args["outflag"].toBufferArray());

  exec();
  std::exit(0);
}

std::istream& binom::operator>>(std::istream& is, VarType& type) {
  static const char values[] =
  "| 1 - byte\n"
  "| 2 - word\n"
  "| 3 - dword\n"
  "| 4 - qword\n"
  "| 5 - byte array\n"
  "| 6 - word array\n"
  "| 7 - dword array\n"
  "| 8 - qword array\n"
  "| 9 - array\n"
  "| 10 - object\n";

  while(true) {
    ui16 number;
    std::string input;

    is >> input;

    if(!isUnsigned(input)) {
      std::cerr << "Invalid type\n"<< values <<"Try again: ";
      continue;
    }

    number = std::stoi(input);

    if(VarType(number) < VarType::byte || VarType(number) > VarType::object) {
      std::cerr << "Invalid type\n"<< values <<"Try again: ";
      continue;
    }

    type = static_cast<VarType>(number);

    return is;
  }
}

std::istream& binom::operator>>(std::istream& is, VarTypeClass& type_class) {
  static const char values[] =
  "| 1 - primitive\n"
  "| 2 - buffer array\n"
  "| 3 - array\n"
  "| 4 - object\n";

  while(true) {
    ui16 number;
    std::string input;

    is >> input;

    if(!isUnsigned(input)) {
      std::cerr << "Invalid type\n"<< values <<"Try again: ";
      continue;
    }

    number = std::stoi(input);

    if(VarTypeClass(number) < VarTypeClass::primitive ||
       VarTypeClass(number) > VarTypeClass::object) {
      std::cerr << "Invalid type class\n"<< values <<"Try again: ";
      continue;
    }

    type_class = static_cast<VarTypeClass>(number);
    return is;
  }
}

std::istream& binom::operator>>(std::istream& is, ValType& val_type) {
  static const char values[] =
  "| 1 - byte\n"
  "| 2 - word\n"
  "| 3 - dword\n"
  "| 4 - qword\n";

  while(true)  {
    ui16 number;
    std::string input;

    is >> input;

    if(!isUnsigned(input)) {
      std::cerr << "Invalid type\n"<< values <<"Try again: ";
      continue;
    }

    number = std::stoi(input);

    if(ValType(number - 1) > ValType::qword) {
      std::cerr << "Invalid value type\n"<< values <<"Try again: ";
      continue;
    }

    val_type = static_cast<ValType>(number - 1);
    return is;
  }

}
