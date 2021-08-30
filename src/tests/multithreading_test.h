#ifndef MULTITHREADING_TEST_H
#define MULTITHREADING_TEST_H

#include "binom/includes/file_storage/file_storage.h"
#include <thread>

using namespace binom;

binom::FileStorage storage("mthr_test.db",
                           varr{
                             vobj{
                               {"a", 1_ui8},
                               {"b", 2_ui16},
                               {"c", 4_ui32},
                               {"d", 8_ui64},
                               {"e", ui8arr{1,2,3}},
                               {"f", ui16arr{4,5,6}},
                               {"g", ui32arr{7,8,9}},
                               {"h", ui64arr{10,11,12}},
                               {"j", varr{1,2,3}}
                             }
                           }, false);

//binom::FileStorage storage("mthr_test.db",
//                           varr{
//                             vobj{
//                               {"a", 1_ui8},
//                               {"b", 2_ui16},
//                               {"c", 4_ui32},
//                               {"d", 8_ui64},
//                               {"e", ui8arr{1,2,3}},
//                               {"f", ui16arr{4,5,6}},
//                               {"g", ui32arr{7,8,9}},
//                               {"h", ui64arr{10,11,12}},
//                               {"j", varr{1,2,3}},
//                               {"k", vobj{
//                                  {"a", 1_ui8},
//                                  {"b", 2_ui16},
//                                  {"c", 4_ui32},
//                                  {"d", 8_ui64},
//                                  {"e", ui8arr{1,2,3}},
//                                  {"f", ui16arr{4,5,6}},
//                                  {"g", ui32arr{7,8,9}},
//                                  {"h", ui64arr{10,11,12}},
//                                  {"j", varr{1,2,3}}
//                                }}
//                             }
//                           }, false);

void writer() {
  try{
    FileNodeVisitor node = storage;
    Variable var = vobj{
    {"a", 1_ui8},
    {"b", 2_ui16},
    {"c", 4_ui32},
    {"d", 8_ui64},
    {"e", ui8arr{1,2,3}},
    {"f", ui16arr{4,5,6}},
    {"g", ui32arr{7,8,9}},
    {"h", ui64arr{10,11,12}},
    {"j", varr{1,2,3}}
  };
    switch (rand()%3) {
      case 0:
        node.pushBack(var);
        std::clog << "\n\nPush back\n\n";
      return;

      case 1:
        node.pushFront(var);
        std::clog << "\n\nPush front\n\n";
      return;

      case 2:
        node.insert(node.getElementCount()/2, var);
        std::clog << "\n\nInsert to middle\n\n";
      return;
    }
  } catch(const Exception& except) {
    std::cerr << except.full() << '\n';
    std::terminate();
  }
}



void reader() {
  try {
    FileNodeVisitor node = storage;
    std::cout << node[rand()%node.getElementCount()].getVariable() << '\n';
  }  catch (const Exception& except) {
    std::cerr << except.full() << '\n';
    std::terminate();
  }
}

#include <list>

void file_storage_multithreading_test() {
  storage.checkHeap();
  std::clog << "Start multithreading test\n";
  srand(time(nullptr));
  std::list<std::thread> threads;
  for(int i = 0; i < 1000; ++i) {
    threads.emplace_back(reader);
    threads.emplace_back(writer);
  }
  for(std::thread& thr : threads)
    thr.join();

  std::clog << "Result: " << storage.getRoot().getVariable() << '\n';
  storage.check();
}

#endif // MULTITHREADING_TEST_H
