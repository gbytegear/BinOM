#ifndef MULTITHREADING_TEST_H
#define MULTITHREADING_TEST_H

#include "libbinom/include/file_storage/file_storage.h"
#include <thread>

using namespace binom;

class PerfomanceTest {
  clock_t start_time;
  const char* msg;
public:
  PerfomanceTest(const char* msg) : start_time(clock()), msg(msg) {}
  ~PerfomanceTest() {std::clog << msg << double( clock() - start_time ) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;}
};

binom::DynamicStorage storage("mthr_test.db",
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
  std::clog << "Start multithreading test\n";
  PerfomanceTest pt("End multithreading test: ");
  srand(time(nullptr));
  std::list<std::thread> threads;
  for(int i = 0; i < 5000; ++i) {
    threads.emplace_back(reader);
    threads.emplace_back(writer);
  }
  for(std::thread& thr : threads)
    thr.join();

//  std::clog << "Result: " << storage.getRoot().getVariable() << '\n';
}

#endif // MULTITHREADING_TEST_H
