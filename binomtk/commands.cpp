#include "cli.h"
#include "../libbinom/include/lexer.h"

using namespace binom;

void editor(std::unique_ptr<binom::NodeVisitorBase> root);

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
           case binom::FileType::dynamic_storage: {
             binom::DynamicStorage storage(file_name);
             std::cout <<
             "File: \"" << file_name << "\";\n"
             "Type: Storage;"
             "\nContent: " << storage.getRoot().getVariable() << ";\n\n";
           }break;
           case binom::FileType::serialized_storage: {
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
           case binom::FileType::serialized_storage: {
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
           case binom::FileType::dynamic_storage: {
             binom::DynamicStorage storage(file_name);

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
       std::getline(std::cin, file_name, '\n');
     } else file_name = CLI::getArgs()["file"].toBufferArray();

     FileType file_type;
     while(true) {
       std::string input;
       std::clog << "Enter file type:\n"
       "| 1 - Dynamic storage\n"
       "| 2 - Serialized storage\n"
       ":";
       std::getline(std::cin, input, '\n');

       if(!isUnsigned(input)) {
         std::cerr << "Invalid input\n";
         continue;
       }

       int i = std::stoi(input);
       if(i != int(FileType::dynamic_storage) && i != int(FileType::serialized_storage)) {
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
       case FileType::serialized_storage: {
         Variable data(type);
         std::unique_ptr<NodeVisitorBase> node = std::unique_ptr<NodeVisitorBase>(new NodeVisitor(data));
         SerializedStorage storage(file_name);
         editor(std::move(node));
         storage = data;
       }return;
       case FileType::dynamic_storage: {
         DynamicStorage storage(file_name, type, true);
         std::unique_ptr<NodeVisitorBase> node = std::unique_ptr<NodeVisitorBase>(new FileNodeVisitor(storage));
         editor(std::move(node));
       }return;

     }

   }},


  {"build", [](){
     requredArg("files", VarType::invalid_type,
                "Files isn't entered! Example:\n"
                "$ binomtk build <source-file> <output-file>");
     Array files = getArgs()["files"].toArray();
     if(files.getMemberCount() < 2) {
       std::cerr << "2 arguments expected!\n"
                    "$ binomtk build <source-file> <output-file>\n";
       std::exit(-1);
     } elif(files.getMemberCount() > 2) {
       std::cerr << "2 arguments expected! The rest of the arguments will be ignored.\n"
                    "$ binomtk build <source-file> <output-file>\n";
     }

     FileIO file(files[0].toBufferArray().toString());
     if(!file.isExist()) {
       std::cerr << "File \"" << file.getPath().string() << "\" isn't exist!";
       std::exit(-1);
     }
     Variable data;
     try {
       data = lexer << file.read(0, file.getSize()).toStdString();
     }  catch (Exception& except) {
       std::cerr << except.full() << '\n';
       std::exit(-1);
     }
     SerializedStorage ser_file(files[1].toBufferArray().toString());
     ser_file.write(data);
     std::exit(0);
  }},


{"edit", [](){
   requredArg("file", VarType::invalid_type,
              "File isn't entered! Example:\n"
              "$ binomtk edit <file>\n");
   clearConsole();

   if(std::string file_name =  getArgs()["file"].toBufferArray();
   FileIO::isExist(file_name)) {
     switch (binom::checkFileType(file_name)) {
       case binom::FileType::undefined_file:
        std::cerr << "Unknown file type\n";
       return;
       case binom::FileType::dynamic_storage: {
         DynamicStorage storage(file_name);
         std::unique_ptr<NodeVisitorBase> node = std::unique_ptr<NodeVisitorBase>(new FileNodeVisitor(storage));
         editor(std::move(node));
       } break;
       case binom::FileType::serialized_storage:{
         SerializedStorage storage(file_name);
         Variable data = storage;
         std::unique_ptr<NodeVisitorBase> node = std::unique_ptr<NodeVisitorBase>(new NodeVisitor(data));
         editor(std::move(node));
         storage = data;
       } break;
     }
   } else std::cerr << "File \"" << file_name << "\" isn't exist";

}},

  {"convert", [](){
     requredArg("files", VarType::invalid_type,
                "File isn't entered! Example:\n"
                "$ binomtk convert <old_file> <new_file>\n");
     Array files = getArgs()["files"].toArray();

     if(files.getMemberCount() < 2) {
       std::cerr << "2 arguments expected!\n"
                    "$ binomtk convert <old_file> <new_file>\n";
       std::exit(-1);
     } elif(files.getMemberCount() > 2) {
       std::cerr << "2 arguments expected! The rest of the arguments will be ignored.\n"
                    "$ binomtk convert <old_file> <new_file>\n";
     }

     std::string url_str = files[0].toBufferArray().toString();
     if(!FileIO::isExist(url_str)) {
       std::cerr << "File \"" << url_str << "\" isn't exist\n";
       std::exit(-1);
     }

     switch (checkFileType(url_str)) {
       case binom::FileType::undefined_file:
       std::cerr << "File \"" << url_str << "\" Unknown file type\n";
       std::exit(-1);
       case binom::FileType::dynamic_storage: {
         SerializedStorage s_storage(files[1].toBufferArray().toString());
         s_storage = DynamicStorage(url_str).getRoot().getVariable();
         std::clog << "Converted: [DynamicStorage] " << url_str << " => [SerializedStorage] " << files[1].toBufferArray().toString() << '\n';
       } break;
       case binom::FileType::serialized_storage: {
         DynamicStorage(files[1].toBufferArray().toString(), SerializedStorage(url_str), true);
         std::clog << "Converted: [SerializedStorage] " << url_str << " => [DynamicStorage] " << files[1].toBufferArray().toString() << '\n';
       } break;
     }

   }}

};
