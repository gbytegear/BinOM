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
  std::cout << root["test"].getVariable() << '\n';
}

void writer() {
  root["test"].pushBack(
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
        );
}

#include <list>

void test() {
  using namespace std::chrono_literals;
  std::list<std::thread> threads;
  for(int i = 0; i < 4; ++i) {
    threads.emplace_back(writer);
    threads.emplace_back(reader);
  }
  for(std::thread& thr : threads)
    thr.join();
}


#endif // MULTITHREAD_H
