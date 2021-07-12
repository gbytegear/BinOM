#ifndef MULTITHREAD_H
#define MULTITHREAD_H

#include "binom/includes/binom.h"
#include <thread>

using namespace binom;

BinOMDataBase data_base("db.binomdb", vobj{
                          {"test", varr{
                             vobj{
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
                             }
                           }}
});

DBNodeVisitor root = data_base.getRoot();

void reader() {
  try {
    DBNodeVisitor node = root["test"];
    std::cout << node[rand()%node.getElementCount()].getVariable() << '\n';
  }  catch (const Exception& except) {
    std::cerr << except.full() << '\n';
    std::terminate();
  }
}

void writer() {
  try {
    Variable var = vobj{
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
    switch (rand()%3) {
      case 0:
        root["test"].pushBack(var);
        std::clog << "\n\nPush back\n\n";
      return;
      case 1:
        root["test"].pushFront(var);
        std::clog << "\n\nPush front\n\n";
      return;
      case 2: {
        DBNodeVisitor node = root["test"];
        node.insert(node.getElementCount()/2, var);
        std::clog << "\n\nInsert to middle\n\n";
      }
      return;
    }
  }  catch (const Exception& except) {
    std::cerr << except.full() << '\n';
    std::terminate();
  }
}

#include <list>

void test() {
  std::clog << "DataBase inited\n";
  output_manip.buffer_array = output_manip.PRIMITIVE;
  std::clog << root.getVariable() << '\n';
  srand(time(nullptr));
  using namespace std::chrono_literals;
  std::list<std::thread> threads;
  for(int i = 0; i < 500; ++i) {
    threads.emplace_back(writer);
    threads.emplace_back(reader);
  }
  for(std::thread& thr : threads)
    thr.join();
}


#endif // MULTITHREAD_H
