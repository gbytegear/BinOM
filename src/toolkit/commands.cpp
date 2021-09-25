#include "cli.h"

using namespace binom;

void editor(std::unique_ptr<binom::NodeVisitorBase> root, bool edit_root = false);

const std::map<BufferArray, command_t> CLI::commands = {

  {"cat", [](){
     requredArg("files", VarType::invalid_type,
                "File(s) isn't entered! Example: \n"
                "$ binomtk cat <file_#1> <file_#2> ... <file_#N>\n");

     for(Variable file_name_var : getArgs()["files"].toArray()) try {
       if(std::string file_name = file_name_var.toBufferArray();
          FileIO::isExist(file_name)) {
         switch (checkFileType(file_name)) {
           default: std::cerr << "File \"" << file_name << "\" is invalid";
           continue;
           case binom::FileType::file_storage: {
             binom::FileStorage storage(file_name);
             std::cout <<
             "File: \"" << file_name << "\";\n"
             "Type: Storage;"
             "\nContent: " << storage.getRoot().getVariable() << ";\n\n";
           }break;
           case binom::FileType::serialized_file_storage: {
             binom::SerializedStorage storage(file_name);
             std::cout <<
             "File: \"" << file_name << "\";\n"
             "Type: Serialized;"
             "\nContent: " << storage.read() << ";\n\n";
           }break;

         }
       } else std::cerr << "File \"" << file_name << "\" isn't exist";
     }  catch (const Exception& except) {
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

         switch (checkFileType(file_name)) {
           default: std::cerr << "File \"" << file_name << "\" is invalid"; return;
           case binom::FileType::serialized_file_storage: {
             binom::SerializedStorage storage(file_name);
             binom::Variable variable = storage;

             std::cout <<
             "File: \"" << file_name << "\";\n"
             "Type: Serialized;\n\n";

             for(Variable path_var : getArgs()["paths"].toArray()) {
               std::string path_str = path_var.toBufferArray();
               binom::NodeVisitor node = binom::NodeVisitor(variable)(Path::fromString(path_str));
               if(node.isNull()) {
                 std::cout << "Path: " << path_str
                           << ";\nNode isn't exist!\n\n";
               } else {
                  std::cout << "Path: " << path_str
                            << ";\nType: " << toTypeString(node.getType())
                            << ";\nValue: " << node.getVariable() << ";\n\n";
               }
             }



           }break;
           case binom::FileType::file_storage: {
             binom::FileStorage storage(file_name);

             std::cout <<
             "File: \"" << file_name << "\";\n"
             "Type: Storage;\n\n";

             for(Variable path_var : getArgs()["paths"].toArray()) {
               std::string path_str = path_var.toBufferArray();
               binom::FileNodeVisitor node = storage.getRoot()(Path::fromString(path_str));
               if(node.isNull()) {
                 std::cout << "Path: " << path_str
                           << ";\nNode isn't exist!\n\n";
               } else {
                 std::cout << "Path: " << path_str
                           << ";\nType: " << toTypeString(node.getType())
                           << ";\nValue: " << node.getVariable() << ";\n\n";
               }
             }

           }break;
         }

       } catch(const Exception& except) {
         std::cerr << "File \"" << file_name
         << "\" read file error: " << except.full() << std::endl;
       }
     } else std::cerr << "File \"" << file_name << "\" isn't exist";
   }},



  {"mk", []{
     std::string file_name;
     if(!CLI::getArgs().contains("file")) {
       std::clog << "Enter file name: ";
       std::cin >> file_name;
     } else file_name = CLI::getArgs()["file"].toBufferArray();

     FileType file_type;
     while(true) {
       std::string input;
       std::clog << "Enter file type:\n"
       "| 1 - File storage\n"
       "| 2 - Serialized storage\n"
       ":";
       std::cin >> input;

       if(!isUnsigned(input)) {
         std::cerr << "Invalid input\n";
         continue;
       }

       int i = std::stoi(input);
       if(i != int(FileType::file_storage) && i != int(FileType::serialized_file_storage)) {
         std::cerr << "Invalid input\n";
         continue;
       }

       file_type = FileType(i);

       break;
     }


     VarType type;
     std::clog << "Enter root type:\n"
     "| 1 - byte\n"
     "| 2 - word\n"
     "| 3 - dword\n"
     "| 4 - qword\n"
     "| 5 - byte array\n"
     "| 6 - word array\n"
     "| 7 - dword array\n"
     "| 8 - qword array\n"
     "| 9 - array\n"
     "| 10 - object\n"
     ":";
     std::cin >> type;


     switch (file_type) {
       default: std::cerr << "Invilid file type!"; return;
       case FileType::serialized_file_storage: {
         Variable data(type);
         std::unique_ptr<NodeVisitorBase> node = std::unique_ptr<NodeVisitorBase>(new NodeVisitor(data));
         SerializedStorage storage(file_name);
         editor(std::move(node), true);
         storage = data;
       }return;
       case FileType::file_storage: {
         FileStorage storage(file_name, type, true);
         std::unique_ptr<NodeVisitorBase> node = std::unique_ptr<NodeVisitorBase>(new FileNodeVisitor(storage));
         editor(std::move(node), true);
       }return;

     }

   }}

//{"edit", [](){
//   requredArg("file", VarType::invalid_type,
//              "File isn't entered! Example: \n"
//              "$ binomtk edit <file>\n");
//   clearConsole();

//   if(std::string file_name =  getArgs()["file"].toBufferArray();
//   FileIO::isExist(file_name)) {
//     if(BinOMDataBase::isValid(file_name)) {
//       BinOMDataBase db(file_name);
//       editValue(db.getRoot());
//     } else {
//       BinOMFile file(file_name);
//       Variable content = file.load();
//       editValue(&content);
//       file.write(content);
//     }
//   } else std::cerr << "File \"" << file_name << "\" isn't exist";

//}},

};
