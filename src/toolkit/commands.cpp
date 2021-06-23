#include "cli.h"

using namespace binom;

enum class FileType {
  serialized_storage = 1,
  data_base = 2
};

const std::map<BufferArray, command_t> CLI::commands = {





{"cat", [](){
  requredArg("files", VarType::invalid_type,
             "File(s) isn't entered! Example: \n"
             "$ binomtk cat <file_#1> <file_#2> ... <file_#N>\n");

   for(Variable file_name_var : getArgs()["files"].toArray())
   try {
     if(std::string file_name = file_name_var.toBufferArray();
     FileIO::isExist(file_name)) {
       if(BinOMDataBase::isValid(file_name)) {
         BinOMDataBase db(file_name);
         std::cout <<
         "File: \"" << file_name << "\";\n"
         "Type: DataBase;"
         "\nContent: " << db.getRoot().getVariable() << ";\n\n";
       } else {
         BinOMFile file(file_name);
         Variable content = file.load();
         std::cout <<
         "File: \"" << file_name << "\";\n"
         "Type: Serialized;"
         "\nContent: " << content << ";\n\n";
       }
     } else std::cerr << "File \"" << file_name << "\" isn't exist";
   } catch(const Exception& except) {
     std::cerr << "File \"" << file_name_var.toBufferArray().toString()
     << "\" read file error: " << except.full() << std::endl;
     continue;
   }
}},






{"pnode", [](){
   requredArg("file", VarType::invalid_type,
              "File isn't entered! Example: \n"
              "$ binomtk pnode <file> <path_to_node_#1> <path_to_node_#2> ... <path_to_node_#N>\n");

   requredArg("paths", VarType::invalid_type,
              "Path(s) isn't entered! Example: \n"
              "$ binomtk pnode <file> <path_to_node_#1> <path_to_node_#2> ... <path_to_node_#N>\n");

   if(std::string file_name = CLI::getArgs()["file"].toBufferArray();
   FileIO::isExist(file_name)) {
     try {
       if(BinOMDataBase::isValid(file_name)) {
         BinOMDataBase db(file_name);
         DBNodeVisitor root_node = db.getRoot();
         std::cout <<
         "File: \"" << file_name << "\";\n"
         "Type: DataBase;\n\n";

         for(Variable path_var : getArgs()["paths"].toArray()) {
           std::string path_str = path_var.toBufferArray();
           DBNodeVisitor node = root_node[Path::fromString(path_str)];
           if(node.isNull()) {
             std::cout << "Path: " << path_str
                       << ";\nNode isn't exist!\n\n";
             continue;
           }
           std::cout << "Path: " << path_str
                     << ";\nType: " << toTypeString(node.getType())
                     << ";\nValue: " << node.getVariable() << ";\n\n";
         }
       } else {
         BinOMFile file(file_name);
         Variable content = file.load();
         NodeVisitor root_node = &content;
         std::cout <<
         "File: \"" << file_name << "\";\n"
         "Type: Serialized;\n\n";

         for(Variable path_var : getArgs()["paths"].toArray()) {
           std::string path_str = path_var.toBufferArray();
           NodeVisitor node = root_node[Path::fromString(path_str)];
           if(node.isNull()) {
             std::cout << "Path: " << path_str
                       << ";\nNode isn't exist!\n\n";
             continue;
           }
           std::cout << "Path: " << path_str
                     << ";\nType: " << toTypeString(node.getType())
                     << ";\nValue: " << (node.isValueRef()? node.getValue() : node.getVariable()) << ";\n\n";
         }
       }
     } catch(const Exception& except) {
       std::cerr << "File \"" << file_name
       << "\" read file error: " << except.full() << std::endl;
     }
   } else std::cerr << "File \"" << file_name << "\" isn't exist";

}},






{"mk", [](){
   requredArg("file", VarType::invalid_type,
              "File isn't entered! Example: \n"
              "$ binomtk pnode <file> <path_to_node_#1> <path_to_node_#2> ... <path_to_node_#N>\n");
   clearConsole();

   FileType file_type;

   while (true) {
     int ftype;
     std::clog << "File types:\n"
                  "| 1 - Serialized Storage\n"
                  "| 2 - DataBase\n"
                  "Enter file type: ";
     std::cin >> ftype;
     if(ftype == 1 || ftype == 2) {
       file_type = static_cast<FileType>(ftype);
       break;
     } else std::cerr << "Invalid value!\n";
   }



   VarType type;
   std::clog << "Variable types:\n" << VAR_TYPES
             << "Enter container root type: ";
   std::cin >> type;

   switch (file_type) {
     case FileType::serialized_storage:{
       Variable var(type);
       editValue(&var);
       BinOMFile(CLI::getArgs()["file"].toBufferArray()).write(var);
     } break;
     case FileType::data_base:{
       BinOMDataBase db(CLI::getArgs()["file"].toBufferArray(), type);
       editValue(db.getRoot());
     } break;
   }

}},






{"edit", [](){
   requredArg("file", VarType::invalid_type,
              "File isn't entered! Example: \n"
              "$ binomtk pnode <file> <path_to_node_#1> <path_to_node_#2> ... <path_to_node_#N>\n");
   clearConsole();

   if(std::string file_name =  getArgs()["file"].toBufferArray();
   FileIO::isExist(file_name)) {
     if(BinOMDataBase::isValid(file_name)) {
       BinOMDataBase db(file_name);
       editValue(db.getRoot());
     } else {
       BinOMFile file(file_name);
       Variable content = file.load();
       editValue(&content);
       file.write(content);
     }
   } else std::cerr << "File \"" << file_name << "\" isn't exist";

}},

};
