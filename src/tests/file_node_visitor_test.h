#ifndef FILE_NODE_VISITOR_TEST_H
#define FILE_NODE_VISITOR_TEST_H

#include "binom/includes/file_storage/file_storage.h"

void file_node_visitor_test() {
  using namespace binom;

  FileStorage storage("test.db");
  FileNodeVisitor root_node = storage.getRoot();

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
                        }}
                        });

  root_node.insert("Hello", "World"); // Oh shit, here we go again...

  OutputManip::primitive = OutputManip::Primitive::UNSIGNED;
  OutputManip::buffer_array = OutputManip::BufferArray::STRING;

  std::clog << root_node.getVariable() << '\n';

}

#endif // FILE_NODE_VISITOR_TEST_H
