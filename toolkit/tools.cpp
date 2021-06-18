#include "tools.h"

using namespace binom;


const char help_str[] =
"Tools:\n\n"
"* cat - print BinOM-file tree\n"
"$ binomtk cat -file <file_name_#1> <file_name_#2> ... <file_name_#n>\n\n"
"* sn - show nodes from BinOM-file\n"
"$ binomtk sn -file <file_name> -path <path_to_node_#1> <path_to_node#2> ... <file_name_#n>\n\n"
"* create - create BinOM-file\n"
"$ binomtk create";

std::map<std::string, std::function<binom::Variable(binom::Variable)>> tool_map = {



{"cat",
[](binom::Variable args)->binom::Variable{
  if(!args.contains("file")) {
    std::cerr << "Expected file name: -file <file_name_#1> <file_name_#2> ... <file_name_#n>\n";
    std::exit(-1);
  }

  for(binom::Variable& file_name : args.getVariable("file").toArray()) {
    std::string file_name_str = file_name.toBufferArray();
    openFile(file_name_str,
    [&](BinOMDataBase db)->void {
      std::clog << "File \"" << file_name_str << "\" type - Data base:\n" << db.getRoot().getVariable() << "\n\n";
    },
    [&](BinOMFile file) {
      std::clog << "File \"" << file_name_str << "\" type - Serialized data:\n" << file.load() << "\n\n";
    },
    [](Exception& except) {
      std::cerr << except.full() << '\n';
    });
  }

  return nullptr;
}},




{"sn",
[](binom::Variable args)->binom::Variable{
  std::string err_str;
  if(!args.contains("file"))
    err_str = "Expected file name: -file <file_name>\n";
  if(!args.contains("path"))
    err_str += "Expected path: -path <path_to_node_#1> <path_to_node#2> ... <file_name_#n>\n";
  if(err_str.length()) {
    std::cerr << err_str;
    std::exit(-1);
  }

  std::string file_name_str = args.getVariable("file").getVariable(0).toBufferArray();

  openFile(file_name_str,
  [&](BinOMDataBase db)->void {
    DBNodeVisitor node = db.getRoot();

    for(Variable& path_var : args.getVariable("path").toArray()) {
      try {
        DBNodeVisitor finded = node[Path::fromString(path_var.toBufferArray())];
        if(finded.isNull()) throw Exception(ErrCode::binom_out_of_range);
        std::cout << "Path: " << path_var.toBufferArray().toString()
                  << "\n|Type: " << toTypeString(finded.getType())
                  << "\n|Value: " << finded.getVariable() << "\n\n";
      } catch (Exception& except) {
        if(except.code() == ErrCode::binom_out_of_range)
          std::cout << "Path: " << path_var.toBufferArray().toString()
                    << "\n|Node not found!\n\n";
        else
          std::cout << "Path: " << path_var.toBufferArray().toString()
                    << "\n|" << except.full() << "\n\n";
      }
    }
  },
  [&](BinOMFile file) {
    Variable var = file.load();
    NodeVisitor node(&var);

    for(Variable& path_var : args.getVariable("path").toArray()) {
      try {
        NodeVisitor finded(node[Path::fromString(path_var.toBufferArray().toString())]);
        if(finded.isNull()) throw Exception(ErrCode::binom_out_of_range);
        std::cout << "Path: " << path_var.toBufferArray().toString()
                  << "\n|Type: " << toTypeString(finded.getType())
                  << "\n|Value: " << (finded.isValueRef()? finded.getValue() : finded.getVariable()) << "\n\n";
      } catch (Exception& except) {
        if(except.code() == ErrCode::binom_out_of_range)
          std::cout << "Path: " << path_var.toBufferArray().toString()
                    << "\n|Node not found!\n\n";
        else
          std::cout << "Path: " << path_var.toBufferArray().toString()
                    << "\n|" << except.full() << "\n\n";
      }
    }
  },
  [](Exception& except) {
    std::cerr << except.full() << '\n';
  });

  return nullptr;
}},



{"create",
[]([[maybe_unused]]binom::Variable args)->binom::Variable{
  std::string path;
  FileType file_type = FileType::invalid;
  if(!args.contains("file")) {
    std::clog << "Enter file name: ";
    std::cin >> path;
  } else path = args["file"][0].toBufferArray();

  while (file_type == FileType::invalid) {
    if(!args.contains("file-type")) {
      std::string str;
      std::clog << "Enter file type(serialized/data_base): ";
      std::cin >> str;
      file_type = toFileType(args["file-type"][0].toBufferArray());
    } else {
      file_type = toFileType(args["file-type"][0].toBufferArray());
      args.toObject().remove("file-type");
    }
  }


  switch (file_type) {
    case FileType::data_base:

    break;
    case FileType::serialized_data:

    break;
    default: throw Exception(ErrCode::binom_invalid_type);
  }


  return nullptr;
}}

};

void fromatOutput(Array output_rules) {
  for(Variable& rule : output_rules)
    if(rule.toBufferArray() == "hex")
      output_manip.primitive = OutputManip::HEX;
    elif(rule.toBufferArray() == "signed")
      output_manip.primitive = OutputManip::SIGNED;
    elif(rule.toBufferArray() == "unsigned")
      output_manip.primitive = OutputManip::UNSIGNED;
    elif(rule.toBufferArray() == "string")
      output_manip.buffer_array = OutputManip::STRING;
    elif(rule.toBufferArray() == "primitive")
      output_manip.buffer_array = OutputManip::PRIMITIVE;
    else
      std::clog << "Warning: invalid output rule - " << rule.toBufferArray().toString() << "\n\n";
}


int execute(Variable args) {

  if(args.contains("h") ||
     args.contains("help") ||
     !args.contains("tool-name")) {
    std::clog << help_str;
    std::exit(0);
  }

  if(args.contains("format-output")) fromatOutput(args.getVariable("format-output").toArray());

  std::string tool_name = args.getVariable("tool-name").getVariable(0).toBufferArray();
  auto tool_it = tool_map.find(tool_name);
  if(tool_it == tool_map.cend()) {
    std::cerr << "Unexpected tool-name\n";
    std::exit(-1);
  }

  try {
  tool_it->second(args);
  } catch(Exception& except) {
    std::cerr << except.full() << '\n';
    std::exit(-1);
  }

  return 0;
}
