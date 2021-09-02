#ifndef FILE_NODE_VISITOR_TEST_H
#define FILE_NODE_VISITOR_TEST_H

#include "binom/includes/file_storage/file_storage.h"

void setTest() {
  using namespace binom;
  FileStorage storage("assign_test.db");
  std::clog << " * File storage has been inited\n";
  FileNodeVisitor root_node = storage;
  std::clog << " * Root node has been getted\n";
  Variable var = vobj{
    {ui64arr{1,2,3,255}, ui64arr{4,5,6}},
    {ui32arr{1,2,3,255}, ui32arr{4,5,6}},
    {ui16arr{1,2,3,255}, ui16arr{4,5,6}},
    {ui8arr{1,2,3,255}, ui8arr{4,5,6}},
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
  };

  root_node.setVariable(var);

  std::clog << "=========================================================================\n"
                 "|                  Check Data of changed Storage BEGIN                  |\n"
                 "=========================================================================\n";

  storage.check();

  std::clog << "=========================================================================\n"
                 "|                  Check Data of changed Storage END                    |\n"
                 "=========================================================================\n";

  std::clog << root_node.getVariable() << '\n';
  std::clog << "Variable:\n" << var;

}

void bufferArrayInsertTest() {
  using namespace binom;

  FileStorage storage("buffer_array_insert_test.db", varr{
                        ui8arr{3,6,9},
                        ui16arr{3,6,9},
                        ui32arr{3,6,9},
                        ui64arr{3,6,9}
                      }, true);

  std::clog << " * File storage has been inited\n";
  FileNodeVisitor root_node = storage;
  std::clog << " * Root node has been getted\n";

//  std::clog << "=========================================================================\n"
//               "|                  Check Data of opened Storage BEGIN                   |\n"
//               "=========================================================================\n";

//  storage.check();

//  std::clog << "=========================================================================\n"
//               "|                  Check Data of opened Storage END                     |\n"
//               "=========================================================================\n";

  // Buffer array insert test
  root_node[0].pushFront(ui64arr{1,2});
  root_node[0].insert(3 , ui64arr{4,5});
  root_node[0].insert(6 , ui64arr{7,8});
  root_node[0].pushBack(ui64arr{10, 11});
  root_node[0].pushFront(0_ui64);
  root_node[0].pushBack(13_ui64);
  root_node[0].insert(12, 12_ui64);

  root_node[1].pushFront(ui32arr{1,2});
  root_node[1].insert(3 , ui32arr{4,5});
  root_node[1].insert(6 , ui32arr{7,8});
  root_node[1].pushBack(ui32arr{10, 11});
  root_node[1].pushFront(0_ui32);
  root_node[1].pushBack(13_ui32);
  root_node[1].insert(12, 12_ui32);

  root_node[2].pushFront(ui16arr{1,2});
  root_node[2].insert(3 , ui16arr{4,5});
  root_node[2].insert(6 , ui16arr{7,8});
  root_node[2].pushBack(ui16arr{10, 11});
  root_node[2].pushFront(0_ui16);
  root_node[2].pushBack(13_ui16);
  root_node[2].insert(12, 12_ui16);

  root_node[3].pushFront(ui8arr{1,2});
  root_node[3].insert(3 , ui8arr{4,5});
  root_node[3].insert(6 , ui8arr{7,8});
  root_node[3].pushBack(ui8arr{10, 11});
  root_node[3].pushFront(0_ui8);
  root_node[3].pushBack(13_ui8);
  root_node[3].insert(12, 12_ui8);

  OutputManip::buffer_array = OutputManip::BufferArray::PRIMITIVE;
  OutputManip::primitive = OutputManip::Primitive::UNSIGNED;

  std::clog << " * File storage changed\n";


//  std::clog << "=========================================================================\n"
//               "|                  Check Data of changed Storage BEGIN                  |\n"
//               "=========================================================================\n";

//  storage.check();

//  std::clog << "=========================================================================\n"
//               "|                  Check Data of changed Storage END                    |\n"
//               "=========================================================================\n";


  std::clog << root_node.getVariable() << '\n';
}

void file_node_visitor_test() {
  using namespace binom;
//  setTest();

  FileStorage storage("set_buffarr_element.binomdb", vobj{ {"test", ui16arr{1,2,2,4,5}} });
  FileNodeVisitor node = storage;
  std::clog << "Before change:\n" << node.getVariable() << '\n';
  node[{"test", 2}].setVariable(3_ui16);
  std::clog << "After change:\n" << node.getVariable() << '\n';
}

#endif // FILE_NODE_VISITOR_TEST_H
