#ifndef FILE_NODE_VISITOR_TEST_H
#define FILE_NODE_VISITOR_TEST_H

#include "binom/includes/file_storage/file_storage.h"

void file_node_visitor_test() {
  using namespace binom;

  FileStorage storage("test.db");
  std::clog << " * File storage has been inited\n";
  FileNodeVisitor root_node = storage;
  std::clog << " * Root node has been getted\n";
//  root_node.setVariable(varr{});

  std::clog << "=========================================================================\n"
               "|                  Check Data of opened Storage BEGIN                   |\n"
               "=========================================================================\n";

  storage.check();

  std::clog << "=========================================================================\n"
               "|                  Check Data of opened Storage END                     |\n"
               "=========================================================================\n";

  root_node.setVariable(vobj{
                          {"a", 1_ui8},
                          {"b", 2_ui16},
                          {"c", 4_ui32},
                          {"d", 8_ui64},
                          {"e", ui8arr{1,2,3}},
                          {"f", ui16arr{4,5,6}},
                          {"g", ui32arr{7,8,9}},
                          {"h", ui64arr{10,11,12}},
                          {"j", varr{1,2,3}},
                          {"k", vobj{
                            {"a", 1_ui8},
                            {"b", 2_ui16},
                            {"c", 4_ui32},
                            {"d", 8_ui64},
                            {"e", ui8arr{1,2,3}},
                            {"f", ui16arr{4,5,6}},
                            {"g", ui32arr{7,8,9}},
                            {"h", ui64arr{10,11,12}},
                            {"j", varr{1,2,3}}
                          }},
                          {"l", vobj{
                             {"a", 1_ui8},
                             {"b", 2_ui16},
                             {"c", 4_ui32},
                             {"d", 8_ui64},
                             {"e", ui8arr{1,2,3}},
                             {"f", ui16arr{4,5,6}},
                             {"g", ui32arr{7,8,9}},
                             {"h", ui64arr{10,11,12}},
                             {"j", varr{1,2,3}},
                             {"k", vobj{
                               {"a", 1_ui8},
                               {"b", 2_ui16},
                               {"c", 4_ui32},
                               {"d", 8_ui64},
                               {"e", ui8arr{1,2,3}},
                               {"f", ui16arr{4,5,6}},
                               {"g", ui32arr{7,8,9}},
                               {"h", ui64arr{10,11,12}},
                               {"j", varr{1,2,3}}
                            }}
                           }}
                        });

//  root_node.setVariable(
//        varr{
//          1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,
//          45,46,47,48,49,50,51,52,53,54,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70
//        }
//  );

//  root_node.insert("Hello", "World"); // Oh shit, here we go again...

//  root_node.setVariable(vobj{
//                          {"usr", varr{
//                             vobj{
//                               {"login", "admin"},
//                               {"password", "any_password"},
//                               {"access_lvl", 0xFF_ui8},
//                               {"data", vobj{}}
//                             }
//                           }},
//                          {"grp", varr{}},
//                          {"msgs", varr{}}
//                        });

//  OutputManip::primitive = OutputManip::Primitive::UNSIGNED;
//  OutputManip::buffer_array = OutputManip::BufferArray::PRIMITIVE;


  std::clog << " * File storage changed\n";


  std::clog << "=========================================================================\n"
               "|                  Check Data of changed Storage BEGIN                  |\n"
               "=========================================================================\n";

  storage.check();

  std::clog << "=========================================================================\n"
               "|                  Check Data of changed Storage END                    |\n"
               "=========================================================================\n";


  std::clog << root_node.getVariable() << '\n';

}

#endif // FILE_NODE_VISITOR_TEST_H
